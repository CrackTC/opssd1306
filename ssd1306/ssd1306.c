#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ssd1306.h"

#define SSD1306_I2C_ADDR 0x3C
#define SSD1306_I2C_W 0x00
#define SSD1306_I2C_R 0x01

#define SSD1306_I2C_CMD 0x80
#define SSD1306_I2C_LAST_CMD 0x00
#define SSD1306_I2C_DATA 0xC0
#define SSD1306_I2C_LAST_DATA 0x40

#define OFF (0)
#define ON (1)

enum {
  SSD1306_ADDR_MODE_H = 0x00,
  SSD1306_ADDR_MODE_V = 0x01,
  SSD1306_ADDR_MODE_P = 0x02,
};

#define SSD1306_OPT_COL_LOW 0x00
#define SSD1306_OPT_COL_HIGH 0x10
#define SSD1306_OPT_ADDR_MODE 0x20
#define SSD1306_OPT_COL_RANGE 0x21
#define SSD1306_OPT_PAGE_RANGE 0x22
#define SSD1306_OPT_LINE_START(V) (0x40 | (V))

#define SSD1306_OPT_CONTRAST 0x81
#define SSD1306_OPT_CHARGE_PUMP 0x8D

#define SSD1306_OPT_HREV(V) (0xA0 | (V))
#define SSD1306_OPT_FULL(V) (0xA4 | (V))
#define SSD1306_OPT_INVERSE(V) (0xA6 | (V))
#define SSD1306_OPT_MULTIPLEX 0xA8
#define SSD1306_OPT_DISPLAY(V) (0xAE | (V))
#define SSD1306_OPT_PAGE_START 0xB0
#define SSD1306_OPT_SCAN_DIR_NORMAL 0xC0
#define SSD1306_OPT_SCAN_DIR_REVERSE 0xC8
#define SSD1306_OPT_VERTICAL_OFFSET 0xD3
#define SSD1306_OPT_OSC_FREQ 0xD5
#define SSD1306_OPT_COM_HW 0xDA

struct ssd1306_t {
  i2c_device *i2c;
  uint8_t fb[128 * 8];
  int cur_x, cur_y, line_start;
  int line_height;
};

static void ssd1306_send_cmd(const ssd1306_t *ssd1306, uint8_t cmd) {
  i2c_start(ssd1306->i2c);
  i2c_send_byte(ssd1306->i2c, (SSD1306_I2C_ADDR << 1) | SSD1306_I2C_W);
  i2c_get_ack(ssd1306->i2c);
  i2c_send_byte(ssd1306->i2c, SSD1306_I2C_LAST_CMD);
  i2c_get_ack(ssd1306->i2c);
  i2c_send_byte(ssd1306->i2c, cmd);
  i2c_get_ack(ssd1306->i2c);
  i2c_stop(ssd1306->i2c);
}

static void ssd1306_send_data(const ssd1306_t *ssd1306, const uint8_t *data,
                              int len) {
  i2c_start(ssd1306->i2c);
  i2c_send_byte(ssd1306->i2c, (SSD1306_I2C_ADDR << 1) | SSD1306_I2C_W);
  i2c_get_ack(ssd1306->i2c);
  i2c_send_byte(ssd1306->i2c, SSD1306_I2C_LAST_DATA);
  i2c_get_ack(ssd1306->i2c);
  for (int i = 0; i < len; i++) {
    i2c_send_byte(ssd1306->i2c, data[i]);
    i2c_get_ack(ssd1306->i2c);
  }
  i2c_stop(ssd1306->i2c);
}

static void ssd1306_init(const ssd1306_t *ssd1306) {
  static uint8_t init_cmds[] = {
      SSD1306_OPT_MULTIPLEX,       0x3F,

      SSD1306_OPT_VERTICAL_OFFSET, 0x00,

      SSD1306_OPT_LINE_START(0),

      SSD1306_OPT_HREV(OFF),

      SSD1306_OPT_SCAN_DIR_NORMAL,

      SSD1306_OPT_COM_HW,          0x12,

      SSD1306_OPT_CONTRAST,        0xFF,

      SSD1306_OPT_FULL(OFF),

      SSD1306_OPT_INVERSE(OFF),

      SSD1306_OPT_ADDR_MODE,       SSD1306_ADDR_MODE_P,

      SSD1306_OPT_OSC_FREQ,        0x80,

      SSD1306_OPT_CHARGE_PUMP,     0x14,

      SSD1306_OPT_DISPLAY(ON),
  };

  for (size_t i = 0; i < sizeof(init_cmds) / sizeof(init_cmds[0]); i++) {
    ssd1306_send_cmd(ssd1306, init_cmds[i]);
  }
}

