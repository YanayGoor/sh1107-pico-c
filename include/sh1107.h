#ifndef SH1107_PICO_C_SH1107_H
#define SH1107_PICO_C_SH1107_H

#include "hardware/gpio.h"
#include "font.h"

#define SH1107_MAX_HEIGHT (128)
#define SH1107_MAX_WIDTH (128)

#define SH1107_PAGE_SIZE (8)

typedef void (sh1107_write_single_command_t)(void *user, uint8_t command);
typedef void (sh1107_write_double_command_t)(void *user, uint8_t command, uint8_t value);
typedef void (sh1107_write_data_t)(void* user, uint8_t *buff, size_t buff_size);

struct sh1107_hw {
    sh1107_write_single_command_t *write_single_command;
    sh1107_write_double_command_t *write_double_command;
    sh1107_write_data_t *write_data;
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
  struct sh1107_hw *hw;
  void* hw_data;
  uint8_t buff[SH1107_MAX_HEIGHT / SH1107_PAGE_SIZE][SH1107_MAX_WIDTH];
  struct line_change changes[SH1107_MAX_HEIGHT / SH1107_PAGE_SIZE];
};

enum text_alignment {
  text_align_left = 0,
  text_align_center,
  text_align_right
};

void sh1107_init(struct sh1107 *sh1107, struct sh1107_hw *hw,
                 void* hw_data, int res, uint height);
void sh1107_show(struct sh1107 *sh1107);
void sh1107_fill(struct sh1107 *sh1107, uint row, uint col, uint width,
                 uint height, bool value);
void sh1107_reset(struct sh1107 *sh1107);
void sh1107_invert(struct sh1107 *sh1107, uint value);
void sh1107_text(struct sh1107 *sh1107, const char *text, uint row, uint col,
                 uint color, uint size, font_t *font,
                 enum text_alignment alignment);

#endif // SH1107_PICO_C_SH1107_H
