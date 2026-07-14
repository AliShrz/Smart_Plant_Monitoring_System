#include <stddef.h>
#include <stdint.h>


#ifndef DISPLAY_FONT_H
#define DISPLAY_FONT_H

typedef struct
{
    uint8_t width;
    uint8_t height;
    char first_char;
    char last_char;
    uint8_t spacing; // Optional spacing between characters (in pixels)
    const uint8_t *bitmap;
} display_font_t;

#endif // DISPLAY_FONT_H

size_t display_font_get_char_size_bytes(const display_font_t *font);

const uint8_t *display_font_get_glyph(const display_font_t *font, char c);