ssd1306_t *ssd1306_open(int sda, int scl, int line_height) {
  ssd1306_t *ssd1306 = malloc(sizeof(ssd1306_t));
  if (!ssd1306)
    return NULL;

  ssd1306->i2c = i2c_open(sda, scl);
  if (!ssd1306->i2c) {
    free(ssd1306);
    return NULL;
  }

  ssd1306_init(ssd1306);

  ssd1306->line_height = line_height;
  ssd1306->cur_x = ssd1306->cur_y = ssd1306->line_start = 0;
  memset(ssd1306->fb, 0, sizeof(ssd1306->fb));

  return ssd1306;
}

void ssd1306_close(ssd1306_t *ssd1306) {
  i2c_close(ssd1306->i2c);
  free(ssd1306);
}

void ssd1306_set_pixel(ssd1306_t *ssd1306, int row, int col, uint8_t val) {
  if (col < 0 || col >= 128) {
    fprintf(stderr, "Invalid pixel position: (%d, %d)\n", row, col);
    return;
  }

  int page = (row >> 3) & 0b111;
  int offset = row & 0b111;

  if (val) {
    ssd1306->fb[(page << 7) + col] |= 1 << offset;
  } else {
    ssd1306->fb[(page << 7) + col] &= ~(1 << offset);
  }
}

void ssd1306_set_page_col(const ssd1306_t *ssd1306, int page, int col) {
  ssd1306_send_cmd(ssd1306, SSD1306_OPT_PAGE_START | page);
  ssd1306_send_cmd(ssd1306, SSD1306_OPT_COL_LOW | (col & 0x0F));
  ssd1306_send_cmd(ssd1306, SSD1306_OPT_COL_HIGH | (col >> 4));
}

void ssd1306_clear(ssd1306_t *ssd1306) {
  memset(ssd1306->fb, 0, sizeof(ssd1306->fb));
  for (int i = 0; i < 8; i++) {
    ssd1306_set_page_col(ssd1306, i, 0);
    ssd1306_send_data(ssd1306, ssd1306->fb + i * 128, 128);
  }
  ssd1306->cur_x = ssd1306->cur_y = ssd1306->line_start = 0;
  ssd1306_send_cmd(ssd1306, SSD1306_OPT_LINE_START(0));
}

void ssd1306_syncln(const ssd1306_t *ssd1306) {
  int cur_page = (ssd1306->cur_y >> 3) & 0b111;
  int end_page = ((ssd1306->cur_y + ssd1306->line_height +
                   (64 % ssd1306->line_height) - 1) >>
                  3) &
                 0b111;
  while (1) {
    ssd1306_set_page_col(ssd1306, cur_page, 0);
    ssd1306_send_data(ssd1306, ssd1306->fb + cur_page * 128, 128);
    if (cur_page == end_page) {
      break;
    }
    cur_page = (cur_page + 1) & 0b111;
  }
}

void ssd1306_scrl_down(ssd1306_t *ssd1306) {
  ssd1306->line_start += ssd1306->line_height;
  for (int i = ssd1306->cur_y;
       i < ssd1306->cur_y + ssd1306->line_height + (64 % ssd1306->line_height);
       i++) {
    for (int j = 0; j < 128; j++) {
      ssd1306_set_pixel(ssd1306, i, j, 0);
    }
  }
  ssd1306_syncln(ssd1306);
  ssd1306_send_cmd(ssd1306, SSD1306_OPT_LINE_START(ssd1306->line_start & 0x3F));
}

static void ssd1306_newline(ssd1306_t *ssd1306) {
  ssd1306_syncln(ssd1306);
  ssd1306->cur_x = 0;
  ssd1306->cur_y += ssd1306->line_height;
  if (ssd1306->cur_y + ssd1306->line_height >= 64) {
    ssd1306_scrl_down(ssd1306);
  }
}

void ssd1306_putchar(ssd1306_t *ssd1306, const font *font, uint32_t c) {
  if (c == L'\n') {
    ssd1306_newline(ssd1306);
    return;
  }

  int width, height, pitch, left, top, advance;
  const uint8_t *pixmap =
      get_pixmap(font, c, &width, &height, &pitch, &left, &top, &advance);
  if (!pixmap) {
    pixmap =
        get_pixmap(font, L'□', &width, &height, &pitch, &left, &top, &advance);
  }

  if (ssd1306->cur_x + left + width >= 128) {
    ssd1306_newline(ssd1306);
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      uint8_t val = pixmap[i * pitch + j / 8];
      val &= (1 << (7 - (j % 8)));
      ssd1306_set_pixel(ssd1306,
                        ssd1306->cur_y + ssd1306->line_height - 1 - top + i,
                        ssd1306->cur_x + left + j, val);
    }
  }

  ssd1306->cur_x += advance;
}

void ssd1306_print(ssd1306_t *ssd1306, const font *font, const uint32_t *str) {
  for (const uint32_t *p = str; *p; p++) {
    ssd1306_putchar(ssd1306, font, *p);
  }
}
