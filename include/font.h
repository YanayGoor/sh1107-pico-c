#ifndef SH1107_PICO_C_FONT_H
#define SH1107_PICO_C_FONT_H

#include "pico.h"

typedef struct font_char {
  uint width;
  uint height;
  const char *buff;
  int offset[2];
} font_char_t;

typedef font_char_t font_t[][128];

font_char_t *font_get_char(font_t *font, uint size, char chr);
bool font_char_get_pixel(font_char_t *font_char, uint row, uint col);
int font_char_get_start_col(font_char_t *font_char, uint col);
int font_char_get_end_col(font_char_t *font_char, uint col);
int font_char_get_start_row(font_char_t *font_char, uint row);
int font_char_get_end_row(font_char_t *font_char, uint row);

#endif // SH1107_PICO_C_FONT_H
