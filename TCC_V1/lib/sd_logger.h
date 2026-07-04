/**
 * sd_logger.h — Backup em cartão microSD via biblioteca FatFs_SPI
 * (a mesma do seu repositório MPU_SD, aqui usada só para gravar CSV).
 */
#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <stdbool.h>
#include <stdint.h>

/* Monta o cartão e cria/abre o arquivo CSV, escrevendo o cabeçalho
 * se o arquivo ainda não existir. Retorna true se pronto para logar. */
bool sd_logger_init(void);

/* Grava uma linha de amostra no CSV.
 * timestamp_ms: ms desde o boot (troque por RTC se adicionar um).
 * i_l1..i_n: corrente RMS de cada fase/neutro (A)
 * v_rede: tensão RMS da rede (V)
 * As potências (VA) são calculadas dentro da função. */
void sd_logger_write(uint32_t timestamp_ms,
                      float i_l1, float i_l2, float i_l3, float i_n,
                      float v_rede);

/* Fecha o arquivo e desmonta o cartão com segurança. */
void sd_logger_close(void);

/* Retorna true se o cartão está montado e pronto. */
bool sd_logger_is_mounted(void);

/* Retorna true se o arquivo está aberto para gravação. */
bool sd_logger_is_logging(void);

/* Desmonta o cartão SD de forma segura. Retorna true se bem-sucedido. */
bool sd_logger_unmount(void);

/* Monta o cartão SD novamente. Retorna true se bem-sucedido. */
bool sd_logger_mount(void);

#endif /* SD_LOGGER_H */
