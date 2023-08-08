#include "../include/sh1107.h"
#include "font.h"

#include <stdio.h>
#include <stdlib.h>

#define SH1107_LOW_COLUMN_ADDRESS (0x00)
#define SH1107_HIGH_COLUMN_ADDRESS (0x10)
#define SH1107_MEM_ADDRESSING_MODE (0x20)
#define SH1107_SET_CONTRAST (0x81)
#define SH1107_SET_SEGMENT_REMAP (0xa0)
#define SH1107_SET_MULTIPLEX_RATIO (0xA8)
#define SH1107_SET_NORMAL_INVERSE (0xa6)
#define SH1107_SET_DISPLAY_OFFSET (0xD3)
#define SH1107_SET_DC_DC_CONVERTER_SF (0xad8d)
#define SH1107_SET_DISPLAY_OFF (0xae)
#define SH1107_SET_DISPLAY_ON (0xaf)
#define SH1107_SET_PAGE_ADDRESS (0xB0)
#define SH1107_SET_SCAN_DIRECTION (0xC0)
#define SH1107_SET_DISPLAY_START_LINE (0xDC)

#define HIGH_BYTE(hword) (hword >> 8)
#define LOW_BYTE(hword) (hword & 0xff)

void sh1107_i2c_write_command(struct sh1107_i2c *sh1107_i2c,
                              const uint8_t *buff, uint buff_size) {
  uint8_t prefix = 0x0;
  i2c_write_blocking(sh1107_i2c->i2c_inst, sh1107_i2c->address, &prefix,
                     sizeof(prefix), true);
  i2c_write_blocking(sh1107_i2c->i2c_inst, sh1107_i2c->address, buff, buff_size,
                     false);
}

void sh1107_i2c_write_data(struct sh1107_i2c *sh1107_i2c, const uint8_t *buff,
                           uint buff_size) {
  uint8_t prefix = 0x40;
  i2c_write_blocking(sh1107_i2c->i2c_inst, sh1107_i2c->address, &prefix,
                     sizeof(prefix), true);
  i2c_write_blocking(sh1107_i2c->i2c_inst, sh1107_i2c->address, buff, buff_size,
                     false);
}

