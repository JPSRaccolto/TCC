#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/dns.h"

#include "config.h"
#include "base44_client.h"
#include "mbedtls/platform_time.h"
#include "pico/aon_timer.h"
#include <time.h>
/* Exigido por MBEDTLS_PLATFORM_MS_TIME_ALT: o mbedtls não tem como
 * obter tempo de sistema em bare-metal, então fornecemos usando o
 * relógio interno do Pico (tempo desde o boot, em ms). Não precisa
 * ser hora real — o mbedtls só usa isso para medir timeouts/tickets
 * internos do TLS, não para validar certificados. */
mbedtls_ms_time_t mbedtls_ms_time(void) {
    return (mbedtls_ms_time_t)to_ms_since_boot(get_absolute_time());
}

#define BASE44_PORT           443
#define BASE44_TIMEOUT_S      15
#define BASE44_RESP_BUF_LEN   1536
#define BASE44_BODY_BUF_LEN   1024
#define BASE44_REQ_BUF_LEN    (BASE44_BODY_BUF_LEN + 512)

typedef struct {
    struct altcp_pcb *pcb;
    bool complete;
    bool ok;
    char resp[BASE44_RESP_BUF_LEN];
    size_t resp_len;
    int timeout_s;
} base44_ctx_t;

static struct altcp_tls_config *s_tls_config = NULL;
static bool s_wifi_connected = false;
static char s_req_buf[BASE44_REQ_BUF_LEN];

static err_t base44_close(base44_ctx_t *ctx) {
    err_t err = ERR_OK;
    ctx->complete = true;
    if (ctx->pcb) {
        altcp_arg(ctx->pcb, NULL);
        altcp_poll(ctx->pcb, NULL, 0);
        altcp_recv(ctx->pcb, NULL);
        altcp_err(ctx->pcb, NULL);
        err = altcp_close(ctx->pcb);
        if (err != ERR_OK) {
            altcp_abort(ctx->pcb);
            err = ERR_ABRT;
        }
        ctx->pcb = NULL;
    }
    return err;
}

static err_t base44_recv_cb(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err) {
    base44_ctx_t *ctx = (base44_ctx_t *)arg;
    if (!p) return base44_close(ctx);

    if (err == ERR_OK && p->tot_len > 0) {
        size_t space = (BASE44_RESP_BUF_LEN - 1) - ctx->resp_len;
        size_t copy_len = p->tot_len < space ? p->tot_len : space;
        if (copy_len > 0) {
            pbuf_copy_partial(p, ctx->resp + ctx->resp_len, copy_len, 0);
            ctx->resp_len += copy_len;
            ctx->resp[ctx->resp_len] = '\0';
        }
        altcp_recved(pcb, p->tot_len);
    }
    pbuf_free(p);
    return ERR_OK;
}

static void base44_err_cb(void *arg, err_t err) {
    base44_ctx_t *ctx = (base44_ctx_t *)arg;
    printf("[BASE44] err_cb disparado, err=%d\n", err);
    ctx->complete = true;
    ctx->ok = false;
}

static err_t base44_poll_cb(void *arg, struct altcp_pcb *pcb) {
    (void)pcb;
    return base44_close((base44_ctx_t *)arg);
}

static err_t base44_connected_cb(void *arg, struct altcp_pcb *pcb, err_t err) {
    base44_ctx_t *ctx = (base44_ctx_t *)arg;
    printf("[BASE44] connected_cb chamado, err=%d\n", err);
    if (err != ERR_OK) return base44_close(ctx);

    if (altcp_write(pcb, s_req_buf, strlen(s_req_buf), TCP_WRITE_FLAG_COPY) != ERR_OK) {
        printf("[BASE44] altcp_write() falhou\n");
        return base44_close(ctx);
    }
    altcp_output(pcb);
    return ERR_OK;
}

static void base44_connect_ip(const ip_addr_t *ip, base44_ctx_t *ctx) {
    printf("[BASE44] Conectando ao IP %s : %d\n", ip4addr_ntoa(ip), BASE44_PORT);
    err_t err = altcp_connect(ctx->pcb, ip, BASE44_PORT, base44_connected_cb);
    if (err != ERR_OK) {
        printf("[BASE44] altcp_connect() falhou, err=%d\n", err);
        base44_close(ctx);
    }
}

static void base44_dns_cb(const char *name, const ip_addr_t *ip, void *arg) {
    base44_ctx_t *ctx = (base44_ctx_t *)arg;
    if (ip) {
        printf("[BASE44] DNS resolveu %s -> %s\n", name, ip4addr_ntoa(ip));
        base44_connect_ip(ip, ctx);
    } else {
        printf("[BASE44] DNS falhou para %s\n", name);
        base44_close(ctx);
    }
}

