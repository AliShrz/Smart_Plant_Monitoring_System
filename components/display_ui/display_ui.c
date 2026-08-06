#include "display_ui.h"
#include "display.h"

#include "display_font_5x7.h"

#include "esp_check.h"

static const char *TAG = "display_ui";

/*==========================
 * Layout
 *==========================*/

#define UI_X               0
#define UI_Y               0

#define UI_WIDTH           128
#define UI_HEIGHT          160

#define UI_MARGIN          2

#define UI_HEADER_HEIGHT   24

#define UI_FOOTER_Y        108

#define UI_CENTER_X        (UI_WIDTH / 2)

/*==========================
 * Header Layout
 *==========================*/
#define HEADER_TITLE_X          3
#define HEADER_TITLE_Y          4

#define HEADER_WIFI_X           64
#define HEADER_WIFI_Y           4

#define HEADER_TIME_X           82
#define HEADER_TIME_Y           4

#define HEADER_DATE_X           66
#define HEADER_DATE_Y           14


/* ===========================
 * Private Functions
 * =========================== */

static void display_ui_draw_layout(void);

static void display_ui_draw_header(const display_ui_data_t *data);

static void display_ui_draw_soil(const display_ui_data_t *data);

static void display_ui_draw_light(const display_ui_data_t *data);

static void display_ui_draw_temperature(const display_ui_data_t *data);

static void display_ui_draw_humidity(const display_ui_data_t *data);

/* ===========================
 * Public API
 * =========================== */

esp_err_t display_ui_init(void)
{
    return ESP_OK;
}

esp_err_t display_ui_show(display_ui_data_t *data)
{
    // if (data == NULL)
    // {
    //     return ESP_ERR_INVALID_ARG;
    // }
    display_fill(COLOR_WHITE);

    display_ui_draw_layout();

    display_ui_draw_header(data);
    
    return ESP_OK;
}

esp_err_t display_ui_deinit(void)
{
    return ESP_OK;
}

static void display_ui_draw_layout(void)
{
    display_draw_rect(
        0,
        0,
        UI_WIDTH,
        UI_HEIGHT,
        COLOR_BLACK);

    display_draw_hline(
        0,
        UI_HEADER_HEIGHT,
        UI_WIDTH,
        COLOR_BLACK);

    display_draw_hline(
        0,
        UI_FOOTER_Y,
        UI_WIDTH,
        COLOR_BLACK);

    display_draw_vline(
        UI_CENTER_X,
        UI_FOOTER_Y,
        UI_HEIGHT,
        COLOR_BLACK);
}

static void display_ui_draw_header(
    const display_ui_data_t *data)
{
    display_printf(
        HEADER_TITLE_X,
        HEADER_TITLE_Y,
        &display_font_5x7,
        COLOR_BLACK,
        COLOR_WHITE,
        DISPLAY_BACKGROUND_TRANSPARENT,
        "Plant",
        data->plant_id);

    display_printf(
        HEADER_TITLE_X,
        HEADER_TITLE_Y + 10,
        &display_font_5x7,
        COLOR_BLACK,
        COLOR_WHITE,
        DISPLAY_BACKGROUND_TRANSPARENT,
        "#%u",
        data->plant_id);

    display_printf(
        HEADER_TIME_X,
        HEADER_TIME_Y,
        &display_font_5x7,
        COLOR_BLACK,
        COLOR_WHITE,
        DISPLAY_BACKGROUND_TRANSPARENT,
        "%s",
        data->time);

    display_printf(
        HEADER_DATE_X,
        HEADER_DATE_Y,
        &display_font_5x7,
        COLOR_BLACK,
        COLOR_WHITE,
        DISPLAY_BACKGROUND_TRANSPARENT,
        "%s",
        data->date);
}