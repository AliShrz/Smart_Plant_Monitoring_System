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

/*==========================
 * Thermometer Widget
 *==========================*/

#define THERMOMETER_STEM_WIDTH   6
#define THERMOMETER_STEM_HEIGHT  20
#define THERMOMETER_BULB_RADIUS  7

#define THERMOMETER_MIN_TEMP       0
#define THERMOMETER_MAX_TEMP       70

/*==========================
 * Humidity Widget
 *==========================*/
#define HUMIDITY_DROP_RADIUS       7
#define HUMIDITY_DROP_HEIGHT       10


static void display_ui_draw_sun_rays(
    int x,
    int y,
    int ray_length,
    uint16_t color);

static void display_ui_draw_thermometer(
    int x,
    int y);

static void display_ui_draw_data_percentage(
    int center_x,
    int bottom_y,
    uint8_t data_percentage);

static void display_ui_draw_humidity_drop(
    int x,
    int y);

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

/**************** Thermometer ****************/

static void display_ui_draw_thermometer_fill(
    int x,
    int y,
    int temperature)
{
    if (temperature < THERMOMETER_MIN_TEMP)
    {
        temperature = THERMOMETER_MIN_TEMP;
    }

    if (temperature > THERMOMETER_MAX_TEMP)
    {
        temperature = THERMOMETER_MAX_TEMP;
    }

    /* Bulb */

    display_fill_circle(
        x,
        y,
        THERMOMETER_BULB_RADIUS - 3,
        UI_COLOR_TEMPERATURE);

    /* Stem fill height */

    int fill_height =
        ((temperature - THERMOMETER_MIN_TEMP + 10) *
         THERMOMETER_STEM_HEIGHT) /
        (THERMOMETER_MAX_TEMP - THERMOMETER_MIN_TEMP);
    
    int fill_width = THERMOMETER_STEM_WIDTH - 4;   // 2 for border and 2 for gap

    if (fill_height > 0)
    {
        display_fill_rect(
            x - fill_width / 2,
            y - fill_height,
            x + fill_width / 2,
            y,
            UI_COLOR_TEMPERATURE);
    }
}


static void display_ui_draw_thermometer(
    int x,
    int y)
{
    /* Stem */

    display_fill_rect(
        x - THERMOMETER_STEM_WIDTH / 2,
        y - THERMOMETER_STEM_HEIGHT - (THERMOMETER_BULB_RADIUS / 2),
        x + (THERMOMETER_STEM_WIDTH + 1) / 2,
        y,
        COLOR_WHITE);

    display_draw_rect(
        x - THERMOMETER_STEM_WIDTH / 2,
        y - THERMOMETER_STEM_HEIGHT - (THERMOMETER_BULB_RADIUS / 2),
        x + (THERMOMETER_STEM_WIDTH + 1) / 2,
        y,
        UI_COLOR_BORDER);

    /* Bulb */

    display_fill_circle(
        x,
        y,
        THERMOMETER_BULB_RADIUS,
        COLOR_WHITE);

    display_draw_circle(
        x,
        y,
        THERMOMETER_BULB_RADIUS,
        UI_COLOR_BORDER);
}

void display_ui_draw_temperature(
    int x,
    int y,
    const display_ui_data_t *data)
{
    display_ui_draw_thermometer(x, y);

    display_ui_draw_thermometer_fill(
        x,
        y,
        data->temperature_c);

    display_printf(
        x + 13,
        y + THERMOMETER_BULB_RADIUS - 20,
        &display_font_5x7,
        UI_COLOR_TEXT,
        UI_COLOR_BACKGROUND,
        DISPLAY_BACKGROUND_TRANSPARENT,
        "Temp:");
    display_printf(
        x + 13,
        y + THERMOMETER_BULB_RADIUS - 10,
        &display_font_5x7,
        UI_COLOR_TEXT,
        UI_COLOR_BACKGROUND,
        DISPLAY_BACKGROUND_TRANSPARENT,
        "%.1f C",
        data->temperature_c);
}

/**************** Humidity ****************/

static void display_ui_draw_humidity_drop(
    int x,
    int y)
{
    /* Drop geometry */

    int circle_radius = HUMIDITY_DROP_RADIUS;

    int triangle_base_y =
        y - circle_radius + 2;

    int triangle_tip_y =
        triangle_base_y - HUMIDITY_DROP_HEIGHT;

    /*==========================
     * Outline
     *==========================*/

    display_draw_circle(
        x,
        y,
        circle_radius,
        UI_COLOR_BORDER);

    display_draw_triangle(
        x - circle_radius + 2,
        triangle_base_y,
        x + circle_radius - 2,
        triangle_base_y,
        x,
        triangle_tip_y,
        UI_COLOR_BORDER);

    /*==========================
     * Inner fill
     *==========================*/

    display_fill_circle(
        x + 1,
        y,
        circle_radius - 3,
        COLOR_BLUE);

    display_fill_triangle(
        x - circle_radius + 3,
        triangle_base_y + 1,
        x + circle_radius - 2,
        triangle_base_y + 1,
        x,
        triangle_tip_y + 2,
        COLOR_WHITE);
}

void display_ui_draw_humidity(
    int x,
    int y,
    const display_ui_data_t *data)
{
    display_ui_draw_humidity_drop(x, y);

    display_printf(
        x + HUMIDITY_DROP_RADIUS + 5,
        y - HUMIDITY_DROP_RADIUS - 6,
        &display_font_5x7,
        UI_COLOR_TEXT,
        UI_COLOR_BACKGROUND,
        DISPLAY_BACKGROUND_TRANSPARENT,
        "H: ");

    display_printf(
        x + HUMIDITY_DROP_RADIUS + 5,
        y - HUMIDITY_DROP_RADIUS + 4,
        &display_font_5x7,
        UI_COLOR_TEXT,
        UI_COLOR_BACKGROUND,
        DISPLAY_BACKGROUND_TRANSPARENT,
        "%.1f%%",
        data->humidity_percent);
}