/**
 * medicao.h — Leitura de corrente (SCT013 x4 via MUX) e tensão (ZMPT101B)
 * compartilhando o mesmo ADC0 através do CD74HC4067.
 *
 * A lógica de amostragem (offset DC, RMS, piso de ruído) é a mesma dos
 * seus arquivos multiplexador.c e ZMPT101B_teste.c — só foi organizada
 * para funcionar com N canais multiplexados em vez de 1 fixo.
 */
#ifndef MEDICAO_H
#define MEDICAO_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    CANAL_CORRENTE,
    CANAL_TENSAO
} tipo_canal_t;

typedef struct {
    const char   *nome;        /* "L1", "L2", "L3", "N", "Tensao" */
    uint8_t       mux_ch;      /* canal 0-15 do CD74HC4067 */
    tipo_canal_t  tipo;

    /* calibração (preenchida por medicao_calibrar_canal) */
    float dc_offset_v;
    float noise_rms_v;
    float noise_floor_v;
} canal_t;

typedef struct {
    float valor;       /* Ampere (corrente) ou Volt (tensão de rede) */
    float vrms_ac;      /* Vrms bruto no ADC, útil para debug */
    float dc_v;          /* offset DC medido nesta leitura */
} leitura_t;

/* Inicializa GPIOs do MUX e o ADC. Chame uma vez no boot. */
void medicao_init(void);

/* Seleciona um canal do MUX e aguarda estabilizar. */
void medicao_selecionar_canal(uint8_t mux_ch);

/* Calibra offset DC e ruído de fundo de um canal.
 * IMPORTANTE: chamar com o sensor correspondente SEM corrente/tensão
 * de rede aplicada (mesmo procedimento já usado nos testes). */
void medicao_calibrar_canal(canal_t *canal);

/* Faz uma leitura RMS completa do canal (corrente em A ou tensão em V,
 * conforme canal->tipo), reatualizando o offset DC a cada chamada. */
leitura_t medicao_ler_canal(canal_t *canal);

#endif /* MEDICAO_H */
