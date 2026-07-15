// to do: 

// display_draw_image(), 
// display_print()
// display_printf()


#include "display.h"
// #include "display_font.c"
#include "esp_lcd_panel_io.h"
#include "driver/spi_master.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_st7735.h"
#include "driver/gpio.h"
#include "math.h"
#include <stdarg.h>

// #include "display_font_5x7.h"
// #include "esp_check.h"

#include "esp_log.h"

static const char *TAG = "display";

// SPI Host
#define LCD_HOST    SPI2_HOST

// SPI Pins
#define PIN_NUM_MOSI   23
#define PIN_NUM_CLK    18
#define PIN_NUM_CS     5
#define PIN_NUM_DC     27
#define PIN_NUM_RST    26
#define PIN_NUM_BCKL   25

// Display Resolution
#define LCD_H_RES    128
#define LCD_V_RES    160

// LCD Configuration
#define LCD_PIXEL_CLOCK_HZ (10 * 1000 * 1000)
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8
#define LCD_SPI_MODE 0
#define LCD_TRANSACTION_QUEUE_DEPTH 1

static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;

#define DISPLAY_BUFFER_LINES    20
#define DISPLAY_BUFFER_SIZE (LCD_H_RES * DISPLAY_BUFFER_LINES)
static uint16_t display_buffer[DISPLAY_BUFFER_SIZE];
#define DISPLAY_PRINTF_BUFFER_SIZE 128

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static volatile bool display_transfer_done = true;

static esp_err_t display_draw_char_solid( int x, int y, char c, const display_font_t *font, uint16_t color, uint16_t background_color);

static esp_err_t display_draw_char_transparent(int x, int y, char c, const display_font_t *font, uint16_t color);



static bool display_color_transfer_done(
    esp_lcd_panel_io_handle_t panel_io,
    esp_lcd_panel_io_event_data_t *edata,
    void *user_ctx)
{
    display_transfer_done = true;
    return false;
}

static esp_err_t spi_bus_init(void)
{
    spi_bus_config_t bus_config = {
    .sclk_io_num = PIN_NUM_CLK,
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = -1,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz =
    LCD_H_RES *
    DISPLAY_BUFFER_LINES *
    sizeof(uint16_t), // transfer 80 lines of pixels
    };
    return spi_bus_initialize(LCD_HOST, &bus_config, SPI_DMA_CH_AUTO); // Enable the DMA feature;
}

static esp_err_t lcd_io_init(void)
{
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_DC,
        .cs_gpio_num = PIN_NUM_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = LCD_SPI_MODE,
        .trans_queue_depth = LCD_TRANSACTION_QUEUE_DEPTH,
    };

    esp_err_t ret = esp_lcd_new_panel_io_spi(
        LCD_HOST,
        &io_config,
        &io_handle);

    if (ret != ESP_OK)
    {
        return ret;
    }

    esp_lcd_panel_io_callbacks_t callbacks = {
        .on_color_trans_done = display_color_transfer_done,
    };

    return esp_lcd_panel_io_register_event_callbacks(
        io_handle,
        &callbacks,
        NULL);
}

static esp_err_t lcd_panel_init(void)
{
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };
    return esp_lcd_new_panel_st7735(io_handle, &panel_config, &panel_handle);
}

