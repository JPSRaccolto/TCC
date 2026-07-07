/**
 * ntp_sync.h — Sincronização de hora via NTP (Network Time Protocol)
 * Raspberry Pi Pico 2 W com WiFi
 *
 * Sincroniza o RTC interno com a hora da internet via SNTP
 * Se falhar, solicita hora manualmente ao usuário via serial
 */

#ifndef NTP_SYNC_H
#define NTP_SYNC_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Inicializa WiFi, sincroniza hora via NTP, e configura o RTC
 * 
 * Se WiFi não estiver disponível ou NTP falhar:
 *   - Solicita ao usuário digitar a hora via serial
 *   - Formato esperado: "YYYY-MM-DD HH:MM:SS"
 *
 * Retorna true se sincronização bem-sucedida, false caso contrário
 */
bool ntp_sync_time(const char *wifi_ssid, const char *wifi_password);
bool ntp_sync_time_wifi_connected(void);
#endif /* NTP_SYNC_H */
