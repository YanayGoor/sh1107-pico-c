#ifndef SH1107_SPI_PICO_C_SH1107_H
#define SH1107_SPI_PICO_C_SH1107_H

#include "hardware/gpio.h"
#include "hardware/spi.h"

#include "sh1107.h"

struct sh1107_spi {
  spi_inst_t *spi;
  int cs; // chip-select gpio pin
  int a0; // data/command gpio pin
};

extern struct sh1107_hw sh1107_hw_spi;

void sh1107_spi_init(struct sh1107_spi *sh1107_spi, spi_inst_t *spi_inst,
                     int sclk, int mosi, int a0, int cs);

#endif // SH1107_SPI_PICO_C_SH1107_H