/**
 * config.h — Analisador de Qualidade de Energia
 * Raspberry Pi Pico 2 W — Pico SDK 2.x
 *
 * Ponto único de configuração de pinos e parâmetros de calibração.
 * Todos os valores marcados "[TESTADO]" vêm literalmente dos seus
 * arquivos multiplexador.c / ZMPT101B_teste.c / ZMPT101B.h (já
 * validados em protoboard) e NÃO foram alterados.
 *
 * Os valores marcados "[ESQUEMÁTICO]" foram lidos das imagens do
 * KiCad que você enviou.
 *
 * Os valores marcados "[VERIFICAR]" são minha melhor interpretação
 * do esquemático, mas a serigrafia/trilha física não estava 100%
 * legível na imagem — confirme com multímetro/continuidade antes
 * de energizar o cartão SD. Ver nota detalhada abaixo.
 */

#ifndef CONFIG_H
#define CONFIG_H

/* ═══════════════════════════════════════════════════════════════════════
 * ADC — leitura analógica compartilhada (corrente + tensão via MUX)
 * [ESQUEMÁTICO] Único pino ADC ligado no seu projeto: GPIO26 (ADC0),
 * rotulado "Saida" no desenho — é o COM do CD74HC4067 chegando na Pico.
 * ═══════════════════════════════════════════════════════════════════════ */
#define ADC_PIN          26
#define ADC_CHANNEL       0
#define ADC_VREF          3.3f
#define ADC_RESOLUTION   4095.0f
#define ADC_TO_VOLTS     (ADC_VREF / ADC_RESOLUTION)

/* ═══════════════════════════════════════════════════════════════════════
 * MUX CD74HC4067 — seleção de canal
 * [ESQUEMÁTICO] S0=GP21, S1=GP20, S2=GP19, S3=GP18 (lido nos rótulos
 * ao lado dos pinos físicos 24-27 da Pico no seu desenho).
 * Repare que esses NÃO são os mesmos pinos usados no seu multiplexador.c
 * de bancada (que usava GP2-GP5) — ajustados aqui para bater com o
 * esquemático final da placa.
 * ═══════════════════════════════════════════════════════════════════════ */
#define MUX_S0_PIN        21
#define MUX_S1_PIN        20
#define MUX_S2_PIN        19
#define MUX_S3_PIN        18
#define MUX_SETTLE_MS       5   /* [TESTADO] tempo de estabilização do MUX */

/* ── Canais do MUX usados neste projeto ──────────────────────────────
 * C0-C2: corrente das fases L1/L2/L3 (SCT013 + burden + bias)
 * C3   : corrente do neutro N        (SCT013 + burden + bias)
 * C4   : tensão de rede (ZMPT101B)
 * Ajuste os números de canal (0-15) conforme os terminais E1-E16
 * realmente usados na sua placa MUX (J1/E1..E16 no esquemático).
 * ═══════════════════════════════════════════════════════════════════════ */
#define MUX_CH_L1         9
#define MUX_CH_L2         8
#define MUX_CH_L3         11
//#define MUX_CH_N          10
#define MUX_CH_VOLTAGE    14

/* ═══════════════════════════════════════════════════════════════════════
 * SCT013 — parâmetros de corrente [TESTADO] (multiplexador.c v4)
 * ═══════════════════════════════════════════════════════════════════════ */
#define CT_TURNS_RATIO    2000.0f
#define CT_BURDEN_OHMS      32.34f
#define CT_CURRENT_SCALE  (CT_TURNS_RATIO / CT_BURDEN_OHMS)  /* ~61.8 A/V */

#define CT_NOISE_SIGMA_FACTOR  2.0f
#define CT_NOISE_FLOOR_V       0.005f   /* 50 mV RMS -> piso mínimo detectável */

/* ═══════════════════════════════════════════════════════════════════════
 * ZMPT101B — parâmetros de tensão [TESTADO] (ZMPT101B.h)
 * ═══════════════════════════════════════════════════════════════════════ */
#define ZMPT_CALIBRATION_FACTOR   912.64f  /* V_rede / V_adc_rms */
#define ZMPT_NOISE_THRESHOLD      0.02f     /* V ADC-RMS (~117 V na rede) - aumentado para evitar ruído */

/* ═══════════════════════════════════════════════════════════════════════
 * Amostragem AC/DC [TESTADO] — 2000 amostras/s, ciclos inteiros de 60 Hz
 * ═══════════════════════════════════════════════════════════════════════ */
