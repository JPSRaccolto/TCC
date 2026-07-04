/**
 * hw_config.c — configuração de hardware exigida pela biblioteca FatFs_SPI
 * (mesmo padrão do seu repositório MPU_SD, com os pinos ajustados para
 * os definidos em config.h — ver a nota [VERIFICAR] lá antes de gravar).
 */
#include <assert.h>
#include <string.h>

#include "my_debug.h"
#include "hw_config.h"
#include "ff.h"
#include "diskio.h"
#include "config.h"

static spi_t spis[] = {
    {
        .hw_inst   = SD_SPI_PORT,
        .miso_gpio = SD_SPI_MISO_PIN,
        .mosi_gpio = SD_SPI_MOSI_PIN,
        .sck_gpio  = SD_SPI_SCK_PIN,
        .baud_rate = SD_SPI_BAUD_HZ,
    }
};

static sd_card_t sd_cards[] = {
    {
        .pcName            = "0:",
        .spi               = &spis[0],
        .ss_gpio           = SD_SPI_CS_PIN,
        .use_card_detect   = false,
        .card_detect_gpio  = 0,
        .card_detected_true = -1,
    }
};

size_t sd_get_num() { return count_of(sd_cards); }
sd_card_t *sd_get_by_num(size_t num) {
    assert(num <= sd_get_num());
    if (num <= sd_get_num()) return &sd_cards[num];
    return NULL;
}
size_t spi_get_num() { return count_of(spis); }
spi_t *spi_get_by_num(size_t num) {
    assert(num <= spi_get_num());
    if (num <= spi_get_num()) return &spis[num];
    return NULL;
}
