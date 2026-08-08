# include "display_ui_widgets.h"
#include "display.h"
#include "display_font_5x7.h"


/*==========================
 * Pot Widget
 *==========================*/

#define POT_WIDTH              22
#define POT_RIM_HEIGHT          4
#define POT_BOTTOM_WIDTH       12
#define POT_BODY_HEIGHT        20
#define POT_RIM_LEFT_OVERHANG    1
#define POT_RIM_RIGHT_OVERHANG   2
#define POT_FILL_OFFSET_X    1

/*==========================
 * Plant Widget
 *==========================*/

#define PLANT_STEM_WIDTH       2
#define PLANT_STEM_HEIGHT      9

#define PLANT_LEAF_WIDTH       6
#define PLANT_LEAF_HEIGHT      5

#define POT_TEXT_OFFSET_Y      6
#define POT_PERCENT_X_OFFSET    8

/*==========================
 * Sun Widget
 *==========================*/

#define SUN_RADIUS                 8

#define SUN_RAY_COUNT              8

#define SUN_RAY_GAP                2

#define SUN_RAY_MIN_LENGTH         2
#define SUN_RAY_MAX_LENGTH         8

#define SUN_TEXT_OFFSET_Y          6
#define MAX_LIGHT_LUX              10000


static void display_ui_draw_sun_rays(
    int x,
    int y,
    int ray_length,
    uint16_t color);

static void display_ui_draw_data_percentage(
    int center_x,
    int bottom_y,
    uint8_t data_percentage);

    typedef struct
{
    int8_t dx;
    int8_t dy;

} display_vector2i_t;

static void display_ui_draw_plant(
    int x,
    int y)
{
    /*==========================
     * Stem
     *==========================*/

    display_fill_rect(
        x,
        y - PLANT_STEM_HEIGHT,
        x + PLANT_STEM_WIDTH,
        y,
        UI_COLOR_STEM);

    display_draw_rect(
        x,
        y - PLANT_STEM_HEIGHT,
        x + PLANT_STEM_WIDTH,
        y,
        UI_COLOR_BORDER);

    /*==========================
     * Left leaf
     *==========================*/

    display_fill_triangle(
        x,
        y - 5,

        x - 6,
        y - 9,

        x - 3,
        y - 1,

        UI_COLOR_LEAF);

    display_draw_triangle(
        x,
        y - 5,

        x - 6,
        y - 9,

        x - 3,
        y - 1,

        UI_COLOR_BORDER);

    /*==========================
     * Right leaf
     *==========================*/

    display_fill_triangle(
        x + PLANT_STEM_WIDTH,
        y - 5,

        x + PLANT_STEM_WIDTH + 6,
        y - 9,

        x + PLANT_STEM_WIDTH + 3,
        y - 1,

        UI_COLOR_LEAF);

    display_draw_triangle(
        x + PLANT_STEM_WIDTH,
        y - 5,

        x + PLANT_STEM_WIDTH + 6,
        y - 9,

        x + PLANT_STEM_WIDTH + 3,
        y - 1,

        UI_COLOR_BORDER);

    /*==========================
     * Top leaf
     *==========================*/

    display_fill_triangle(
        x + 1,
        y - PLANT_STEM_HEIGHT - 5,

        x - 3,
        y - PLANT_STEM_HEIGHT + 1,

        x + 5,
        y - PLANT_STEM_HEIGHT + 1,

        UI_COLOR_LEAF);

    display_draw_triangle(
        x + 1,
        y - PLANT_STEM_HEIGHT - 5,

        x - 3,
        y - PLANT_STEM_HEIGHT + 1,

        x + 5,
        y - PLANT_STEM_HEIGHT + 1,

        UI_COLOR_BORDER);
}

static void display_ui_draw_pot_water(
    int top_left_x,
    int top_right_x,
    int bottom_left_x,
    int bottom_right_x,
    int top_y,
    int bottom_y,
    uint8_t moisture)
{
    if (moisture == 0)
    {
        return;
    }

    if (moisture > 100)
    {
        moisture = 100;
    }

    /* Water height */

    int pot_height = bottom_y - top_y;

    int water_height =
        (pot_height * moisture) / 100;

    int water_top_y =
        bottom_y - water_height;

    /* Water top width (linear interpolation) */

    int water_top_left_x =
        top_left_x +
        ((bottom_left_x - top_left_x) *
         (water_top_y - top_y)) /
        pot_height;

    int water_top_right_x =
        top_right_x +
        ((bottom_right_x - top_right_x) *
         (water_top_y - top_y)) /
        pot_height;

    /* Draw */

    display_fill_trapezoid(
        water_top_left_x + POT_FILL_OFFSET_X,
        water_top_right_x,
        water_top_y,
        bottom_left_x + POT_FILL_OFFSET_X,
        bottom_right_x,
        bottom_y,
        UI_COLOR_WATER);
}

static void display_ui_draw_data_percentage(
    int center_x,
    int bottom_y,
    uint8_t data_percentage)
{
    char text[5];

    snprintf(
        text,
        sizeof(text),
        "%u%%",
        data_percentage);

    display_printf(
        center_x - POT_PERCENT_X_OFFSET,
        bottom_y + POT_TEXT_OFFSET_Y,
        &display_font_5x7,
        UI_COLOR_TEXT,
        UI_COLOR_BACKGROUND,
        DISPLAY_BACKGROUND_TRANSPARENT,
        "%s",
        text);
}

