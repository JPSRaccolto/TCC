/**
 * ntp_sync.c — Sincronização de hora via NTP (com fallback HTTP)
 * Raspberry Pi Pico 2 W com WiFi e lwIP
 *
 * Usa pico_aon_timer (API nova do SDK 2.x, funciona no RP2040 e no
 * RP2350) no lugar da antiga hardware_rtc, que só existe pro RP2040.
 *
 * Estratégia de sincronização, em ordem:
 *   1. SNTP (UDP porta 123) — padrão, mas algumas redes (ex.: redes
 *      institucionais/universitárias) bloqueiam essa porta por
 *      política de segurança.
 *   2. HTTP (porta 80) — lê o cabeçalho "Date:" de uma resposta HTTP
 *      comum. Praticamente nenhuma rede bloqueia a porta 80, então
 *      isso funciona como fallback confiável quando o SNTP falha.
 *   3. Entrada manual via serial — último recurso.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/aon_timer.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/sntp.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "ntp_sync.h"

#define SNTP_TIMEOUT_MS 10000  /* timeout de 10s pra sincronizar via NTP */

#define HTTP_TIME_HOST    "ntp.br"
#define HTTP_TIME_PORT    80
#define HTTP_RESP_BUF_LEN 512
#define HTTP_TIMEOUT_MS   8000
#define HTTP_DNS_TIMEOUT_MS 5000

/* Horário de Brasília = UTC-3 (sem horário de verão desde 2019).
 * SNTP e o cabeçalho HTTP "Date:" sempre vêm em UTC/GMT, então
 * aplicamos esse deslocamento antes de gravar no AON timer. */
#define BRAZIL_UTC_OFFSET_SEC (-3 * 3600L)

static volatile bool sntp_time_received = false;

/**
 * Converte um struct tm em UTC para epoch Unix (segundos desde
 * 1970-01-01 00:00:00 UTC), sem depender de timegm()/mktime() (que
 * têm comportamento de fuso horário nem sempre confiável em libc
 * embarcada). Implementação autocontida, cobre 1970-2099.
 */
