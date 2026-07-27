#ifndef AHT20_H
#define AHT20_H

#include "esp_err.h"
#include "driver/i2c_master.h"

typedef struct {
    float temperature;
    float humidity;
} aht20_data_t;

esp_err_t aht20_init(i2c_master_bus_handle_t bus_handle);

esp_err_t aht20_deinit(void);

esp_err_t aht20_read(aht20_data_t *data);

#endif // AHT20_H