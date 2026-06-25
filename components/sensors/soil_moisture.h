#pragma once
#include "esp_err.h"

esp_err_t soil_moisture_init(void);

int soil_moisture_read_raw(void);