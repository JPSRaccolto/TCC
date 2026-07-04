/**
 * oled_display.c — display com suporte a múltiplas páginas
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "config.h"
#include "oled_display.h"

static ssd1306_t ssd;
static display_page_t current_page = PAGE_MEASUREMENTS_1;

/* Últimas medições armazenadas para exibição */
static float last_i_l1 = 0;
static float last_i_l2 = 0;
static float last_i_l3 = 0;
static float last_i_n = 0;
static float last_v_rede = 0;
static sd_status_t last_sd_status = SD_STATUS_NOT_INIT;

void oled_display_init(void) {
    i2c_init(OLED_I2C_PORT, OLED_I2C_BAUD);
    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, OLED_I2C_ADDR, OLED_I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Inicializando...", 0, 0);
    ssd1306_send_data(&ssd);
}

void oled_display_next_page(void) {
    current_page = (display_page_t)((current_page + 1) % NUM_PAGES);
}

void oled_display_set_page(display_page_t page) {
    if (page < NUM_PAGES) {
        current_page = page;
    }
}

display_page_t oled_display_get_current_page(void) {
    return current_page;
}

static void display_page_measurements_1(void) {
    char linha[24];

    ssd1306_fill(&ssd, false);
    
    /* Título */
    ssd1306_draw_string(&ssd, "Pagina 1/3", 0, 0);
    
    /* Tensão RMS */
    snprintf(linha, sizeof(linha), "V: %6.1f V", last_v_rede);
    ssd1306_draw_string(&ssd, linha, 0, 16);

    /* Fase L1 */
    snprintf(linha, sizeof(linha), "L1: %6.2f A", last_i_l1);
    ssd1306_draw_string(&ssd, linha, 0, 32);

    /* Fase L2 */
    snprintf(linha, sizeof(linha), "L2: %6.2f A", last_i_l2);
    ssd1306_draw_string(&ssd, linha, 0, 48);

    ssd1306_send_data(&ssd);
}

static void display_page_measurements_2(void) {
    char linha[24];

    ssd1306_fill(&ssd, false);
    
    /* Título */
    ssd1306_draw_string(&ssd, "Pagina 2/3", 0, 0);
    
    /* Fase L3 */
    snprintf(linha, sizeof(linha), "L3: %6.2f A", last_i_l3);
    ssd1306_draw_string(&ssd, linha, 0, 16);

    /* Neutro */
    snprintf(linha, sizeof(linha), "N : %6.2f A", last_i_n);
    ssd1306_draw_string(&ssd, linha, 0, 32);

    /* Total de corrente */
    float i_total = last_i_l1 + last_i_l2 + last_i_l3;
    snprintf(linha, sizeof(linha), "Total: %6.2f A", i_total);
    ssd1306_draw_string(&ssd, linha, 0, 48);

    ssd1306_send_data(&ssd);
}

static void display_page_system_info(void) {
    char linha[24];

    ssd1306_fill(&ssd, false);
    
    /* Título */
    ssd1306_draw_string(&ssd, "Info Sistema", 0, 0);
    
    /* Status do SD */
    ssd1306_draw_string(&ssd, "MicroSD:", 0, 16);
    
    const char *sd_text = "";
    switch (last_sd_status) {
        case SD_STATUS_NOT_INIT:
            sd_text = "Nao init.";
            break;
        case SD_STATUS_MOUNTING:
            sd_text = "Montando...";
            break;
        case SD_STATUS_MOUNTED:
            sd_text = "Pronto";
            break;
        case SD_STATUS_LOGGING:
            sd_text = "Gravando";
            break;
        case SD_STATUS_FAILED:
            sd_text = "FALHA";
            break;
        case SD_STATUS_UNMOUNTING:
            sd_text = "Desmontando";
            break;
    }
    snprintf(linha, sizeof(linha), "  %s", sd_text);
    ssd1306_draw_string(&ssd, linha, 0, 32);

    ssd1306_draw_string(&ssd, "Use Bot2 p/", 0, 48);

    ssd1306_send_data(&ssd);
}

void oled_display_show(float i_l1, float i_l2, float i_l3, float i_n,
                        float v_rede, sd_status_t sd_status) {
    /* Armazena últimas medições */
    last_i_l1 = i_l1;
    last_i_l2 = i_l2;
    last_i_l3 = i_l3;
    last_i_n = i_n;
    last_v_rede = v_rede;
    last_sd_status = sd_status;

    /* Exibe página atual */
    switch (current_page) {
        case PAGE_MEASUREMENTS_1:
            display_page_measurements_1();
            break;
        case PAGE_MEASUREMENTS_2:
            display_page_measurements_2();
            break;
        case PAGE_SYSTEM_INFO:
            display_page_system_info();
            break;
        default:
            break;
    }
}
