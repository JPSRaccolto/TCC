#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H

// Need more memory for TLS
#ifdef MQTT_CERT_INC
#define MEM_SIZE 8000
#endif

// Generally you would define your own explicit list of lwIP options
// (see https://www.nongnu.org/lwip/2_1_x/group__lwip__opts.html)
//
// This example uses a common include to avoid repetition
#include "lwipopts_examples_common.h"

#define MEMP_NUM_SYS_TIMEOUT        (LWIP_NUM_SYS_TIMEOUT_INTERNAL+1)

#ifdef MQTT_CERT_INC
#define LWIP_ALTCP               1
#define LWIP_ALTCP_TLS           1
#define LWIP_ALTCP_TLS_MBEDTLS   1
#ifndef NDEBUG
#define ALTCP_MBEDTLS_DEBUG  LWIP_DBG_ON
#endif
/* TCP WND must be at least 16 kb to match TLS record size
   or you will get a warning "altcp_tls: TCP_WND is smaller than the RX decrypion buffer, connection RX might stall!" */
#undef TCP_WND
#define TCP_WND  16384
#endif // MQTT_CERT_INC

// This defaults to 4
#define MQTT_REQ_MAX_IN_FLIGHT 7

/* --- Sincronização de hora via SNTP (usada por ntp_sync.c) ---
 * Sem SNTP_SET_SYSTEM_TIME, o lwIP recebe a resposta do servidor NTP
 * e simplesmente descarta o valor, sem chamar nada. Aqui apontamos
 * essa macro para uma função nossa (ntp_sync_set_system_time, em
 * lib/ntp_sync.c) que grava o horário recebido no AON timer.
 *
 * SNTP_SERVER_DNS habilita resolver o nome "pool.ntp.org" via DNS ao
 * invés de exigir um IP numérico fixo.
 */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
void ntp_sync_set_system_time(uint32_t sec);
#ifdef __cplusplus
}
#endif

#define SNTP_SET_SYSTEM_TIME(sec) ntp_sync_set_system_time(sec)
#define SNTP_SERVER_DNS 1

#endif