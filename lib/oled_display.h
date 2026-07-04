/**
 * oled_display.h — display de status em tempo real (SSD1306, DM-OLED096-636)
 * Com suporte a múltiplas páginas e navegação por botão.
 */
#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <stdbool.h>

typedef enum {
    SD_STATUS_NOT_INIT,    /* Não inicializado */
    SD_STATUS_MOUNTING,    /* Tentando montar */
    SD_STATUS_MOUNTED,     /* Montado pronto para gravar */
    SD_STATUS_LOGGING,     /* Gravando dados */
    SD_STATUS_FAILED,      /* Falha na montagem */
    SD_STATUS_UNMOUNTING   /* Desmontando */
} sd_status_t;

typedef enum {
    PAGE_MEASUREMENTS_1,   /* Página 1: L1, L2, Tensão */
    PAGE_MEASUREMENTS_2,   /* Página 2: L3, N */
    PAGE_SYSTEM_INFO,      /* Página 3: Info do sistema (SD) */
    NUM_PAGES
} display_page_t;

void oled_display_init(void);

void oled_display_show(float i_l1, float i_l2, float i_l3, float i_n,
                        float v_rede, sd_status_t sd_status);

void oled_display_next_page(void);

void oled_display_set_page(display_page_t page);

display_page_t oled_display_get_current_page(void);

#endif /* OLED_DISPLAY_H */
