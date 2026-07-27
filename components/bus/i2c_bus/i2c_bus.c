#include "i2c_bus.h"

#include "esp_log.h"

static const char *TAG = "i2c_bus";

esp_err_t i2c_bus_init(const i2c_bus_config_t *config, i2c_master_bus_handle_t *bus_handle)
{
    if (config == NULL || bus_handle == NULL) {
        ESP_LOGE(TAG, "Invalid bus handle");
        return ESP_ERR_INVALID_ARG;
    }
    
    if (config->glitch_ignore_cnt > 15) {
        ESP_LOGE(TAG, "Invalid glitch_ignore_cnt");
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_bus_config_t bus_config = {
        .i2c_port = config->port,
        .sda_io_num = config->sda,
        .scl_io_num = config->scl,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = config->glitch_ignore_cnt,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags = {
            .enable_internal_pullup = config->enable_internal_pullup,
            .allow_pd = false,
        },
    };

    esp_err_t err = i2c_new_master_bus(&bus_config, bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C bus: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "I2C bus %d initialized (SDA=%d, SCL=%d)", config->port, config->sda, config->scl);

    return ESP_OK;
}

esp_err_t i2c_bus_deinit(i2c_master_bus_handle_t *bus_handle)
{
    if (bus_handle == NULL || *bus_handle == NULL) {
        ESP_LOGE(TAG, "Invalid bus handle");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = i2c_del_master_bus(*bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to deinitialize I2C bus: %s", esp_err_to_name(err));
        return err;
    }

    *bus_handle = NULL;

    ESP_LOGI(TAG, "I2C bus deinitialized");

    return ESP_OK;
}

esp_err_t i2c_bus_add_device(i2c_master_bus_handle_t bus_handle, const i2c_device_config_t *dev_config, i2c_master_dev_handle_t *dev_handle)
{
    if (bus_handle == NULL || dev_config == NULL || dev_handle == NULL) {
        ESP_LOGE(TAG, "Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = i2c_master_bus_add_device(
        bus_handle,
        dev_config,
        dev_handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add I2C device (0x%02X): %s", dev_config->device_address, esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "I2C device added (0x%02X, %lu Hz)", dev_config->device_address, dev_config->scl_speed_hz);

    return ESP_OK;
}

esp_err_t i2c_bus_remove_device(
    i2c_master_dev_handle_t *dev_handle)
{
    if (dev_handle == NULL || *dev_handle == NULL) {
        ESP_LOGE(TAG, "Invalid device handle");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = i2c_master_bus_rm_device(*dev_handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to remove I2C device: %s", esp_err_to_name(err));
        return err;
    }

    *dev_handle = NULL;

    ESP_LOGI(TAG, "I2C device removed");

    return ESP_OK;
}