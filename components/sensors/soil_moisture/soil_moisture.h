#pragma once
#include "esp_err.h"

#include "esp_adc/adc_oneshot.h"

typedef struct
{
    int raw_value;
    float moisture_percentage;
} soil_moisture_data_t;

typedef struct
{
    int raw_dry;
    int raw_wet;
} soil_moisture_calibration_t;

typedef struct
{
    adc_oneshot_unit_handle_t adc_handle;
    soil_moisture_calibration_t calibration;
    bool initialized;
} soil_moisture_t;

static soil_moisture_t soil = {0};

esp_err_t soil_moisture_init(void);

esp_err_t soil_moisture_read(soil_moisture_data_t *data);

esp_err_t soil_moisture_deinit(void);

// esp_err_t soil_moisture_set_calibration(int raw_dry, int raw_wet);

// esp_err_t soil_moisture_get_calibration(soil_moisture_calibration_t *calibration);

// esp_err_t soil_moisture_reset_calibration(void);