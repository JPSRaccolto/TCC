/**
 * main.c — Analisador de Qualidade de Energia (3 fases + neutro)
 * Raspberry Pi Pico 2 W — Pico SDK 2.x
 *
 * Une, num único pipeline multiplexado (CD74HC4067 -> ADC0/GP26):
 *   - 4x SCT013 (L1, L2, L3, N)   -> corrente RMS
 *   - 1x ZMPT101B                 -> tensão RMS da rede
 * e grava tudo em CSV no cartão microSD como backup (FatFs_SPI).
 *
 * O método de amostragem/calibração é o mesmo já validado nos seus
 * arquivos multiplexador.c e ZMPT101B_teste.c — ver medicao.c.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"

#include "config.h"
#include "medicao.h"
#include "sd_logger.h"
#include "oled_display.h"
#include "ntp_sync.h"
#include "base44_client.h"
#define NUM_CANAIS 4

/* ===== Botões via interrupção de GPIO =====
 * O loop principal fica bloqueado por ~2,3s por volta fazendo a
 * amostragem dos 4 canais (medir_dc + RMS), então uma leitura de botão
 * por polling (como antes) só é checada 1x a cada ~2,8s — um toque
 * normal já foi solto antes disso e é perdido. Por isso era preciso
 * segurar o botão por muito tempo. A interrupção resolve isso: o botão
 * é capturado no instante em que é pressionado, independente do que o
 * loop principal está fazendo (mesma lógica do seu Cartao_FatFS_SPI.c).
 */
static volatile bool button_next_flag  = false;
static volatile bool button_mount_flag = false;
static volatile uint32_t last_irq_next_us  = 0;
static volatile uint32_t last_irq_mount_us = 0;

/* Status do SD para display */
static sd_status_t current_sd_status = SD_STATUS_NOT_INIT;

/* Callback de interrupção - executa no instante do aperto do botão */
static void gpio_button_callback(uint gpio, uint32_t events) {
    uint32_t now_us = to_us_since_boot(get_absolute_time());

    if (gpio == BUTTON_NEXT_PIN) {
        if (now_us - last_irq_next_us >= (BUTTON_DEBOUNCE_MS * 1000)) {
            last_irq_next_us = now_us;
            button_next_flag = true;
            /* Troca a página aqui mesmo: é só uma variável, não bloqueia.
             * Assim a página muda no instante do toque, mesmo que o
             * redesenho na tela só aconteça no próximo ciclo do loop. */
            oled_display_next_page();
        }
    } else if (gpio == BUTTON_MOUNT_PIN) {
        if (now_us - last_irq_mount_us >= (BUTTON_DEBOUNCE_MS * 1000)) {
            last_irq_mount_us = now_us;
            button_mount_flag = true;
        }
    }
}

/* Processa as flags setadas pela interrupção. Chamado no loop principal.
 * Operações de SD (montar/desmontar) são bloqueantes, por isso NÃO são
 * feitas dentro da interrupção — só a flag é setada lá. */
static void handle_buttons(void) {
    if (button_next_flag) {
        button_next_flag = false;
        printf("[BOTAO_NEXT] Proxima pagina\n");
    }

    if (button_mount_flag) {
        button_mount_flag = false;
        printf("[BOTAO_MOUNT] Montar/Desmontar SD\n");

        /* Alterna entre montado e desmontado */
        if (current_sd_status == SD_STATUS_MOUNTED || current_sd_status == SD_STATUS_LOGGING) {
            current_sd_status = SD_STATUS_UNMOUNTING;
            if (sd_logger_unmount()) {
                current_sd_status = SD_STATUS_NOT_INIT;
            } else {
                current_sd_status = SD_STATUS_FAILED;
            }
        } else {
            current_sd_status = SD_STATUS_MOUNTING;
            if (sd_logger_mount()) {
                current_sd_status = SD_STATUS_MOUNTED;
            } else {
                current_sd_status = SD_STATUS_FAILED;
            }
        }
    }
}