static time_t utc_tm_to_epoch(const struct tm *tm) {
    static const int cumdays[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    int year  = tm->tm_year + 1900;
    int month = tm->tm_mon;   /* 0-11 */
    int day   = tm->tm_mday;

    long days = (long)(year - 1970) * 365L;
    /* anos bissextos completos desde 1970 até o ano anterior */
    days += (year - 1969) / 4 - (year - 1901) / 100 + (year - 1601) / 400;
    days += cumdays[month];
    if (month > 1 &&
        ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) {
        days += 1; /* ano bissexto e já passamos fevereiro */
    }
    days += day - 1;

    time_t secs = (time_t)days * 86400L;
    secs += tm->tm_hour * 3600L;
    secs += tm->tm_min * 60L;
    secs += tm->tm_sec;

    return secs;
}

/**
 * Parseia string "YYYY-MM-DD HH:MM:SS" e preenche struct tm
 */
static bool parse_datetime(const char *str, struct tm *tm_out) {
    int year, month, day, hour, minute, second;

    int result = sscanf(str, "%d-%d-%d %d:%d:%d",
                        &year, &month, &day, &hour, &minute, &second);

    if (result != 6) {
        return false;
    }

    memset(tm_out, 0, sizeof(*tm_out));
    tm_out->tm_year = year - 1900;   /* struct tm conta anos a partir de 1900 */
    tm_out->tm_mon  = month - 1;     /* struct tm conta meses de 0 a 11 */
    tm_out->tm_mday = day;
    tm_out->tm_hour = hour;
    tm_out->tm_min  = minute;
    tm_out->tm_sec  = second;
    tm_out->tm_isdst = -1;           /* não sabemos, deixa indefinido */

    return true;
}

/**
 * Lê data/hora do usuário via serial
 */
static bool read_datetime_from_user(struct tm *tm_out) {
    char input[30];

    printf("\n[NTP] WiFi/SNTP/HTTP não disponíveis.\n");
    printf("[NTP] Digite a data e hora manualmente:\n");
    printf("[NTP] Formato: YYYY-MM-DD HH:MM:SS\n");
    printf("[NTP] Exemplo: 2026-07-04 14:30:00\n");
    printf("[NTP] Entrada: ");
    fflush(stdout);

    /* Lê até 29 caracteres ou newline */
    int i = 0;
    while (i < (int)sizeof(input) - 1) {
        int c = getchar();
        if (c < 0) {
            /* timeout ou erro */
            return false;
        }
        if (c == '\n' || c == '\r') {
            break;
        }
        input[i++] = (char)c;
    }
    input[i] = '\0';

    printf("\n");

    if (!parse_datetime(input, tm_out)) {
        printf("[NTP] Formato inválido!\n");
        return false;
    }

    printf("[NTP] Data/hora lida: %04d-%02d-%02d %02d:%02d:%02d\n",
           tm_out->tm_year + 1900, tm_out->tm_mon + 1, tm_out->tm_mday,
           tm_out->tm_hour, tm_out->tm_min, tm_out->tm_sec);

    return true;
}

/**
 * Grava um struct tm no AON timer, iniciando-o se ainda não estiver
 * rodando, ou apenas ajustando a hora se já estiver.
 */
static void aon_set_from_tm(struct tm *tm_in) {
    if (aon_timer_is_running()) {
        aon_timer_set_time_calendar(tm_in);
    } else {
        aon_timer_start_calendar(tm_in);
    }
}

/**
 * Chamada pelo lwIP através da macro SNTP_SET_SYSTEM_TIME (definida em
 * lwipopts.h) no instante em que uma resposta NTP válida chega. É esse
 * ponto que de fato grava o horário recebido no AON timer.
 */
void ntp_sync_set_system_time(uint32_t sec) {
    time_t utc_epoch = (time_t)sec;
    time_t local_epoch = utc_epoch + BRAZIL_UTC_OFFSET_SEC;

    struct tm tm_info;
    gmtime_r(&local_epoch, &tm_info);

    aon_set_from_tm(&tm_info);
    sntp_time_received = true;

    printf("[NTP] Hora recebida via SNTP (Brasília): %04d-%02d-%02d %02d:%02d:%02d\n",
           tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
           tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec);
}

/**
 * Tenta sincronizar via SNTP (UDP/123). Assume que o WiFi já está
 * conectado. Retorna true se conseguiu.
 */
static bool try_sntp_sync(void) {
    printf("[NTP] Iniciando sincronização SNTP (UDP/123)...\n");
    sntp_setservername(0, "pool.ntp.org");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_init();

    sntp_time_received = false;
    uint32_t start_ms = to_ms_since_boot(get_absolute_time());

    while (!sntp_time_received) {
        cyw43_arch_poll();

        if (to_ms_since_boot(get_absolute_time()) - start_ms > SNTP_TIMEOUT_MS) {
            printf("[NTP] Timeout SNTP (%u ms) — porta UDP/123 pode estar bloqueada nesta rede\n",
                   SNTP_TIMEOUT_MS);
            break;
        }

        sleep_ms(10);
    }

    sntp_stop();

    return sntp_time_received;
}

/* ================= Fallback via HTTP (porta 80) ================= */

typedef struct {
    char buf[HTTP_RESP_BUF_LEN];
    size_t len;
    bool done;
    bool error;
} http_time_ctx_t;

static err_t http_time_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    http_time_ctx_t *ctx = (http_time_ctx_t *)arg;

    if (!p) {
        /* Servidor fechou a conexão: resposta completa */
        ctx->done = true;
        tcp_close(tpcb);
        return ERR_OK;
    }

    if (err == ERR_OK) {
        size_t copy_len = p->tot_len;
        size_t space_left = (HTTP_RESP_BUF_LEN - 1) - ctx->len;
        if (copy_len > space_left) {
            copy_len = space_left;
        }
        if (copy_len > 0) {
            pbuf_copy_partial(p, ctx->buf + ctx->len, copy_len, 0);
            ctx->len += copy_len;
            ctx->buf[ctx->len] = '\0';
        }
        tcp_recved(tpcb, p->tot_len);
    }

    pbuf_free(p);
    return ERR_OK;
}

static err_t http_time_connected_cb(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        http_time_ctx_t *ctx = (http_time_ctx_t *)arg;
        ctx->error = true;
        ctx->done = true;
        return err;
    }

    static const char req_fmt[] =
        "HEAD / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";
    char req[128];
    int req_len = snprintf(req, sizeof(req), req_fmt, HTTP_TIME_HOST);

    tcp_write(tpcb, req, (u16_t)req_len, TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    return ERR_OK;
}

static void http_time_err_cb(void *arg, err_t err) {
    (void)err;
    http_time_ctx_t *ctx = (http_time_ctx_t *)arg;
    ctx->error = true;
    ctx->done = true;
}

static volatile bool http_dns_done = false;
static ip_addr_t http_resolved_addr;

static void http_dns_cb(const char *name, const ip_addr_t *ipaddr, void *arg) {
    (void)name;
    (void)arg;
    if (ipaddr) {
        http_resolved_addr = *ipaddr;
    }
    http_dns_done = true;
}

/**
 * Parseia o cabeçalho "Date: <dia-da-semana>, DD Mon YYYY HH:MM:SS GMT"
 * de uma resposta HTTP e grava a hora no AON timer.
 */
