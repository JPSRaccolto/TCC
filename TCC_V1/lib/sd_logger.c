/**
 * sd_logger.c — gravação contínua em CSV no microSD (backup),
 * usando a biblioteca FatFs_SPI do seu repositório MPU_SD.
 */
#include <stdio.h>
#include <string.h>

#include "ff.h"
#include "diskio.h"
#include "f_util.h"
#include "hw_config.h"
#include "config.h"
#include "sd_logger.h"
#include "pico/aon_timer.h"
#include "pico/util/datetime.h"
#include "pico/stdio.h"
#include "spi.h"

static FATFS s_fatfs;
static FIL   s_file;
static bool  s_mounted = false;
static bool  s_file_open = false;

bool sd_logger_init(void) {
    /* O driver SPI do cartao SD (FatFs_SPI) usa por padrao DMA_IRQ_0 com
     * handler exclusivo (irq_set_exclusive_handler). O driver WiFi do
     * Pico W (cyw43_arch) TAMBEM usa DMA_IRQ_0 internamente para a
     * comunicacao SPI/PIO com o chip CYW43439. Como os dois registram
     * handler exclusivo na mesma IRQ, quem inicializa por ULTIMO trava
     * (nao importa a ordem WiFi->SD ou SD->WiFi). A solucao e mover o
     * SD para DMA_IRQ_1, que fica livre. Precisa ser chamado ANTES do
     * primeiro acesso ao cartao (f_mount), que e quando o spi_init()
     * interno roda e reivindica a IRQ. */
    set_spi_dma_irq_channel(true, true);  /* true = usa DMA_IRQ_1 */

    printf("[SD] Antes de f_mount()...\n");
    stdio_flush();
    FRESULT fr = f_mount(&s_fatfs, "0:", 1);
    printf("[SD] f_mount() retornou\n");
    stdio_flush();
    if (fr != FR_OK) {
        printf("[SD] Falha ao montar cartao: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }
    s_mounted = true;

    /* Verifica se o arquivo já existe para decidir se escreve cabeçalho */
    FILINFO fno;
    bool arquivo_existe = (f_stat(SD_LOG_FILENAME, &fno) == FR_OK);

    fr = f_open(&s_file, SD_LOG_FILENAME, FA_WRITE | FA_OPEN_APPEND);
    if (fr != FR_OK) {
        printf("[SD] Falha ao abrir %s: %s (%d)\n", SD_LOG_FILENAME, FRESULT_str(fr), fr);
        return false;
    }
    s_file_open = true;

    if (!arquivo_existe) {
        const char header[] =
            "timestamp_ms,data,hora,I_L1_A,I_L2_A,I_L3_A,I_N_A,V_rede_V,"
            "P_L1_VA,P_L2_VA,P_L3_VA,P_total_VA\n";
        UINT bw;
        f_write(&s_file, header, strlen(header), &bw);
        f_sync(&s_file);
    }

    printf("[SD] Pronto. Gravando em %s\n", SD_LOG_FILENAME);
    return true;
}

void sd_logger_write(uint32_t timestamp_ms,
                      float i_l1, float i_l2, float i_l3, float i_n,
                      float v_rede) {
    if (!s_file_open) return;

    float p_l1 = i_l1 * v_rede;
    float p_l2 = i_l2 * v_rede;
    float p_l3 = i_l3 * v_rede;
    float p_total = p_l1 + p_l2 + p_l3;

    /* Lê data/hora atual do AON timer (gravado por ntp_sync.c). Se o
     * AON timer não estiver rodando por algum motivo, cai num valor
     * de fallback visível (00/00/0000 00:00:00) em vez de desalinhar
     * as colunas do CSV. */
    char data_str[11] = "00/00/0000";
    char hora_str[9]  = "00:00:00";

    struct timespec ts;
    if (aon_timer_get_time(&ts)) {
        time_t t = ts.tv_sec;
        struct tm tm_info;
        gmtime_r(&t, &tm_info);
        snprintf(data_str, sizeof(data_str), "%02d/%02d/%04d",
                 tm_info.tm_mday, tm_info.tm_mon + 1, tm_info.tm_year + 1900);
        snprintf(hora_str, sizeof(hora_str), "%02d:%02d:%02d",
                 tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec);
    }

    char linha[192];
    int n = snprintf(linha, sizeof(linha),
                      "%lu,%s,%s,%.4f,%.4f,%.4f,%.4f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                      (unsigned long)timestamp_ms,
                      data_str, hora_str,
                      i_l1, i_l2, i_l3, i_n, v_rede,
                      p_l1, p_l2, p_l3, p_total);

    UINT bw;
    FRESULT fr = f_write(&s_file, linha, (UINT)n, &bw);
    if (fr != FR_OK || bw != (UINT)n) {
        printf("[SD] Erro ao escrever linha: %s (%d)\n", FRESULT_str(fr), fr);
        return;
    }
    f_sync(&s_file); /* garante que os dados cheguem no cartão a cada linha */
}

void sd_logger_close(void) {
    if (s_file_open) {
        f_close(&s_file);
        s_file_open = false;
    }
    if (s_mounted) {
        f_unmount("0:");
        s_mounted = false;
    }
}

bool sd_logger_is_mounted(void) {
    return s_mounted;
}

bool sd_logger_is_logging(void) {
    return s_file_open;
}

bool sd_logger_unmount(void) {
    if (s_file_open) {
        f_close(&s_file);
        s_file_open = false;
    }
    if (s_mounted) {
        FRESULT fr = f_unmount("0:");
        if (fr != FR_OK) {
            printf("[SD] Erro ao desmontar: %s (%d)\n", FRESULT_str(fr), fr);
            return false;
        }
        s_mounted = false;
        printf("[SD] Desmontado com sucesso\n");
        return true;
    }
    return true;
}

bool sd_logger_mount(void) {
    if (s_mounted) {
        printf("[SD] Cartao ja esta montado\n");
        return true;
    }

    FRESULT fr = f_mount(&s_fatfs, "0:", 1);
    if (fr != FR_OK) {
        printf("[SD] Falha ao montar cartao: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }
    s_mounted = true;

    /* Verifica se o arquivo já existe */
    FILINFO fno;
    bool arquivo_existe = (f_stat(SD_LOG_FILENAME, &fno) == FR_OK);

    fr = f_open(&s_file, SD_LOG_FILENAME, FA_WRITE | FA_OPEN_APPEND);
    if (fr != FR_OK) {
        printf("[SD] Falha ao abrir %s: %s (%d)\n", SD_LOG_FILENAME, FRESULT_str(fr), fr);
        f_unmount("0:");
        s_mounted = false;
        return false;
    }
    s_file_open = true;

    if (!arquivo_existe) {
        const char header[] =
            "timestamp_ms,I_L1_A,I_L2_A,I_L3_A,I_N_A,V_rede_V,"
            "P_L1_VA,P_L2_VA,P_L3_VA,P_total_VA\n";
        UINT bw;
        f_write(&s_file, header, strlen(header), &bw);
        f_sync(&s_file);
    }

    printf("[SD] Pronto. Gravando em %s\n", SD_LOG_FILENAME);
    return true;
}