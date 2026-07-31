#include "driver/i2c_master.h"
#include "stdint.h"
#include "esp_err.h"

typedef struct
{
    i2c_master_dev_handle_t device_handle;
} bh1750_t;

static bh1750_t bh1750 = {0};

typedef struct
{
    float lux;
} bh1750_data_t;

esp_err_t bh1750_init(i2c_master_bus_handle_t bus_handle);
esp_err_t bh1750_read(bh1750_data_t *data);
esp_err_t bh1750_deinit(void);