static bool parse_http_date_header(const char *resp) {
    static const char *month_names[12] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    const char *p = strstr(resp, "Date:");
    if (!p) {
        printf("[HTTP-TIME] Cabeçalho 'Date:' não encontrado na resposta\n");
        return false;
    }
    p += strlen("Date:");
    while (*p == ' ') p++;

    char day_name[8], mon_name[8];
    int day, year, hour, minute, second;

    /* Formato RFC 1123: "Fri, 04 Jul 2026 14:23:01 GMT" */
    int n = sscanf(p, "%7[^,], %d %7s %d %d:%d:%d",
                   day_name, &day, mon_name, &year, &hour, &minute, &second);

    if (n != 7) {
        printf("[HTTP-TIME] Falha ao interpretar data: '%.40s'\n", p);
        return false;
    }

    int month = -1;
    for (int i = 0; i < 12; i++) {
        if (strncmp(mon_name, month_names[i], 3) == 0) {
            month = i;
            break;
        }
    }
    if (month < 0) {
        printf("[HTTP-TIME] Mês inválido: '%s'\n", mon_name);
        return false;
    }

    struct tm tm_info;
    memset(&tm_info, 0, sizeof(tm_info));
    tm_info.tm_year = year - 1900;
    tm_info.tm_mon  = month;
    tm_info.tm_mday = day;
    tm_info.tm_hour = hour;
    tm_info.tm_min  = minute;
    tm_info.tm_sec  = second;
    tm_info.tm_isdst = -1;

    /* O cabeçalho Date: vem em UTC/GMT — converte para epoch, aplica
     * o deslocamento de Brasília e decompõe de volta em struct tm. */
    time_t utc_epoch = utc_tm_to_epoch(&tm_info);
    time_t local_epoch = utc_epoch + BRAZIL_UTC_OFFSET_SEC;

    struct tm local_tm;
    gmtime_r(&local_epoch, &local_tm);

    aon_set_from_tm(&local_tm);

    printf("[HTTP-TIME] Hora sincronizada via HTTP (Brasília): %04d-%02d-%02d %02d:%02d:%02d\n",
           local_tm.tm_year + 1900, local_tm.tm_mon + 1, local_tm.tm_mday,
           local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec);

    return true;
}

/**
 * Tenta sincronizar a hora fazendo uma requisição HTTP simples e lendo
 * o cabeçalho "Date:" da resposta. Assume que o WiFi já está conectado.
 */
static bool try_http_sync(void) {
    printf("[NTP] Tentando fallback via HTTP (porta 80), host '%s'...\n", HTTP_TIME_HOST);

    http_dns_done = false;
    err_t dns_err = dns_gethostbyname(HTTP_TIME_HOST, &http_resolved_addr, http_dns_cb, NULL);

    if (dns_err == ERR_INPROGRESS) {
        uint32_t start_ms = to_ms_since_boot(get_absolute_time());
        while (!http_dns_done &&
               (to_ms_since_boot(get_absolute_time()) - start_ms) < HTTP_DNS_TIMEOUT_MS) {
            cyw43_arch_poll();
            sleep_ms(10);
        }
        if (!http_dns_done) {
            printf("[HTTP-TIME] Timeout ao resolver '%s'\n", HTTP_TIME_HOST);
            return false;
        }
    } else if (dns_err != ERR_OK) {
        printf("[HTTP-TIME] Erro ao resolver '%s' (código %d)\n", HTTP_TIME_HOST, dns_err);
        return false;
    }

    http_time_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));

    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("[HTTP-TIME] Falha ao criar socket TCP\n");
        return false;
    }

    tcp_arg(pcb, &ctx);
    tcp_recv(pcb, http_time_recv_cb);
    tcp_err(pcb, http_time_err_cb);

    err_t c_err = tcp_connect(pcb, &http_resolved_addr, HTTP_TIME_PORT, http_time_connected_cb);
    if (c_err != ERR_OK) {
        printf("[HTTP-TIME] Erro ao conectar (código %d)\n", c_err);
        return false;
    }

    uint32_t start_ms = to_ms_since_boot(get_absolute_time());
    while (!ctx.done && (to_ms_since_boot(get_absolute_time()) - start_ms) < HTTP_TIMEOUT_MS) {
        cyw43_arch_poll();
        sleep_ms(10);
    }

    if (!ctx.done) {
        printf("[HTTP-TIME] Timeout na requisição HTTP\n");
        return false;
    }
    if (ctx.error) {
        printf("[HTTP-TIME] Erro na conexão/requisição HTTP\n");
        return false;
    }

    return parse_http_date_header(ctx.buf);
}
bool ntp_sync_time_wifi_connected(void) {
    printf("\n================================================\n");
    printf("  Sincronização de Hora (NTP / HTTP)\n");
    printf("================================================\n\n");

    /* 1) Tenta via SNTP (UDP/123) */
    if (try_sntp_sync()) {
        printf("[NTP] Sincronização via SNTP concluída com sucesso!\n\n");
        return true;
    }

    /* 2) Fallback via HTTP (porta 80) */
    if (try_http_sync()) {
        printf("[NTP] Sincronização via HTTP concluída com sucesso!\n\n");
        return true;
    }

    printf("[NTP] SNTP e HTTP falharam, tentando entrada manual...\n");

    struct tm tm_manual;
    if (!read_datetime_from_user(&tm_manual)) {
        printf("[NTP] Erro ao ler data/hora do usuário\n");
        printf("[NTP] Usando padrão: 2026-01-01 00:00:00\n");
        memset(&tm_manual, 0, sizeof(tm_manual));
        tm_manual.tm_year = 2026 - 1900;
        tm_manual.tm_mon  = 0;
        tm_manual.tm_mday = 1;
        tm_manual.tm_isdst = -1;
    }

    aon_set_from_tm(&tm_manual);
    printf("[NTP] Hora configurada: %04d-%02d-%02d %02d:%02d:%02d\n\n",
           tm_manual.tm_year + 1900, tm_manual.tm_mon + 1, tm_manual.tm_mday,
           tm_manual.tm_hour, tm_manual.tm_min, tm_manual.tm_sec);

    return false;
}
/**
 * Função principal de sincronização.
 * Ordem: SNTP -> HTTP -> entrada manual.
 */