int main(void) {
    stdio_usb_init();
    for (int i = 0; i < 50 && !stdio_usb_connected(); i++) {
        sleep_ms(100);
    }

    printf("\n");
    printf("================================================\n");
    printf("  Analisador de Qualidade de Energia            \n");
    printf("  Raspberry Pi Pico 2 W                         \n");
    printf("  3x SCT013 (fases) + 1x SCT013 (neutro)        \n");
    printf("  1x ZMPT101B (tensao) via MUX CD74HC4067       \n");
    printf("================================================\n\n");

    medicao_init();
    oled_display_init();

    /* Inicializa botões em GP0/GP1 (UART pins - forçar como GPIO) */
    gpio_init(BUTTON_NEXT_PIN);
    gpio_set_function(BUTTON_NEXT_PIN, GPIO_FUNC_SIO);  /* Forçar GPIO ao invés de UART */
    gpio_set_dir(BUTTON_NEXT_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_NEXT_PIN);
    
    gpio_init(BUTTON_MOUNT_PIN);
    gpio_set_function(BUTTON_MOUNT_PIN, GPIO_FUNC_SIO); /* Forçar GPIO ao invés de UART */
    gpio_set_dir(BUTTON_MOUNT_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_MOUNT_PIN);
    
    sleep_ms(100);  /* Aguardar estabilização */

    /* Com pull-down, 1 = pressionado -> interrompe na borda de subida.
     * O primeiro registro define o callback para o núcleo; o segundo
     * pino só precisa habilitar o evento (mesmo callback é usado). */
    gpio_set_irq_enabled_with_callback(BUTTON_NEXT_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_button_callback);
    gpio_set_irq_enabled(BUTTON_MOUNT_PIN, GPIO_IRQ_EDGE_RISE, true);

    canal_t canais[NUM_CANAIS] = {
        { .nome = "L1",     .mux_ch = MUX_CH_L1,      .tipo = CANAL_CORRENTE },
        { .nome = "L2",     .mux_ch = MUX_CH_L2,      .tipo = CANAL_CORRENTE },
        { .nome = "L3",     .mux_ch = MUX_CH_L3,      .tipo = CANAL_CORRENTE },
        //{ .nome = "N",      .mux_ch = MUX_CH_N,       .tipo = CANAL_CORRENTE },
        { .nome = "Tensao", .mux_ch = MUX_CH_VOLTAGE, .tipo = CANAL_TENSAO  },
    };

    printf("Aguardando estabilizacao do circuito e do MUX...\n");
    sleep_ms(1000);

    printf("\n--- CALIBRACAO ---\n");
    printf("Mantenha os SCT013 SEM corrente (nenhuma carga ligada nas\n");
    printf("fases medidas) durante esta etapa. O ZMPT101B pode ficar\n");
    printf("ligado normalmente na rede.\n\n");
    for (int i = 0; i < NUM_CANAIS; i++) {
        medicao_calibrar_canal(&canais[i]);
    }
    printf("--- Calibracao concluida ---\n\n");
    /* MUDE ESSAS CREDENCIAIS PARA SUAS REDES WIFI! */
    const char *wifi_ssid = "computador";
    const char *wifi_password = "12345678";
    printf("Aguardando estabilizacao do sistema (%d ms) antes de iniciar...\n\n",
           CALIBRATION_SETTLE_MS);
    sleep_ms(CALIBRATION_SETTLE_MS);

    /* 1) Monta o SD primeiro — SEM WiFi ativo, evita disputa de IRQ/timing
     * com a inicializacao SPI do cartao. */
if (sd_logger_init()) {
        current_sd_status = SD_STATUS_MOUNTED;
        printf("[SD] Cartao montado com sucesso e pronto para gravar.\n\n");
    } else {
        current_sd_status = SD_STATUS_FAILED;
        printf("[AVISO] Sistema vai continuar SEM backup em SD.\n");
        printf("        Verifique montagem/pinos do cartao (config.h).\n\n");
    }
    stdio_flush();

    printf("\n================================================\n");
    printf("  Conectando WiFi / Sincronizando Hora\n");
    printf("================================================\n");
    stdio_flush();

    bool base44_ready = base44_client_connect_wifi(WIFI_SSID, WIFI_PASSWORD);
    bool ntp_success = false;

    if (!base44_ready) {
        printf("[MAIN] WiFi indisponivel — sem NTP e sem envio ao Base44.\n");
    } else {
        ntp_success = ntp_sync_time_wifi_connected();
        if (ntp_success) {
            printf("[MAIN] Hora sincronizada via NTP com sucesso\n\n");
        } else {
            printf("[MAIN] Hora configurada manualmente (ou padrão)\n\n");
        }
    }

    printf("%-8s %-10s %-10s %-10s %-10s %-10s\n",
           "Canal", "Valor", "Vrms AC", "DC (V)", "Unidade", "Status");
    printf("%-8s %-10s %-10s %-10s %-10s %-10s\n",
           "-----", "-----", "-------", "------", "-------", "------");

    uint32_t proximo_log_ms = to_ms_since_boot(get_absolute_time());
    uint32_t proximo_envio_base44_ms = to_ms_since_boot(get_absolute_time());
    while (true) {
        /* Processa botões PRIMEIRO - máxima responsividade */
        handle_buttons();
        
        uint32_t agora_ms = to_ms_since_boot(get_absolute_time());
        float i_l1 = 0, i_l2 = 0, i_l3 = 0, i_n = 0, v_rede = 0;

        for (int i = 0; i < NUM_CANAIS; i++) {
            leitura_t leitura = medicao_ler_canal(&canais[i]);
            const char *unidade = (canais[i].tipo == CANAL_CORRENTE) ? "A" : "V";

            const char *status;
            if (canais[i].tipo == CANAL_CORRENTE) {
                if (leitura.valor == 0.0f)       status = "SEM CARGA";
                else if (leitura.valor < 1.0f)   status = "LEVE";
                else if (leitura.valor < 10.0f)  status = "NORMAL";
                else if (leitura.valor < 50.0f)  status = "ALTO";
                else                             status = "CRITICO";
            } else {
                if (leitura.valor < 10.0f)        status = "SEM REDE";
                else if (leitura.valor > 250.0f)  status = "ALERTA";
                else                              status = "OK";
            }

            printf("%-8s %-6.3f     %-7.4f    %-7.4f    %-10s %s\n",
                   canais[i].nome, leitura.valor, leitura.vrms_ac,
                   leitura.dc_v, unidade, status);

            if (canais[i].mux_ch == MUX_CH_L1) i_l1 = leitura.valor;
            else if (canais[i].mux_ch == MUX_CH_L2) i_l2 = leitura.valor;
            else if (canais[i].mux_ch == MUX_CH_L3) i_l3 = leitura.valor;
            //else if (canais[i].mux_ch == MUX_CH_N)  i_n  = leitura.valor;
            else if (canais[i].mux_ch == MUX_CH_VOLTAGE) v_rede = leitura.valor;
        }
        printf("\n");

        /* GRAVAÇÃO NO SD — se arquivo está aberto e chegou a hora */
        if (sd_logger_is_logging() && agora_ms >= proximo_log_ms) {
            sd_logger_write(agora_ms, i_l1, i_l2, i_l3, i_n, v_rede);
            proximo_log_ms = agora_ms + SD_LOG_INTERVAL_MS;
        }

        /* Atualiza status de gravação DEPOIS da escrita */
        if (current_sd_status == SD_STATUS_MOUNTED && sd_logger_is_logging()) {
            current_sd_status = SD_STATUS_LOGGING;
        }
        if (base44_ready && agora_ms >= proximo_envio_base44_ms) {
            char ts[24];
            base44_format_timestamp(ts, sizeof(ts));   /* declare como não-static no .c e exponha no .h se preferir, ou gere o timestamp dentro do proprio send_bulk */
            base44_reading_t leituras[] = {
                { "F1", v_rede, i_l1 },
                { "F2", v_rede, i_l2 },
                { "F3", v_rede, i_l3 },
            };
            base44_client_send_bulk(ts, leituras, 3);
            proximo_envio_base44_ms = agora_ms + BASE44_SEND_INTERVAL_MS;
        }
        oled_display_show(i_l1, i_l2, i_l3, i_n, v_rede, current_sd_status);

        sleep_ms(READ_INTERVAL_MS);
    }

    sd_logger_close();
    return 0;
}