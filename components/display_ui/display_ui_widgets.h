#pragma once

#include "display_ui.h"

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