#include "display_font.h"
#include <assert.h>



size_t display_font_get_char_size_bytes(const display_font_t *font)
{
    assert(font != NULL);

    size_t bytes_per_column = (font->height + 7U) / 8U;

    return (size_t)font->width * bytes_per_column;
}

const uint8_t *display_font_get_glyph(const display_font_t *font, char c)
{
    assert(font != NULL);
    if(c < font->first_char || c > font->last_char)
    {
        return NULL;
    }

    size_t index = c - font->first_char;
    return &font->bitmap[index * display_font_get_char_size_bytes(font)];
}