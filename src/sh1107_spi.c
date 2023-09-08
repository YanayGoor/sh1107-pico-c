#include "../include/sh1107_spi.h"

#include <pico/time.h>
#include <stdio.h>
#include <stdlib.h>

void sh1107_spi_init(struct sh1107_spi *sh1107_spi, spi_inst_t *spi_inst, int sclk, int mosi, int a0, int cs) {
	sh1107_spi->spi = spi_inst;
	sh1107_spi->cs = cs;
	sh1107_spi->a0 = a0;

	spi_init(sh1107_spi->spi, 4000000);
	gpio_set_function(sclk, GPIO_FUNC_SPI);
	gpio_set_function(mosi, GPIO_FUNC_SPI);

	gpio_init(sh1107_spi->a0);
	gpio_set_dir(sh1107_spi->a0, true);
	gpio_put(sh1107_spi->a0, 0);

	gpio_init(sh1107_spi->cs);
	gpio_set_dir(sh1107_spi->cs, true);
	gpio_put(sh1107_spi->cs, 1);
}

static void sh1107_spi_write_command(struct sh1107_spi *sh1107_spi, uint8_t *buff, size_t buff_size) {
	gpio_put(sh1107_spi->cs, 1);
	gpio_put(sh1107_spi->a0, 0);
	gpio_put(sh1107_spi->cs, 0);
	spi_write_blocking(sh1107_spi->spi, buff, buff_size);
	gpio_put(sh1107_spi->cs, 1);
}

static void sh1107_spi_write_data(struct sh1107_spi *sh1107_spi, uint8_t *buff, size_t buff_size) {
	gpio_put(sh1107_spi->cs, 1);
	gpio_put(sh1107_spi->a0, 1);
	gpio_put(sh1107_spi->cs, 0);
	spi_write_blocking(sh1107_spi->spi, buff, buff_size);
	gpio_put(sh1107_spi->cs, 1);
}

static void sh1107_spi_write_single_command(struct sh1107_spi *sh1107_spi, uint8_t command) {
	sh1107_spi_write_command(sh1107_spi, &command, 1);
}

static void sh1107_spi_write_double_command(struct sh1107_spi *sh1107_spi, uint8_t command, uint8_t value) {
	uint8_t buff[2] = {command, value};
	sh1107_spi_write_command(sh1107_spi, (uint8_t *)buff, sizeof(buff));
}

struct sh1107_hw sh1107_hw_spi = {.write_single_command = sh1107_spi_write_single_command,
								  .write_double_command = sh1107_spi_write_double_command,
								  .write_data = sh1107_spi_write_data};