static esp_err_t backlight_init(void)
{
    gpio_config_t io_config = {
        .pin_bit_mask = (1ULL << PIN_NUM_BCKL),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    return gpio_config(&io_config);
}

static esp_err_t panel_reset(void)
{
    return esp_lcd_panel_reset(panel_handle);
}

static esp_err_t panel_initialize(void)
{
    return esp_lcd_panel_init(panel_handle);
}

static esp_err_t panel_display_on(void)
{
    return esp_lcd_panel_disp_on_off(panel_handle, true);
}

static esp_err_t panel_display_off(void)
{
    return esp_lcd_panel_disp_on_off(panel_handle, false);
}

static esp_err_t backlight_on(void)
{
    return gpio_set_level(PIN_NUM_BCKL, 1);
}

static esp_err_t backlight_off(void)
{
    return gpio_set_level(PIN_NUM_BCKL, 0);
}

esp_err_t display_init(void)
{
    esp_err_t ret;

    ret = spi_bus_init();
    if (ret != ESP_OK) return ret;

    ret = lcd_io_init();
    if (ret != ESP_OK) return ret;

    ret = lcd_panel_init();
    if (ret != ESP_OK) return ret;

    ret = panel_reset();
    if (ret != ESP_OK) return ret;

    ret = panel_initialize();
    if (ret != ESP_OK) return ret;

    ret = backlight_init();
    if (ret != ESP_OK) return ret;

    ret = panel_display_on();
    if (ret != ESP_OK) return ret;

    ret = backlight_on();
    if (ret != ESP_OK) return ret;

    return ESP_OK;
}

static inline uint16_t display_format_color(uint16_t color)
{
    return (color >>8) | (color << 8);
}

static inline void display_wait_transfer_done(void)
{
    while (!display_transfer_done)
    {
    }
}

static inline void display_begin_transfer(void)
{
    display_transfer_done = false;
}

esp_err_t display_fill(uint16_t color)
{
    if (panel_handle == NULL)
    {
    return ESP_ERR_INVALID_STATE;
    }

    uint16_t lcd_color = display_format_color(color);

    for (size_t i = 0; i < DISPLAY_BUFFER_SIZE; i++)
    {
        display_buffer[i] = lcd_color;
    }
    size_t current_y = 0;

    while (current_y < LCD_V_RES)
    {
        display_wait_transfer_done();
        display_begin_transfer();

        size_t lines_to_send = MIN(DISPLAY_BUFFER_LINES, LCD_V_RES - current_y);
        esp_err_t ret = esp_lcd_panel_draw_bitmap(panel_handle,0, current_y, LCD_H_RES, current_y + lines_to_send, display_buffer);
        if (ret != ESP_OK) 
        {
            return ret;
        }
        current_y += lines_to_send;
    }

    return ESP_OK;
}

esp_err_t display_draw_pixel(int x, int y, uint16_t color)
{
    if (panel_handle == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (x < 0 || x >= LCD_H_RES || y < 0 || y >= LCD_V_RES)
    {
        return ESP_ERR_INVALID_ARG;
    }
    uint16_t lcd_color = display_format_color(color);
    display_wait_transfer_done();
    display_begin_transfer();
    esp_err_t ret = esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 1, y + 1, &lcd_color);
    return ret;
}

esp_err_t display_fill_rect(int x_start, int y_start, int x_end, int y_end, uint16_t color)
{
    if (panel_handle == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (x_start < 0 || x_start >= LCD_H_RES || y_start < 0 || y_start >= LCD_V_RES || x_end < 0 || x_end >= LCD_H_RES || y_end < 0 || y_end >= LCD_V_RES || x_end <= x_start || y_end <= y_start)
    {
        return ESP_ERR_INVALID_ARG;
    }

    uint16_t lcd_color = display_format_color(color);
    size_t rect_width = x_end - x_start;
    size_t current_y = y_start;

    while (current_y < y_end)
    {
        size_t lines_to_send = MIN(DISPLAY_BUFFER_LINES, y_end - current_y);
        size_t pixels_to_fill = rect_width * lines_to_send;

        display_wait_transfer_done();

        for (size_t i = 0; i < pixels_to_fill; i++)
        {
            display_buffer[i] = lcd_color;
        }
        
        display_begin_transfer();

        esp_err_t ret = esp_lcd_panel_draw_bitmap(panel_handle,x_start, current_y,x_end, current_y + lines_to_send, display_buffer);
        if (ret != ESP_OK) 
        {
            return ret;
        }
        current_y += lines_to_send;
    }
    return ESP_OK;
}

esp_err_t display_draw_hline(int x_start, int y, int x_end, uint16_t color)
{
    if (x_start == x_end)
    {
        return display_draw_pixel(x_start, y, color);
    }
    return display_fill_rect(x_start, y, x_end, y + 1, color);
}

esp_err_t display_draw_vline(int x, int y_start, int y_end, uint16_t color)
{
    if(y_start == y_end)
    {
        return display_draw_pixel(x, y_start, color);
    }
    else
    {
        return display_fill_rect(x, y_start, x + 1, y_end, color);
    }
}

esp_err_t display_draw_rect(int x_start, int y_start, int x_end, int y_end, uint16_t color)
{
    esp_err_t ret;

    ret = display_draw_hline(x_start, y_start, x_end, color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_hline(x_start, y_end - 1, x_end, color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_vline(x_start, y_start, y_end, color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_vline(x_end - 1, y_start, y_end, color);
    if (ret != ESP_OK) return ret;

    return ESP_OK;
}


esp_err_t display_draw_line_dda(int x_start, int y_start, int x_end, int y_end, uint16_t color)
{
    int dx = x_end - x_start;
    int dy = y_end - y_start;
    uint16_t steps = MAX(abs(dx), abs(dy));

    if (steps == 0)
    {
        return display_draw_pixel(x_start, y_start, color);
    }

    float x_increment = dx / (float)steps;
    float y_increment = dy / (float)steps;

    float x = x_start;
    float y = y_start;
    for (int i = 0; i <= steps; i++)
    {
        x += x_increment;
        y += y_increment;
        esp_err_t ret = display_draw_pixel(lroundf(x), lroundf(y), color);
        if (ret != ESP_OK)
        {
            return ret;
        }
    }

    return ESP_OK;
}

esp_err_t display_draw_line_int(int x_start, int y_start, int x_end, int y_end, uint16_t color)
{
    int dx = x_end - x_start;
    int dy = y_end - y_start;
    uint16_t steps = MAX(abs(dx), abs(dy));

    if (steps == 0)
    {
        return display_draw_pixel(x_start, y_start, color);
    }

    int error = 0;
    int x = x_start;
    int y = y_start;
    int sx = (x_end >= x_start) ? 1 : -1;
    int sy = (y_end >= y_start) ? 1 : -1;

    display_draw_pixel(x, y, color);

    for(int i = 0; i <= steps; i++)
    {
        if (abs(dx) > abs(dy))
        {
            x += sx;
            error += abs(dy);
            if(error >= abs(dx))
            {
                y += sy;
                error -= abs(dx);
            }
        }
        else
        {
            y += sy;
            error += abs(dx);
            if(error >= abs(dy))
            {
                x += sx;
                error -= abs(dy);
            }
        }
        esp_err_t ret = display_draw_pixel(x, y, color);
        if (ret != ESP_OK)
        {
            return ret;
        }
    }

    return ESP_OK;
}


esp_err_t display_draw_line_bresenham(int x_start, int y_start, int x_end, int y_end, uint16_t color)
{
    if (panel_handle == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (x_start == x_end && y_start == y_end)
    {
        return display_draw_pixel(x_start, y_start, color);
    }

    int dx = abs(x_end - x_start);
    int dy = abs(y_end - y_start);

    int error = dx - dy;
    int x = x_start;
    int y = y_start;
    int sx = (x_start < x_end) ? 1 : -1;
    int sy = (y_start < y_end) ? 1 : -1;

    esp_err_t ret = display_draw_pixel(x, y, color);
    if (ret != ESP_OK)
    {
        return ret;
    }

    while (x != x_end || y != y_end)
    {
        int error2 = error << 1; // equivalent to error * 2
        if (error2 > -dy)
        {
            error -= dy;
            x += sx;
        }
        if (error2 < dx)
        {
            error += dx;
            y += sy;
        }
        esp_err_t ret = display_draw_pixel(x, y, color);
        if (ret != ESP_OK)
        {
            return ret;
        }
    }

    return ESP_OK;
}


esp_err_t display_draw_line(int x_start, int y_start, int x_end, int y_end, uint16_t color)
{
    return display_draw_line_bresenham(x_start, y_start, x_end, y_end, color);
}

static esp_err_t display_draw_circle_points(int x_center, int y_center, int x, int y, uint16_t color)
{
    esp_err_t ret;

    ret = display_draw_pixel(x_center + x, y_center + y, color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_pixel(x_center - x, y_center + y, color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_pixel(x_center + x, y_center - y, color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_pixel(x_center - x, y_center - y, color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_pixel(x_center + y, y_center + x, color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_pixel(x_center - y, y_center + x, color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_pixel(x_center + y, y_center - x, color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_pixel(x_center - y, y_center - x, color);
    if (ret != ESP_OK) return ret;

    return ESP_OK;
}

esp_err_t display_draw_circle(int x_center, int y_center, int radius, uint16_t color)
{
    if (panel_handle == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }
    const uint16_t lcd_color = display_format_color(color);

    if (radius < 0)
    {
        return ESP_ERR_INVALID_ARG;
    }
    if (radius == 0)
    {
        return display_draw_pixel(x_center, y_center, lcd_color);
    }

    int d = 3 - 2 * radius;    
    int x = 0;
    int y = radius;


    while(x <= y)
    {
        esp_err_t ret = display_draw_circle_points(x_center, y_center, x, y, lcd_color);
        if (ret != ESP_OK)
        {
            return ret;
        }

        if(d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;

    }
    
    return ESP_OK;
}

static esp_err_t display_fill_circle_lines(int x_center, int y_center, int x, int y, uint16_t color)
{

    esp_err_t ret;
    const uint16_t lcd_color = display_format_color(color);

    ret = display_draw_hline(x_center - x, y_center + y, x_center + x, lcd_color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_hline(x_center - x, y_center - y, x_center + x, lcd_color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_hline(x_center - y, y_center + x, x_center + y, lcd_color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_hline(x_center - y, y_center - x, x_center + y, lcd_color);
    if (ret != ESP_OK) return ret;

    return ESP_OK;
}

esp_err_t display_fill_circle(int x_center, int y_center, int radius, uint16_t color)
{
    if (panel_handle == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    const uint16_t lcd_color = display_format_color(color);

    if (radius < 0)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (radius == 0)
    {
        return display_draw_pixel(x_center, y_center, lcd_color);
    }

    int d = 3-2 * radius;
    int x = 0;
    int y = radius;

    while(x <= y)
    {
        esp_err_t ret = display_fill_circle_lines(x_center, y_center, x, y, lcd_color);
        if (ret != ESP_OK) 
        {
            return ret;
        }

        if(d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;

    }
    
    return ESP_OK;
}

esp_err_t display_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color)
{
    esp_err_t ret;
    const uint16_t lcd_color = display_format_color(color);

    ret = display_draw_line(x1, y1, x2, y2, lcd_color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_line(x2, y2, x3, y3, lcd_color);
    if (ret != ESP_OK) return ret;

    ret = display_draw_line(x3, y3, x1, y1, lcd_color);
    if (ret != ESP_OK) return ret;

    return ESP_OK;
}

typedef struct
{
    int x;
    int y;
} display_point_t;

static inline void swap_points(display_point_t *a, display_point_t *b)
{
    display_point_t temp = *a;
    *a = *b;
    *b = temp;
}


static void edge_update(
    int *x,
    int *error,
    int dx,
    int dy,
    int sx)
{
    if (dy <= 0)
    {
        return;
    }

    *error += dx;

    while (*error >= dy)
    {
        *error -= dy;
        *x += sx;
    }
}

esp_err_t display_fill_triangle(int x1, int y1,
                                int x2, int y2,
                                int x3, int y3,
                                uint16_t color)
{
    // Sort vertices by descending y
    display_point_t p1 = {x1, y1};
    display_point_t p2 = {x2, y2};
    display_point_t p3 = {x3, y3};

    if (p1.y < p2.y) swap_points(&p1, &p2);
    if (p2.y < p3.y) swap_points(&p2, &p3);
    if (p1.y < p2.y) swap_points(&p1, &p2);

    const uint16_t lcd_color = display_format_color(color);

    // ---------- Flat Top ----------
    if (p1.y == p2.y)
    {
        if (p1.x > p2.x)
            swap_points(&p1, &p2);

        int y = p1.y;

        int x_left = p1.x;
        int dx_left = abs(p3.x - p1.x);
        int dy_left = abs(p3.y - p1.y);
        int error_left = 0;
        int sx_left = (p1.x < p3.x) ? 1 : -1;

        int x_right = p2.x;
        int dx_right = abs(p3.x - p2.x);
        int dy_right = abs(p3.y - p2.y);
        int error_right = 0;
        int sx_right = (p2.x < p3.x) ? 1 : -1;

        while (y >= p3.y)
        {
            esp_err_t ret = display_draw_hline(
                MIN(x_left, x_right),
                y,
                MAX(x_left, x_right),
                lcd_color);

            if (ret != ESP_OK)
                return ret;

            edge_update(&x_left, &error_left, dx_left, dy_left, sx_left);
            edge_update(&x_right, &error_right, dx_right, dy_right, sx_right);

            y--;
        }

        return ESP_OK;
    }

    // ---------- General + Flat Bottom ----------

    int y = p1.y;

    int x_left = p1.x;
    int dx_left = abs(p2.x - p1.x);
    int dy_left = abs(p2.y - p1.y);
    int error_left = 0;
    int sx_left = (p1.x < p2.x) ? 1 : -1;

    int x_right = p1.x;
    int dx_right = abs(p3.x - p1.x);
    int dy_right = abs(p3.y - p1.y);
    int error_right = 0;
    int sx_right = (p1.x < p3.x) ? 1 : -1;
    
    while (y >= p2.y)
    {
        esp_err_t ret = display_draw_hline(
            MIN(x_left, x_right),
            y,
            MAX(x_left, x_right),
            lcd_color);

        if (ret != ESP_OK)
            return ret;

        edge_update(&x_left, &error_left, dx_left, dy_left, sx_left);
        edge_update(&x_right, &error_right, dx_right, dy_right, sx_right);

        y--;
    }

    // ---------- Flat Bottom ----------
    if (p2.y == p3.y)
    {
        return ESP_OK;
    }

    // ---------- Lower Half ----------

    x_left = p2.x;
    dx_left = abs(p3.x - p2.x);
    dy_left = abs(p3.y - p2.y);
    error_left = 0;
    sx_left = (p2.x < p3.x) ? 1 : -1;

    while (y >= p3.y)
    {
        esp_err_t ret = display_draw_hline(
            MIN(x_left, x_right),
            y,
            MAX(x_left, x_right),
            lcd_color);

        if (ret != ESP_OK)
            return ret;

        edge_update(&x_left, &error_left, dx_left, dy_left, sx_left);
        edge_update(&x_right, &error_right, dx_right, dy_right, sx_right);

        y--;
    }

    return ESP_OK;
}

static esp_err_t display_draw_char_solid( int x, int y, char c, const display_font_t *font, uint16_t color, uint16_t background_color)
{
    assert(font != NULL);

    const uint8_t *glyph = display_font_get_glyph(font, c);

    if (glyph == NULL)
    {
        glyph = display_font_get_glyph(font, font->fallback_char);

        if (glyph == NULL)
        {
            return ESP_ERR_INVALID_ARG;
        }
    }
    const uint16_t lcd_color = display_format_color(color);
    const uint16_t lcd_background = display_format_color(background_color);

    // uint16_t buffer[font->width * font->height];
    uint16_t *buffer = display_buffer;
    // static uint16_t buffer[5 * 7];

    display_wait_transfer_done();

    for (int row = 0; row < font->height; row++)
    {
        for (int col = 0; col < font->width; col++)
        {
            bool pixel = ((glyph[col] >> row) & 0x01U);
            buffer[row * font->width + col] = pixel ? lcd_color : lcd_background;
        }
    }

    display_begin_transfer();

    esp_err_t ret = esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + font->width, y + font->height, buffer);
    return ret;
}


static esp_err_t display_draw_char_transparent(int x, int y, char c, const display_font_t *font, uint16_t color)
{
    assert(font != NULL);

    const uint8_t *glyph = display_font_get_glyph(font, c);

    if (glyph == NULL)
    {
        glyph = display_font_get_glyph(font, font->fallback_char);

        if (glyph == NULL)
        {
            return ESP_ERR_INVALID_ARG;
        }
    }
    // const uint16_t lcd_color = display_format_color(color);

    // uint16_t buffer[font->width * font->height];
    // uint16_t *buffer = display_buffer;
    // static uint16_t buffer[5 * 7];


    for (int row = 0; row < font->height; row++)
    {
        for (int col = 0; col < font->width; col++)
        {
            bool pixel = ((glyph[col] >> row) & 0x01U);
            if (pixel)
            {
                esp_err_t ret = display_draw_pixel(x + col, y + row, color);
                if (ret != ESP_OK)
                {
                    return ret;
                }
            }
        }
    }
    return ESP_OK;
}

esp_err_t display_draw_char( int x, int y, char c, const display_font_t *font, uint16_t color, uint16_t background_color, display_background_mode_t background_mode)
{
    if (background_mode == DISPLAY_BACKGROUND_TRANSPARENT)
    {
        return display_draw_char_transparent(x, y, c, font, color);
    }

    return display_draw_char_solid(x, y, c, font, color, background_color);

}

esp_err_t display_draw_string( int x, int y, const char *text, const display_font_t *font, uint16_t color, uint16_t background_color, display_background_mode_t background_mode)
{
    assert(text != NULL);
    assert(font != NULL);
    const char *p = text;
    while(*p != '\0')
    {
        esp_err_t ret = display_draw_char(x, y, *p, font, color, background_color, background_mode);
        if (ret != ESP_OK)
        {
            return ret;
        }
        x += font->width + font->spacing;
        p++;
    }
    return ESP_OK;

}

esp_err_t display_printf( int x, int y, const display_font_t *font, uint16_t color, uint16_t background_color, display_background_mode_t background_mode, const char *format, ...)
{
    assert(format != NULL);
    assert(font != NULL);
    
    char buffer[DISPLAY_PRINTF_BUFFER_SIZE];

    va_list args;

    va_start(args, format);

    int length = vsnprintf(buffer, sizeof(buffer), format, args);

    if (length < 0)
    {
        return ESP_FAIL;
    }

    if (length >= sizeof(buffer))
    {
        return ESP_ERR_NO_MEM;
    }
    va_end(args);
    return display_draw_string( x, y, buffer, font, color, background_color, background_mode);
}