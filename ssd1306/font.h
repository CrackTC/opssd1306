#ifndef _FONT_H
#define _FONT_H

#include <stdint.h>

typedef struct font font;

font *font_load(const char *filename);
const uint8_t *get_pixmap(const font *font, uint32_t c, int *width, int *height,
                          int *pitch, int *left, int *top, int *advance);
void font_free(font *font);

#endif
