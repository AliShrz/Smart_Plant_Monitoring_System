#pragma once

#include "display_ui.h"

/*==========================
 * UI Theme
 *==========================*/

#define UI_COLOR_BACKGROUND      COLOR_WHITE

#define UI_COLOR_TEXT            COLOR_BLACK
#define UI_COLOR_BORDER          COLOR_BLACK

#define UI_COLOR_POT             0xFBA0 // 0xFC00
#define UI_COLOR_POT_BORDER      0xB2E2
#define UI_COLOR_WATER           COLOR_LIGHT_BLUE

#define UI_COLOR_LEAF            COLOR_GREEN
#define UI_COLOR_STEM            COLOR_DARK_GREEN

#define UI_COLOR_SUN             0xFEE0
#define UI_COLOR_SUN_RAY         0xFE20

#define UI_COLOR_TEMPERATURE     COLOR_RED

#define UI_COLOR_HUMIDITY        COLOR_CYAN

#define UI_COLOR_WIFI            COLOR_BLACK
#define UI_COLOR_WIFI_DISABLED   COLOR_LIGHT_GRAY



void display_ui_draw_pot(
    int x,
    int y,
    const display_ui_data_t *data);

void display_ui_draw_sun(
    int x,
    int y,
    const display_ui_data_t *data);

void display_ui_draw_temperature(
    int x,
    int y,
    const display_ui_data_t *data);

void display_ui_draw_humidity(
    int x,
    int y,
    const display_ui_data_t *data);

void display_ui_draw_wifi(
    int x,
    int y,
    const display_ui_data_t *data);