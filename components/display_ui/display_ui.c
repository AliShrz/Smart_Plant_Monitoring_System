
#include "display.h"
#include "display_ui.h"
#include "display_ui_widgets.h"

#include "display_font_5x7.h"

#include "esp_check.h"

static const char *TAG = "display_ui";

/*==========================
 * Layout
 *==========================*/

#define UI_X               0
#define UI_Y               0

#define UI_WIDTH                128
#define UI_HEIGHT               160

#define UI_MARGIN               2

/* Golden ratio: 160 * 0.618 ≈ 99 */

#define UI_HEADER_HEIGHT        29
#define UI_MAIN_BOTTOM_Y        99

#define UI_CENTER_X             (UI_WIDTH / 2)

/* Bottom section */

#define UI_BOTTOM_Y             UI_MAIN_BOTTOM_Y
#define UI_BOTTOM_HEIGHT        (UI_HEIGHT - UI_BOTTOM_Y)
#define UI_BOTTOM_CENTER_Y      (UI_BOTTOM_Y + UI_BOTTOM_HEIGHT / 2)

/* Main section */

#define UI_MAIN_Y               UI_HEADER_HEIGHT
#define UI_MAIN_HEIGHT          (UI_MAIN_BOTTOM_Y - UI_MAIN_Y)
#define UI_MAIN_CENTER_Y        (UI_MAIN_Y + UI_MAIN_HEIGHT / 2)

/*==========================
 * Header Layout
 *==========================*/
#define HEADER_TITLE_X          3
#define HEADER_TITLE_Y          3

#define HEADER_WIFI_X           60
#define HEADER_WIFI_Y           10

#define HEADER_TIME_X           78
#define HEADER_TIME_Y           3

#define HEADER_DATE_X           60
#define HEADER_DATE_Y           13

/*==========================
 * Widgets Layout
 *==========================*/
#define POT_X              20
#define POT_Y              55

#define SUN_X              95
#define SUN_Y              60

#define TEMP_X             13
#define TEMP_Y             135

#define HUMIDITY_X         80
#define HUMIDITY_Y         135

/* ===========================
 * Private Functions
 * =========================== */

static void display_ui_draw_layout(void);

static void display_ui_draw_header(const system_data_t *data);


/* ===========================
 * Public API
 * =========================== */

esp_err_t display_ui_init(void)
{
    return ESP_OK;
}

esp_err_t display_ui_show(system_state_t *state)
{
    // if (data == NULL)
    // {
    //     return ESP_ERR_INVALID_ARG;
    // }
    display_fill(COLOR_WHITE);

    display_ui_draw_layout();

    display_ui_draw_header(&state->data);

    display_ui_draw_pot( POT_X, POT_Y, &state->data);

    display_ui_draw_sun( SUN_X, SUN_Y, &state->data);

    display_ui_draw_temperature( TEMP_X, TEMP_Y, &state->data);

    display_ui_draw_humidity( HUMIDITY_X, HUMIDITY_Y, &state->data);

    display_ui_draw_wifi( HEADER_WIFI_X, HEADER_WIFI_Y, state);
    
    return ESP_OK;
}

esp_err_t display_ui_deinit(void)
{
    return ESP_OK;
}

static void display_ui_draw_layout(void)
{
    // display_draw_rect(
    //     UI_X,
    //     UI_Y,
    //     UI_WIDTH,
    //     UI_HEIGHT,
    //     COLOR_BLACK);

    // Header separator
    display_draw_hline(
        UI_X,
        UI_HEADER_HEIGHT,
        UI_WIDTH,
        COLOR_BLACK);

    // Golden ratio separator
    display_draw_hline(
        UI_X,
        UI_MAIN_BOTTOM_Y,
        UI_WIDTH,
        COLOR_BLACK);

    // Bottom section separator
    display_draw_vline(
        UI_CENTER_X,
        UI_MAIN_BOTTOM_Y,
        UI_HEIGHT,
        COLOR_BLACK);
}

static void display_ui_draw_header(
    const system_data_t *data)
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