void sh1107_spi_init(struct sh1107_spi *sh1107_spi, spi_inst_t *spi_inst,
                     int sclk, int mosi, int a0, int cs) {
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

void sh1107_i2c_init(struct sh1107_i2c *sh1107_i2c, i2c_inst_t *i2c_inst,
                     int sda, int scl, int address) {
  sh1107_i2c->i2c_inst = i2c_inst;
  sh1107_i2c->address = address;

  i2c_init(sh1107_i2c->i2c_inst, 400000);
  gpio_set_function(sda, GPIO_FUNC_SPI);
  gpio_set_function(scl, GPIO_FUNC_SPI);
}

static void sh1107_spi_write_command(struct sh1107_spi *sh1107_spi,
                                     uint8_t *buff, size_t buff_size) {
  gpio_put(sh1107_spi->cs, 1);
  gpio_put(sh1107_spi->a0, 0);
  gpio_put(sh1107_spi->cs, 0);
  spi_write_blocking(sh1107_spi->spi, buff, buff_size);
  gpio_put(sh1107_spi->cs, 1);
}

static void sh1107_spi_write_data(struct sh1107_spi *sh1107_spi, uint8_t *buff,
                                  size_t buff_size) {
  gpio_put(sh1107_spi->cs, 1);
  gpio_put(sh1107_spi->a0, 1);
  gpio_put(sh1107_spi->cs, 0);
  spi_write_blocking(sh1107_spi->spi, buff, buff_size);
  gpio_put(sh1107_spi->cs, 1);
}

static void sh1107_spi_write_single_command(struct sh1107_spi *sh1107_spi,
                                            uint8_t command) {
  sh1107_spi_write_command(sh1107_spi, &command, 1);
}

static void sh1107_spi_write_double_command(struct sh1107_spi *sh1107_spi,
                                            uint8_t command, uint8_t value) {
  uint8_t buff[2] = {command, value};
  sh1107_spi_write_command(sh1107_spi, (uint8_t *)buff, sizeof(buff));
}

static void sh1107_i2c_write_single_command(struct sh1107_i2c *sh1107_i2c,
                                            uint8_t command) {
  sh1107_i2c_write_command(sh1107_i2c, &command, 1);
}

static void sh1107_i2c_write_double_command(struct sh1107_i2c *sh1107_i2c,
                                            uint8_t command, uint8_t value) {
  uint8_t buff[2] = {command, value};
  sh1107_i2c_write_command(sh1107_i2c, (uint8_t *)buff, sizeof(buff));
}

static void sh1107_write_single_command(struct sh1107 *sh1107,
                                        uint8_t command) {
  if (sh1107->spi) {
    sh1107_spi_write_single_command(sh1107->spi, command);
  } else if (sh1107->i2c) {
    sh1107_i2c_write_single_command(sh1107->i2c, command);
  } else {
    panic("no protocol defined\n");
  }
}

static void sh1107_write_double_command(struct sh1107 *sh1107, uint8_t command,
                                        uint8_t value) {
  if (sh1107->spi) {
    sh1107_spi_write_double_command(sh1107->spi, command, value);
  } else if (sh1107->i2c) {
    sh1107_i2c_write_double_command(sh1107->i2c, command, value);
  } else {
    panic("no protocol defined\n");
  }
}

static void sh1107_write_data(struct sh1107 *sh1107, uint8_t *buff,
                              size_t buff_size) {
  if (sh1107->spi) {
    sh1107_spi_write_data(sh1107->spi, buff, buff_size);
  } else if (sh1107->i2c) {
    sh1107_i2c_write_data(sh1107->i2c, buff, buff_size);
  } else {
    panic("no protocol defined\n");
  }
}

static void sh1107_set_pixel(struct sh1107 *sh1107, uint row, uint col,
                             bool value) {
  uint page = row / 8;
  uint offset = row % 8;

  if (value) {
    sh1107->buff[page][col] |= 1 << offset;
  } else {
    sh1107->buff[page][col] &= ~(1 << offset);
  }

  if (sh1107->changes[page].changed) {
    sh1107->changes[page].start = MIN(sh1107->changes[page].start, col);
    sh1107->changes[page].end = MAX(sh1107->changes[page].end, col + 1);
  } else {
    sh1107->changes[page].start = col;
    sh1107->changes[page].end = col + 1;
  }
  sh1107->changes[page].changed = true;
}

void sh1107_fill(struct sh1107 *sh1107, uint row, uint col, uint width,
                 uint height, bool value) {
  for (uint i = 0; i < width; i++) {
    for (uint j = 0; j < height; j++) {
      sh1107_set_pixel(sh1107, row + j, col + i, value);
    }
  }
}

void sh1107_contrast(struct sh1107 *sh1107, uint value) {
  sh1107_spi_write_double_command(sh1107->spi, SH1107_SET_CONTRAST, value);
}

void sh1107_invert(struct sh1107 *sh1107, uint value) {
  sh1107_write_single_command(sh1107, SH1107_SET_NORMAL_INVERSE | value);
}

static void sh1107_set_column(struct sh1107 *sh1107, uint col) {
  assert(col <= 0xff);
  sh1107_write_single_command(sh1107, SH1107_LOW_COLUMN_ADDRESS | (col & 0x0f));
  sh1107_write_single_command(sh1107, SH1107_HIGH_COLUMN_ADDRESS | (col >> 4));
}

static void sh1107_set_page(struct sh1107 *sh1107, uint page) {
  assert(page <= 0xf);
  sh1107_write_single_command(sh1107, SH1107_SET_PAGE_ADDRESS | page);
}

void sh1107_poweron(struct sh1107 *sh1107) {
  sh1107_write_single_command(sh1107, SH1107_SET_DISPLAY_ON);
  sleep_ms(100); // SH1107 recommended delay in power on sequence
}

void sh1107_poweroff(struct sh1107 *sh1107) {
  sh1107_write_single_command(sh1107, SH1107_SET_DISPLAY_OFF);
}

void sh1107_init(struct sh1107 *sh1107, struct sh1107_spi *sh1107_spi,
                 struct sh1107_i2c *sh1107_i2c, int res, uint height) {
  assert(sh1107);
  assert(sh1107_i2c || sh1107_spi);

  sh1107->spi = sh1107_spi;
  sh1107->i2c = sh1107_i2c;
  sh1107->res = res;
  sh1107->width = SH1107_MAX_WIDTH;
  sh1107->height = height;

  gpio_init(sh1107->res);
  gpio_set_dir(sh1107->res, true);
  gpio_put(sh1107->res, 0);

  sh1107_reset(sh1107);
  sh1107_poweroff(sh1107);

  uint multiplex_ratio = height == SH1107_MAX_HEIGHT ? 0x7f : 0x3f;
  sh1107_fill(sh1107, 0, 0, SH1107_MAX_WIDTH, height, 0);
  sh1107_write_double_command(sh1107, HIGH_BYTE(SH1107_SET_DC_DC_CONVERTER_SF),
                              LOW_BYTE(SH1107_SET_DC_DC_CONVERTER_SF));
  sh1107_write_double_command(sh1107, SH1107_SET_MULTIPLEX_RATIO,
                              multiplex_ratio);
  sh1107_write_double_command(sh1107, SH1107_MEM_ADDRESSING_MODE, 0);
  sh1107_set_page(sh1107, 0);
  sh1107_contrast(sh1107, 128);
  sh1107_invert(sh1107, 0);
  sh1107_write_single_command(sh1107, SH1107_SET_DISPLAY_OFFSET | 0x60);
  sh1107_write_single_command(sh1107, SH1107_SET_SEGMENT_REMAP | 0);
  sh1107_write_single_command(sh1107, SH1107_SET_SCAN_DIRECTION | 0);
  sh1107_poweron(sh1107);
}

void sh1107_show(struct sh1107 *sh1107) {
  for (uint page = 0; page < sh1107->height / SH1107_PAGE_SIZE; page++) {
    if (!sh1107->changes[page].changed)
      continue;
    uint start = sh1107->changes[page].start;
    uint end = sh1107->changes[page].end;
    sh1107_set_page(sh1107, page);
    sh1107_set_column(sh1107, start);
    sh1107_write_data(sh1107, sh1107->buff[page] + start, end - start);
  }
}

void sh1107_reset(struct sh1107 *sh1107) {
  gpio_put(sh1107->res, 1);
  sleep_ms(1);
  gpio_put(sh1107->res, 0);
  sleep_ms(20);
  gpio_put(sh1107->res, 1);
  sleep_ms(20);
}

static uint sh1107_char(struct sh1107 *sh1107, uint row, uint col, char chr,
                        uint color, uint size, font_t *font) {
  assert(font != NULL);

  font_char_t *font_char = font_get_char(font, size, chr);

  int start_col = font_char_get_start_col(font_char, col);
  int start_row = font_char_get_start_row(font_char, row);

  for (int i = 0; i < font_char->width; i++) {
    for (int j = 0; j < font_char->height; j++) {
      if (font_char_get_pixel(font_char, j, i)) {
        if (start_col + i >= 0 && start_row + j >= 0 &&
            start_col + i < sh1107->width && start_row + j < sh1107->height)
          sh1107_set_pixel(sh1107, start_row + j, start_col + i, color);
      }
    }
  }

  return start_col + font_char->width;
}

void sh1107_text(struct sh1107 *sh1107, const char *text, uint row, uint col,
                 uint color, uint size, font_t *font,
                 enum text_alignment alignment) {
  assert(sh1107 != NULL);
  assert(font != NULL);
  assert(size % 8 == 0);

  uint text_width = 0;
  for (const char *chr = text; *chr != '\0'; chr++) {
    font_char_t *font_char = font_get_char(font, size, *chr);
    text_width = font_char_get_end_col(font_char, text_width);
  }

  switch (alignment) {
  case text_align_center:
    col -= text_width / 2;
    break;
  case text_align_right:
    col -= text_width;
    break;
  case text_align_left:
    break;
  default:
    panic("unknown alignmnent\n");
  }

  for (const char *chr = text; *chr != '\0'; chr++) {
    col = sh1107_char(sh1107, row, col, *chr, color, size, font);
  }
}