#define GRID_HZ             60
#define GRID_VOLTAGE_V     220.0f     /* usado só como fallback/print;
                                          a tensão real agora vem do ZMPT */
#define SAMPLE_DELAY_US    500
#define N_SAMPLES_AC       200    /* ~7 ciclos x ~30 amostras/ciclo (reduzido de 20 para acelerar) */
#define N_SAMPLES_DC       333    /* 10 ciclos x ~33 amostras/ciclo        */

#define READ_INTERVAL_MS   500

/* Tempo de espera após a calibração terminar, antes do sistema começar
 * a operar normalmente (exibir/gravar). Dá tempo do MUX e dos sensores
 * assentarem depois da troca de canal durante a calibração. */
#define CALIBRATION_SETTLE_MS  3000

/* ═══════════════════════════════════════════════════════════════════════
 * Display OLED SSD1306 (DM-OLED096-636) — [ESQUEMÁTICO]
 * SDA=GP2, SCL=GP3, I2C1, endereço padrão 0x3C
 * ═══════════════════════════════════════════════════════════════════════ */
#define OLED_I2C_PORT     i2c1
#define OLED_SDA_PIN       2
#define OLED_SCL_PIN       3
#define OLED_I2C_ADDR      0x3C
#define OLED_I2C_BAUD      (400 * 1000)

/* ═══════════════════════════════════════════════════════════════════════
 * Cartão microSD (FatFs_SPI) — [VERIFICAR]
 *
 * No esquemático, o conector J4 (6 vias) sai da região dos pinos
 * GPIO10-GPIO13 da Pico até o módulo SD (CS + 3 sinais + 3V3 + GND).
 * GPIO10-13 são os pinos nativos do periférico SPI1 do RP2040
 * (SPI1 SCK=GP10, TX/MOSI=GP11, RX/MISO=GP12, CSn=GP13), então a
 * distribuição abaixo é a que faz sentido eletricamente — mas a
 * imagem não deixa 100% claro qual fio físico é qual sinal.
 *
 * ANTES DE LIGAR O CARTÃO SD: confirme com continuímetro no seu KiCad
 * (ou no PCB) que:
 *   GPIO10 -> SCK do módulo SD
 *   GPIO11 -> MOSI (DI) do módulo SD
 *   GPIO12 -> MISO (DO) do módulo SD
 *   GPIO9 -> CS do módulo SD
 * Se estiver diferente, basta corrigir os 4 defines abaixo — o
 * restante do código (sd_logger.c) não precisa mudar.
 * ═══════════════════════════════════════════════════════════════════════ */
#define SD_SPI_PORT       spi1
#define SD_SPI_SCK_PIN    10
#define SD_SPI_MOSI_PIN   11
#define SD_SPI_MISO_PIN   12
#define SD_SPI_CS_PIN     9
#define SD_SPI_BAUD_HZ    (10 * 1000 * 1000)  /* pode reduzir se der erro de leitura */

/* ═══════════════════════════════════════════════════════════════════════
 * Botões — controle de interface
 * [ESQUEMÁTICO] Button 1 (navegação) = GP0, Button 2 (Mount/Unmount) = GP1
 * ═══════════════════════════════════════════════════════════════════════ */
#define BUTTON_NEXT_PIN      0    /* Botão para próxima página */
#define BUTTON_MOUNT_PIN     1    /* Botão para montar/desmontar SD */
#define BUTTON_DEBOUNCE_MS  20    /* Debounce agressivo para melhor resposta */
#define BUTTON_HOLD_MS    1000    /* Tempo para considerar hold */

#define SD_LOG_FILENAME   "qualidade_energia.csv"
#define SD_LOG_INTERVAL_MS 1000   /* grava uma linha por segundo */

/* ═══════════════════════════════════════════════════════════════════════
 * WiFi — sincronização NTP (Network Time Protocol)
 * MUDE PARA SUA REDE WIFI!
 * ═══════════════════════════════════════════════════════════════════════ */
#define WIFI_SSID       "computador"
#define WIFI_PASSWORD   "12345678"
/* ═══════════════════════════════════════════════════════════════════════
 * Base44 — API HTTPS para envio das leituras (PhaseReading)
 * ═══════════════════════════════════════════════════════════════════════ */
#define BASE44_HOST            "grinning-volt-pulse-grid.base44.app"
#define BASE44_API_KEY         "f2120861640f4076bf5cb51d87b7455d"
#define BASE44_SEND_INTERVAL_MS  4000   /* envia a cada 10s, não a cada 500ms */
#endif /* CONFIG_H */