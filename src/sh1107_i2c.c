#include <pico/time.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/sh1107.h"
#include "font.h"

void sh1107_i2c_write_command(struct sh1107_i2c *sh1107_i2c, const uint8_t *buff, uint buff_size) {
	uint8_t prefix = 0x0;
	i2c_write_blocking(sh1107_i2c->i2c_inst, sh1107_i2c->address, &prefix, sizeof(prefix), true);
	i2c_write_blocking(sh1107_i2c->i2c_inst, sh1107_i2c->address, buff, buff_size, false);
}

void sh1107_i2c_write_data(struct sh1107_i2c *sh1107_i2c, const uint8_t *buff, uint buff_size) {
	uint8_t prefix = 0x40;
	i2c_write_blocking(sh1107_i2c->i2c_inst, sh1107_i2c->address, &prefix, sizeof(prefix), true);
	i2c_write_blocking(sh1107_i2c->i2c_inst, sh1107_i2c->address, buff, buff_size, false);
}

void sh1107_i2c_init(struct sh1107_i2c *sh1107_i2c, i2c_inst_t *i2c_inst, int sda, int scl, int address) {
	sh1107_i2c->i2c_inst = i2c_inst;
	sh1107_i2c->address = address;

	i2c_init(sh1107_i2c->i2c_inst, 400000);
	gpio_set_function(sda, GPIO_FUNC_SPI);
	gpio_set_function(scl, GPIO_FUNC_SPI);
}

static void sh1107_i2c_write_single_command(struct sh1107_i2c *sh1107_i2c, uint8_t command) {
	sh1107_i2c_write_command(sh1107_i2c, &command, 1);
}

static void sh1107_i2c_write_double_command(struct sh1107_i2c *sh1107_i2c, uint8_t command, uint8_t value) {
	uint8_t buff[2] = {command, value};
	sh1107_i2c_write_command(sh1107_i2c, (uint8_t *)buff, sizeof(buff));
}

struct sh1107_hw sh1107_hw_i2c = {.write_single_command = sh1107_i2c_write_single_command,
								  .write_double_command = sh1107_i2c_write_double_command,
								  .write_data = sh1107_i2c_write_data};