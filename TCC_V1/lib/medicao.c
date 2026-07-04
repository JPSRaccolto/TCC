/**
 * medicao.c
 *
 * Portado diretamente do seu multiplexador.c (v4) e ZMPT101B_teste.c,
 * generalizado para N canais do CD74HC4067. O método de medição é
 * idêntico ao testado:
 *   1) mux_select_channel()  -> mesma função, mesmo delay de assentamento
 *   2) measure_dc()          -> média de N_SAMPLES_DC amostras cobrindo
 *                                ciclos inteiros de 60 Hz (a senoide some
 *                                na média, sobra só o DC/offset)
 *   3) RMS da janela AC      -> soma de quadrados sobre N_SAMPLES_AC
 *                                amostras, com subtração de ruído em
 *                                quadratura (mesma equação do original)
 *
 * Diferença: corrente usa CT_CURRENT_SCALE (A/V); tensão usa
 * ZMPT_CALIBRATION_FACTOR (V/V) — mesmos números dos seus arquivos.
 */

#include <math.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "config.h"
#include "medicao.h"

void medicao_init(void) {
    gpio_init(MUX_S0_PIN); gpio_set_dir(MUX_S0_PIN, GPIO_OUT);
    gpio_init(MUX_S1_PIN); gpio_set_dir(MUX_S1_PIN, GPIO_OUT);
    gpio_init(MUX_S2_PIN); gpio_set_dir(MUX_S2_PIN, GPIO_OUT);
    gpio_init(MUX_S3_PIN); gpio_set_dir(MUX_S3_PIN, GPIO_OUT);

    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_CHANNEL);

    medicao_selecionar_canal(0);
}

void medicao_selecionar_canal(uint8_t ch) {
    gpio_put(MUX_S0_PIN, (ch >> 0) & 1);
    gpio_put(MUX_S1_PIN, (ch >> 1) & 1);
    gpio_put(MUX_S2_PIN, (ch >> 2) & 1);
    gpio_put(MUX_S3_PIN, (ch >> 3) & 1);
    sleep_ms(MUX_SETTLE_MS);
}

/* Média de N_SAMPLES_DC amostras cobrindo ciclos inteiros -> só sobra o DC */
static float medir_dc(void) {
    uint64_t sum = 0;
    for (int i = 0; i < N_SAMPLES_DC; i++) {
        sum += adc_read();
        sleep_us(SAMPLE_DELAY_US);
    }
    return ((float)(sum / N_SAMPLES_DC)) * ADC_TO_VOLTS;
}

void medicao_calibrar_canal(canal_t *canal) {
    medicao_selecionar_canal(canal->mux_ch);

    /* Offset DC — média de 3 medições, igual ao original */
    float dc_acc = 0.0f;
    for (int r = 0; r < 3; r++) {
        dc_acc += medir_dc();
    }
    canal->dc_offset_v = dc_acc / 3.0f;

    if (canal->dc_offset_v < 1.4f || canal->dc_offset_v > 1.9f) {
        printf("  [AVISO] %s: offset DC fora do esperado (1,4-1,9 V): %.4f V\n",
               canal->nome, canal->dc_offset_v);
    }

    /* Ruído de fundo (SCT013) / sinal medido durante a calibração (ZMPT) */
    double sum_sq = 0.0;
    for (int i = 0; i < N_SAMPLES_AC; i++) {
        float v = (float)adc_read() * ADC_TO_VOLTS - canal->dc_offset_v;
        sum_sq += (double)(v * v);
        sleep_us(SAMPLE_DELAY_US);
    }
    float rms_calib = sqrtf((float)(sum_sq / N_SAMPLES_AC));

    if (canal->tipo == CANAL_CORRENTE) {
        /* SCT013 sem carga nas fases -> isso é ruído eletrônico puro,
         * pode ser subtraído em quadratura das leituras futuras. */
        canal->noise_rms_v = rms_calib;
        float from_noise = CT_NOISE_SIGMA_FACTOR * canal->noise_rms_v;
        canal->noise_floor_v = (from_noise > CT_NOISE_FLOOR_V) ? from_noise : CT_NOISE_FLOOR_V;

        printf("  [%s] C%d  offset=%.4fV  ruido=%.4fV  piso=%.4fV\n",
               canal->nome, canal->mux_ch, canal->dc_offset_v,
               canal->noise_rms_v, canal->noise_floor_v);
    } else {
        /* ZMPT101B fica ligado na rede durante a calibração, então
         * rms_calib aqui é o PRÓPRIO sinal real de tensão, não ruído.
         * Se usássemos isso em medicao_ler_canal() (subtração em
         * quadratura), a leitura em operação normal ficaria perto de
         * zero, porque o código estaria subtraindo o sinal dele mesmo.
         * Por isso noise_rms_v fica zerado para este canal, e usamos
         * só o piso fixo ZMPT_NOISE_THRESHOLD para decidir "sem rede". */
        canal->noise_rms_v = 0.0f;
        canal->noise_floor_v = ZMPT_NOISE_THRESHOLD;

        printf("  [%s] C%d  offset=%.4fV  sinal_calib=%.4fV (rede ligada, nao usado p/ subtracao)  piso=%.4fV\n",
               canal->nome, canal->mux_ch, canal->dc_offset_v,
               rms_calib, canal->noise_floor_v);
    }
}

leitura_t medicao_ler_canal(canal_t *canal) {
    leitura_t r = {0};

    medicao_selecionar_canal(canal->mux_ch);

    float dc = medir_dc();
    r.dc_v = dc;

    double sum_sq = 0.0;
    for (int i = 0; i < N_SAMPLES_AC; i++) {
        float v = (float)adc_read() * ADC_TO_VOLTS - dc;
        sum_sq += (double)(v * v);
        sleep_us(SAMPLE_DELAY_US);
    }

    float v_rms = sqrtf((float)(sum_sq / N_SAMPLES_AC));
    r.vrms_ac = v_rms;

    if (v_rms < canal->noise_floor_v) {
        r.valor = 0.0f;
        return r;
    }

    /* Subtração de ruído em quadratura, igual ao original */
    float noise  = canal->noise_rms_v;
    float radicando = v_rms * v_rms - noise * noise;
    float v_corr = (radicando > 0.0f) ? sqrtf(radicando) : 0.0f;

    if (canal->tipo == CANAL_CORRENTE) {
        r.valor = v_corr * CT_CURRENT_SCALE;
    } else {
        r.valor = v_corr * ZMPT_CALIBRATION_FACTOR;
    }

    return r;
}