bool base44_client_connect_wifi(const char *ssid, const char *password) {
    if (s_wifi_connected) return true;

    printf("[BASE44] Antes de cyw43_arch_init()...\n");
    stdio_flush();

    if (cyw43_arch_init()) {
        printf("[BASE44] ERRO ao inicializar WiFi\n");
        return false;
    }

    printf("[BASE44] cyw43_arch_init() OK\n");
    stdio_flush();
    cyw43_arch_enable_sta_mode();

    printf("[BASE44] Conectando ao WiFi '%s'...\n", ssid);
    int r = -1;
    for (int tentativa = 1; tentativa <= 3; tentativa++) {
        r = cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 10000);
        if (r == 0) break;
        printf("[BASE44] Tentativa %d/3 falhou (codigo %d)%s\n",
               tentativa, r, tentativa < 3 ? ", tentando novamente..." : "");
        if (tentativa < 3) sleep_ms(2000);
    }
    if (r != 0) {
        printf("[BASE44] Falha ao conectar ao WiFi apos 3 tentativas (codigo %d)\n", r);
        cyw43_arch_deinit();
        return false;
    }

    printf("[BASE44] WiFi conectado. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));

    /* Desliga o power-save do radio. Por padrao o CYW43439 pulsa o
     * radio (liga/desliga em ciclos) pra economizar energia, o que
     * gera picos de corrente na mesma alimentacao de 3,3V usada pelo
     * cartao SD. Como a inicializacao do SD e sensivel a tempo (exige
     * pulsos de clock regulares logo no inicio), um pico de corrente
     * bem nessa hora pode travar a montagem do cartao. Modo continuo
     * evita esses picos. */
    cyw43_wifi_pm(&cyw43_state, CYW43_NO_POWERSAVE_MODE);

    s_wifi_connected = true;
    return true;
}
/* Formata o horário atual do AON timer como "YYYY-MM-DD HH:MM:SS".
 * Atenção: o AON timer guarda hora local de Brasília (UTC-3), não UTC —
 * é assim que o ntp_sync.c grava. Se o Base44 esperar UTC estrito,
 * ajuste ou documente esse deslocamento na thesis. */
void base44_format_timestamp(char *buf, size_t len) {
    struct timespec ts;
    if (aon_timer_get_time(&ts)) {
        time_t t = ts.tv_sec;
        struct tm tm_info;
        gmtime_r(&t, &tm_info);
        snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d",
                 tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
                 tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec);
    } else {
        snprintf(buf, len, "2026-01-01 00:00:00");
    }
}
bool base44_client_send_bulk(const char *timestamp_iso, const base44_reading_t *readings, int count) {
    if (!s_wifi_connected || count <= 0) {
        printf("[BASE44] WiFi nao conectado ou lote vazio, ignorando envio\n");
        return false;
    }

    char body[BASE44_BODY_BUF_LEN];
    int pos = snprintf(body, sizeof(body), "[");
    for (int i = 0; i < count && pos < (int)sizeof(body) - 1; i++) {
        pos += snprintf(body + pos, sizeof(body) - pos,
            "%s{\"timestamp\":\"%s\",\"phase\":\"%s\",\"voltage_v\":%.2f,\"current_a\":%.3f}",
            (i == 0) ? "" : ",", timestamp_iso, readings[i].phase,
            readings[i].voltage_v, readings[i].current_a);
    }
    pos += snprintf(body + pos, sizeof(body) - pos, "]");
    int body_len = pos;
    

    snprintf(s_req_buf, sizeof(s_req_buf),
        "POST /api/entities/PhaseReading/bulk HTTP/1.1\r\n"
        "Host: %s\r\n"
        "api_key: %s\r\n"
        "appId: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        BASE44_HOST, BASE44_API_KEY, BASE44_APP_ID, body_len, body);

    base44_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.timeout_s = BASE44_TIMEOUT_S;
    ctx.ok = true;

    if (!s_tls_config) {
        /* Sem verificacao de certificado CA (adequado para um projeto
         * de bancada / TCC; nao usar assim em producao real). */
        s_tls_config = altcp_tls_create_config_client(NULL, 0);
        if (!s_tls_config) {
            printf("[BASE44] Falha ao criar config TLS\n");
            return false;
        }
    }

    ctx.pcb = altcp_tls_new(s_tls_config, IPADDR_TYPE_ANY);
    if (!ctx.pcb) {
        printf("[BASE44] Falha ao criar PCB TLS\n");
        return false;
    }

    altcp_arg(ctx.pcb, &ctx);
    altcp_poll(ctx.pcb, base44_poll_cb, ctx.timeout_s * 2);
    altcp_recv(ctx.pcb, base44_recv_cb);
    altcp_err(ctx.pcb, base44_err_cb);
    mbedtls_ssl_set_hostname(altcp_tls_context(ctx.pcb), BASE44_HOST);

    cyw43_arch_lwip_begin();
    ip_addr_t resolved;
    err_t derr = dns_gethostbyname(BASE44_HOST, &resolved, base44_dns_cb, &ctx);
    printf("[BASE44] dns_gethostbyname retornou %d (0=OK sincrono, %d=em progresso)\n", derr, ERR_INPROGRESS);
    if (derr == ERR_OK) {
        base44_connect_ip(&resolved, &ctx);
    } else if (derr != ERR_INPROGRESS) {
        cyw43_arch_lwip_end();
        printf("[BASE44] Erro ao resolver DNS (%d)\n", derr);
        base44_close(&ctx);
        return false;
    }
    cyw43_arch_lwip_end();

    uint32_t start_ms = to_ms_since_boot(get_absolute_time());
    while (!ctx.complete) {
        if (to_ms_since_boot(get_absolute_time()) - start_ms > (uint32_t)(ctx.timeout_s * 1000)) {
            printf("[BASE44] Timeout no envio\n");
            base44_close(&ctx);
            break;
        }
        sleep_ms(20);
    }

    bool http_ok = ctx.ok && (strstr(ctx.resp, "HTTP/1.1 2") || strstr(ctx.resp, "HTTP/1.0 2"));
    if (!http_ok) printf("[BASE44] Envio falhou. Resposta completa:\n%s\n", ctx.resp);
    else           printf("[BASE44] %d leitura(s) enviada(s) com sucesso\n", count);

    return http_ok;
}