bool ntp_sync_time(const char *wifi_ssid, const char *wifi_password) {
    printf("\n================================================\n");
    printf("  Sincronização de Hora (NTP / HTTP)\n");
    printf("================================================\n\n");

    /* Tenta conectar ao WiFi */
    printf("[NTP] Inicializando WiFi...\n");
    if (cyw43_arch_init()) {
        printf("[NTP] ERRO ao inicializar WiFi (cyw43_arch_init)\n");
        printf("[NTP] Tentando sincronizar via entrada manual...\n");
        goto manual_time;
    }

    cyw43_arch_enable_sta_mode();

    printf("[NTP] Conectando ao WiFi '%s'...\n", wifi_ssid);
    {
        int wifi_result = cyw43_arch_wifi_connect_timeout_ms(
            wifi_ssid,
            wifi_password,
            CYW43_AUTH_WPA2_AES_PSK,
            10000  /* timeout 10s */
        );

        if (wifi_result != 0) {
            printf("[NTP] ERRO: Falha ao conectar ao WiFi (código %d)\n", wifi_result);
            printf("[NTP] Tentando sincronizar via entrada manual...\n");
            cyw43_arch_deinit();
            goto manual_time;
        }
    }

    printf("[NTP] WiFi conectado!\n");
    printf("[NTP] IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));

    /* 1) Tenta via SNTP (UDP/123) */
    if (try_sntp_sync()) {
        cyw43_arch_deinit();
        printf("[NTP] Sincronização via SNTP concluída com sucesso!\n\n");
        return true;
    }

    /* 2) Se SNTP falhou (ex.: porta UDP/123 bloqueada pela rede),
     *    tenta via HTTP (porta 80) */
    if (try_http_sync()) {
        cyw43_arch_deinit();
        printf("[NTP] Sincronização via HTTP concluída com sucesso!\n\n");
        return true;
    }

    cyw43_arch_deinit();
    printf("[NTP] SNTP e HTTP falharam, tentando entrada manual...\n");

manual_time:
    {
        struct tm tm_manual;

        /* Tenta ler data/hora do usuário */
        if (!read_datetime_from_user(&tm_manual)) {
            printf("[NTP] Erro ao ler data/hora do usuário\n");
            printf("[NTP] Usando padrão: 2026-01-01 00:00:00\n");
            memset(&tm_manual, 0, sizeof(tm_manual));
            tm_manual.tm_year = 2026 - 1900;
            tm_manual.tm_mon  = 0;
            tm_manual.tm_mday = 1;
            tm_manual.tm_hour = 0;
            tm_manual.tm_min  = 0;
            tm_manual.tm_sec  = 0;
            tm_manual.tm_isdst = -1;
        }

        aon_set_from_tm(&tm_manual);

        printf("[NTP] Hora configurada: %04d-%02d-%02d %02d:%02d:%02d\n\n",
               tm_manual.tm_year + 1900, tm_manual.tm_mon + 1, tm_manual.tm_mday,
               tm_manual.tm_hour, tm_manual.tm_min, tm_manual.tm_sec);

        return false;  /* Indica que foi manual, não automático */
    }
}