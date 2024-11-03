#include <ft2build.h>
#include <stdio.h>
#include FT_FREETYPE_H

#include "font.h"

struct font {
  FT_Face face;
};

static FT_Library library;

static void init() {
  FT_Error err = FT_Init_FreeType(&library);
  if (err) {
    fprintf(stderr, "Failed to initialize freetype: %d\n", err);
    exit(1);
  }
}

font *font_load(const char *filename) {
  static int initialized = 0;
  if (!initialized) {
    initialized = 1;
    init();
  }

  font *f = malloc(sizeof(font));

  FT_Error err = FT_New_Face(library, filename, 0, &f->face);
  if (err) {
    fprintf(stderr, "Failed to load font: %d\n", err);
    exit(1);
  }

  return f;
}

const uint8_t *get_pixmap(const font *font, uint32_t c, int *width, int *height,
                          int *pitch, int *left, int *top, int *advance) {
  if (FT_Get_Char_Index(font->face, c) == 0) {
    return NULL;
  }

  FT_Error err =
      FT_Load_Char(font->face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_MONO);
  if (err) {
    fprintf(stderr, "Failed to load char: %d\n", err);
    exit(1);
  }

  FT_GlyphSlot slot = font->face->glyph;

  *width = slot->bitmap.width;
  *height = slot->bitmap.rows;
  *pitch = slot->bitmap.pitch;
  *left = slot->bitmap_left;
  *top = slot->bitmap_top;
  *advance = slot->advance.x >> 6;

  return slot->bitmap.buffer;
}

void font_free(font *font) {
  FT_Done_Face(font->face);
  free(font);
}