// =========================

void display_ui_draw_pot(
    int x,
    int y,
    const display_ui_data_t *data)
{
    (void)data;


    /* Rim */

    int rim_left_x   = x;
    int rim_right_x  = x + POT_WIDTH;

    int rim_top_y    = y;
    int rim_bottom_y = y + POT_RIM_HEIGHT;

    /* Pot body */

    int body_top_left_x     = rim_left_x;
    int body_top_right_x    = rim_right_x;

    int body_bottom_left_x =
        x + (POT_WIDTH - POT_BOTTOM_WIDTH) / 2;

    int body_bottom_right_x =
        body_bottom_left_x + POT_BOTTOM_WIDTH;

    int body_top_y    = rim_bottom_y;

    int body_bottom_y = body_top_y + POT_BODY_HEIGHT;


    /* Plant */

    display_ui_draw_plant(
        (body_top_left_x + body_top_right_x) / 2,
        rim_top_y);

    /* pot */
    display_fill_rect(
        rim_left_x - POT_RIM_LEFT_OVERHANG,
        rim_top_y,
        rim_right_x + POT_RIM_RIGHT_OVERHANG,
        rim_bottom_y,
        UI_COLOR_POT);

    display_fill_trapezoid(
        body_top_left_x + POT_FILL_OFFSET_X,
        body_top_right_x,
        body_top_y,
        body_bottom_left_x + POT_FILL_OFFSET_X,
        body_bottom_right_x,
        body_bottom_y,
        UI_COLOR_POT);

    /* Water */
    display_ui_draw_pot_water(
        body_top_left_x,
        body_top_right_x,
        body_bottom_left_x,
        body_bottom_right_x,
        body_top_y,
        body_bottom_y,
        data->soil_moisture_percent);

    /* Outline */
    display_draw_rect(
        rim_left_x - POT_RIM_LEFT_OVERHANG,
        rim_top_y,
        rim_right_x + POT_RIM_RIGHT_OVERHANG,
        rim_bottom_y,
        UI_COLOR_POT_BORDER);

    display_draw_trapezoid(
        body_top_left_x,
        body_top_right_x,
        body_top_y,
        body_bottom_left_x,
        body_bottom_right_x,
        body_bottom_y,
        UI_COLOR_POT_BORDER);

    display_ui_draw_data_percentage(
    (body_top_left_x + body_top_right_x) / 2,
    body_bottom_y,
    data->soil_moisture_percent);
}

/**************** sun *****************/

static const display_vector2i_t sun_rays[SUN_RAY_COUNT] =
{
    {  0, -1 },   // Up
    {  1, -1 },   // Up Right
    {  1,  0 },   // Right
    {  1,  1 },   // Down Right
    {  0,  1 },   // Down
    { -1,  1 },   // Down Left
    { -1,  0 },   // Left
    { -1, -1 },   // Up Left
};

static void display_ui_draw_sun_rays(
    int x,
    int y,
    int ray_length,
    uint16_t color)
{
    for (int i = 0; i < SUN_RAY_COUNT; i++)
    {
        int x_start =
            x + sun_rays[i].dx * (SUN_RADIUS + SUN_RAY_GAP);

        int y_start =
            y + sun_rays[i].dy * (SUN_RADIUS + SUN_RAY_GAP);

        int x_end =
            x + sun_rays[i].dx *
            (SUN_RADIUS + SUN_RAY_GAP + ray_length);

        int y_end =
            y + sun_rays[i].dy *
            (SUN_RADIUS + SUN_RAY_GAP + ray_length);

        display_draw_line(
            x_start,
            y_start,
            x_end,
            y_end,
            color);
    }
}


void display_ui_draw_sun(
    int x,
    int y,
    const display_ui_data_t *data)
{
    uint32_t lux = data->light_lux;

    if (lux > MAX_LIGHT_LUX)
    {
        lux = MAX_LIGHT_LUX;
    }

    int ray_length =
    SUN_RAY_MIN_LENGTH +
    (lux * (SUN_RAY_MAX_LENGTH - SUN_RAY_MIN_LENGTH))
    / MAX_LIGHT_LUX;
    
    display_ui_draw_sun_rays(
        x,
        y,
        ray_length,
        UI_COLOR_SUN_RAY);

    display_fill_circle(
        x,
        y,
        SUN_RADIUS,
        UI_COLOR_SUN);

    display_draw_circle(
        x,
        y,
        SUN_RADIUS,
        UI_COLOR_SUN_RAY);

    display_ui_draw_data_percentage(
        x,
        y + SUN_RAY_MAX_LENGTH + SUN_RADIUS,
        (lux * 100) / MAX_LIGHT_LUX);

    display_printf(
        x - 25,
        y + SUN_RAY_MAX_LENGTH + SUN_RADIUS + 18,
        &display_font_5x7,
        UI_COLOR_TEXT,
        UI_COLOR_BACKGROUND,
        DISPLAY_BACKGROUND_TRANSPARENT,
        "Lux: %d",lux);
}

