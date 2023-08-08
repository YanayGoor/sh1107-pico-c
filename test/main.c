#include <fonts/arial.h>
#include <sh1107.h>
#include <stdio.h>

#define SCLK_PIN (10)
#define MOSI_PIN (11)
#define CS_PIN (9)
#define A0_PIN (8)
#define RES_PIN (12)

#define SIZE (20)

int main(void) {
  struct sh1107_spi sh1107_spi = {0};
  struct sh1107 sh1107 = {0};

  sleep_ms(500);
  sh1107_spi_init(&sh1107_spi, spi1, SCLK_PIN, MOSI_PIN, A0_PIN, CS_PIN);
  sh1107_init(&sh1107, &sh1107_spi, NULL, RES_PIN, 128);
  sh1107_fill(&sh1107, 0, 0, 128, 128, 0);
  sh1107_text(&sh1107, "hello world!", 0, 0, 1, 16, &font_arial,
              text_align_left);
  sh1107_show(&sh1107);
  sleep_ms(100);
}