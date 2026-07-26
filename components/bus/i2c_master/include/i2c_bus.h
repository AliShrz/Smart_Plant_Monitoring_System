#ifndef I2C_BUS_H
#define I2C_BUS_H

#include "driver/i2c_master.h"
#include "esp_err.h"

typedef struct {
    i2c_port_num_t port;
    gpio_num_t sda;
    gpio_num_t scl;
    bool enable_internal_pullup;
    uint8_t glitch_ignore_cnt;
} i2c_bus_config_t;

esp_err_t i2c_bus_init(const i2c_bus_config_t *config, i2c_master_bus_handle_t *bus_handle);

esp_err_t i2c_bus_deinit(i2c_master_bus_handle_t *bus_handle);

esp_err_t i2c_bus_add_device(i2c_master_bus_handle_t bus_handle, const i2c_device_config_t *dev_config, i2c_master_dev_handle_t *dev_handle );

esp_err_t i2c_bus_remove_device(i2c_master_dev_handle_t *dev_handle);

#endif // I2C_BUS_H