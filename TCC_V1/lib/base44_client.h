#ifndef BASE44_CLIENT_H
#define BASE44_CLIENT_H

#include <stdbool.h>

typedef struct {
    const char *phase;     /* "F1", "F2", "F3" ou "N" */
    float voltage_v;
    float current_a;
} base44_reading_t;

/* Liga o WiFi e mantém a conexão viva. Chamar uma vez, depois do NTP. */
bool base44_client_connect_wifi(const char *ssid, const char *password);

/* Envia um lote de leituras via bulkCreate. Bloqueia até completar
 * (handshake TLS costuma levar de 1 a 3s no Pico). Retorna true em
 * sucesso HTTP 2xx. */
bool base44_client_send_bulk(const char *timestamp_iso, const base44_reading_t *readings, int count);
void base44_format_timestamp(char *buf, size_t len);
#endif