#ifndef _SSD1306_H
#define _SSD1306_H

#include "font.h"
#include "i2c.h"

typedef struct ssd1306_t ssd1306_t;

ssd1306_t *ssd1306_open(int sda, int scl, int line_height);
void ssd1306_close(ssd1306_t *ssd1306);

void ssd1306_set_pixel(ssd1306_t *ssd1306, int row, int col, uint8_t val);
void ssd1306_set_page_col(const ssd1306_t *ssd1306, int page, int col);
void ssd1306_clear(ssd1306_t *ssd1306);
void ssd1306_syncln(const ssd1306_t *ssd1306);
void ssd1306_scrl_down(ssd1306_t *ssd1306);
void ssd1306_putchar(ssd1306_t *ssd1306, const font *font, uint32_t c);
void ssd1306_print(ssd1306_t *ssd1306, const font *font, const uint32_t *str);

#endif
