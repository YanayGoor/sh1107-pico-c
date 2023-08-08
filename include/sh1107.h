#ifndef SH1107_PICO_C_SH1107_H
#define SH1107_PICO_C_SH1107_H

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"

#include "font.h"

#define SH1107_MAX_HEIGHT (128)
#define SH1107_MAX_WIDTH (128)

#define SH1107_PAGE_SIZE (8)

#define SH1107_I2C_DEFAULT_ADDR (0x3d)

struct sh1107_i2c {
  i2c_inst_t *i2c_inst;
  uint8_t address;
};

struct sh1107_spi {
  spi_inst_t *spi;
  int cs; // chip-select gpio pin
  int a0; // data/command gpio pin
};

struct line_change {
  bool changed;
  uint start;
  uint end;
};

struct sh1107 {
  uint width;
  uint height;
  uint rotate;
  uint rotate90;
  int res; // reset gpio pin
  struct sh1107_spi *spi;
  struct sh1107_i2c *i2c;
  uint8_t buff[SH1107_MAX_HEIGHT / SH1107_PAGE_SIZE][SH1107_MAX_WIDTH];
  struct line_change changes[SH1107_MAX_HEIGHT / SH1107_PAGE_SIZE];
};

enum text_alignment {
  text_align_left = 0,
  text_align_center,
  text_align_right
};

void sh1107_init(struct sh1107 *sh1107, struct sh1107_spi *spi,
                 struct sh1107_i2c *i2c, int res, uint height);
void sh1107_show(struct sh1107 *sh1107);

void sh1107_i2c_init(struct sh1107_i2c *sh1107_i2c, i2c_inst_t *i2c_inst,
                     int sda, int scl, int address);
void sh1107_spi_init(struct sh1107_spi *sh1107_spi, spi_inst_t *spi_inst,
                     int sclk, int mosi, int a0, int cs);

void sh1107_fill(struct sh1107 *sh1107, uint row, uint col, uint width,
                 uint height, bool value);
void sh1107_reset(struct sh1107 *sh1107);
void sh1107_invert(struct sh1107 *sh1107, uint value);
void sh1107_text(struct sh1107 *sh1107, const char *text, uint row, uint col,
                 uint color, uint size, font_t *font,
                 enum text_alignment alignment);

#endif // SH1107_PICO_C_SH1107_H
