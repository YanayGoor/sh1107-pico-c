#ifndef SH1107_I2C_PICO_C_SH1107_H
#define SH1107_I2C_PICO_C_SH1107_H

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "sh1107.h"

#define SH1107_I2C_DEFAULT_ADDR (0x3d)

struct sh1107_i2c {
	i2c_inst_t *i2c_inst;
	uint8_t address;
};

extern struct sh1107_hw sh1107_hw_i2c;

void sh1107_i2c_init(struct sh1107_i2c *sh1107_i2c, i2c_inst_t *i2c_inst, int sda, int scl, int address);

#endif // SH1107_I2C_PICO_C_SH1107_H