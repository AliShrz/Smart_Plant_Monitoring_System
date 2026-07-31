/* TODO:

Public API
──────────
bh1750_init()                  ☐
bh1750_read()                  ☐
bh1750_deinit()                ☐

Private Sensor Functions
────────────────────────
bh1750_initialize()            x
bh1750_power_on()              x
bh1750_power_down()            x
bh1750_reset()                 x
bh1750_start_measurement()     x
bh1750_read_raw()              x
bh1750_convert_to_lux()        ☐

Private I2C Access
──────────────────
bh1750_write_command()         x
bh1750_read_data()             x
*/

#include "bh1750.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// I2C Configuration
#define BH1750_I2C_ADDRESS      0x23    // if ADDR pin is connected to GND, otherwise 0x5C if connected to VCC
#define BH1750_I2C_SPEED_HZ     100000

// Commands
#define BH1750_CMD_POWER_DOWN               0x00
#define BH1750_CMD_POWER_ON                 0x01
#define BH1750_CMD_RESET                    0x07

#define BH1750_CMD_CONTINUOUS_HIGH_RES      0x10
#define BH1750_CMD_CONTINUOUS_HIGH_RES2     0x11
#define BH1750_CMD_CONTINUOUS_LOW_RES       0x13

#define BH1750_CMD_ONE_TIME_HIGH_RES        0x20
#define BH1750_CMD_ONE_TIME_HIGH_RES2       0x21
#define BH1750_CMD_ONE_TIME_LOW_RES         0x23

// Timings
#define BH1750_HIGH_RES_MEASUREMENT_TIME_MS     180
#define BH1750_LOW_RES_MEASUREMENT_TIME_MS       24

// Conversion
#define BH1750_LUX_CONVERSION_FACTOR        1.2f

// constants
static const int BH1750_I2C_TIMEOUT_MS = -1;

static const char *TAG = "bh1750fvi";

static esp_err_t bh1750_write_command(uint8_t command);
static esp_err_t bh1750_read_data(uint16_t *data);
static esp_err_t bh1750_power_on(void);
static esp_err_t bh1750_power_down(void);
static esp_err_t bh1750_reset(void);
static esp_err_t bh1750_start_measurement(void);
static esp_err_t bh1750_read_raw(uint16_t *raw_data);
static esp_err_t bh1750_initialize(void);
static esp_err_t bh1750_convert_to_lux(uint16_t raw_data, float *lux);

// Private I2C Access
static esp_err_t bh1750_write_command(uint8_t command)
{
    return i2c_master_transmit(
        bh1750.device_handle,
        &command,
        sizeof(command),
        BH1750_I2C_TIMEOUT_MS
    );
}

static esp_err_t bh1750_read_data(uint16_t *data)
{
    if (data == NULL)
    {
        ESP_LOGE(TAG, "Invalid data pointer");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t buffer[2];
    esp_err_t ret = i2c_master_receive(
        bh1750.device_handle,
        buffer,
        sizeof(buffer),
        BH1750_I2C_TIMEOUT_MS
    );

    if (ret != ESP_OK)
    {
        return ret;
    }

    *data = ((uint16_t)buffer[0] << 8) | buffer[1];
    return ESP_OK;
}

/*********** Private Sensor Functions ************/

static esp_err_t bh1750_power_on(void)
{
    return bh1750_write_command(BH1750_CMD_POWER_ON);
}

static esp_err_t bh1750_power_down(void)
{
    return bh1750_write_command(BH1750_CMD_POWER_DOWN);
}

static esp_err_t bh1750_reset(void)
{
    ESP_RETURN_ON_ERROR(
        bh1750_power_on(),
        TAG,
        "Failed to power on BH1750");

    return bh1750_write_command(BH1750_CMD_RESET);
}

static esp_err_t bh1750_start_measurement(void)
{
    return bh1750_write_command(BH1750_CMD_ONE_TIME_HIGH_RES);
}

static esp_err_t bh1750_read_raw(uint16_t *raw_data)
{
    if (raw_data == NULL)
    {
        ESP_LOGE(TAG, "Invalid raw data pointer");
        return ESP_ERR_INVALID_ARG;
    }

    return bh1750_read_data(raw_data);
}

static esp_err_t bh1750_initialize(void)
{
    ESP_RETURN_ON_ERROR(
        bh1750_reset(),
        TAG,
        "Failed to reset BH1750");

    return ESP_OK;
}

static esp_err_t bh1750_convert_to_lux(uint16_t raw_data, float *lux)
{
    if (lux == NULL)
    {
        ESP_LOGE(TAG, "Invalid lux pointer");
        return ESP_ERR_INVALID_ARG;
    }

    *lux = (float)raw_data / BH1750_LUX_CONVERSION_FACTOR;

    return ESP_OK;
}

/*********** Public API ************/

esp_err_t bh1750_init(i2c_master_bus_handle_t bus_handle)
{
    if (bh1750.device_handle != NULL)
    {
        ESP_LOGW(TAG, "BH1750 is already initialized");
        return ESP_ERR_INVALID_STATE;
    }

    i2c_device_config_t dev_config = {
        .device_address = BH1750_I2C_ADDRESS,
        .scl_speed_hz = BH1750_I2C_SPEED_HZ,
    };

    esp_err_t ret = i2c_master_bus_add_device(
        bus_handle,
        &dev_config,
        &bh1750.device_handle);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add BH1750 device to the I2C bus");
        return ret;
    }

    ret = bh1750_initialize();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize BH1750");

        (void)i2c_master_bus_rm_device(bh1750.device_handle);
        bh1750.device_handle = NULL;

        return ret;
    }

    ESP_LOGI(TAG, "BH1750 initialized successfully");

    return ESP_OK;
}

esp_err_t bh1750_read(bh1750_data_t *data)
{
    if (data == NULL)
    {
        ESP_LOGE(TAG, "Invalid data pointer");
        return ESP_ERR_INVALID_ARG;
    }

    if (bh1750.device_handle == NULL)
    {
        ESP_LOGE(TAG, "BH1750 is not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_ERROR(
        bh1750_power_on(),
        TAG,
        "Failed to power on BH1750");

    ESP_RETURN_ON_ERROR(
        bh1750_start_measurement(),
        TAG,
        "Failed to start measurement");

    vTaskDelay(pdMS_TO_TICKS(BH1750_HIGH_RES_MEASUREMENT_TIME_MS));

    uint16_t raw_data;

    ESP_RETURN_ON_ERROR(
        bh1750_read_raw(&raw_data),
        TAG,
        "Failed to read raw data");

    ESP_RETURN_ON_ERROR(
        bh1750_convert_to_lux(raw_data, &data->lux),
        TAG,
        "Failed to convert raw data to lux");

    return ESP_OK;
}

esp_err_t bh1750_deinit(void)
{
    if (bh1750.device_handle == NULL)
    {
        ESP_LOGW(TAG, "BH1750 is not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Ignore the return value since the device may already be powered down.
    (void)bh1750_power_down();

    esp_err_t ret = i2c_master_bus_rm_device(bh1750.device_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to remove BH1750 device from the I2C bus");
        return ret;
    }

    bh1750.device_handle = NULL;

    ESP_LOGI(TAG, "BH1750 deinitialized successfully");

    return ESP_OK;
}