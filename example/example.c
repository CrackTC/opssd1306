#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <wiringPi.h>

#include "font.h"
#include "ssd1306.h"

int main(void) {
  ssd1306_t *ssd1306 = ssd1306_open(0, 1, 12);
  ssd1306_clear(ssd1306);

  const uint32_t *str = L"你好，世界！\nこんにちは、せかい！안녕하세요, "
                        L"세계! A quick brown fox jumps over the lazy "
                        L"dog.";

  font *font = font_load("./fonts/wenquanyi_9pt.pcf");
  ssd1306_print(ssd1306, font, str);
  ssd1306_syncln(ssd1306);

  while (1) {
    int c = getchar();
    if (c == EOF)
      break;
    ssd1306_putchar(ssd1306, font, c);
  }

  font_free(font);
  ssd1306_close(ssd1306);

  return 0;
}
