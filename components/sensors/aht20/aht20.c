#include "aht20.h"
#include "esp_err.h"
#include "i2c_bus.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "aht20";

static const uint8_t AHT20_I2C_ADDRESS = 0x38;

// Commands
static const uint8_t AHT20_CMD_STATUS       = 0x71;
static const uint8_t AHT20_CMD_INIT         = 0xBE;
static const uint8_t AHT20_CMD_TRIGGER      = 0xAC;
static const uint8_t AHT20_CMD_SOFT_RESET   = 0xBA;

// Command parameters
static const uint8_t AHT20_INIT_PARAM1      = 0x08;
static const uint8_t AHT20_INIT_PARAM2      = 0x00;

static const uint8_t AHT20_TRIGGER_PARAM1   = 0x33;
static const uint8_t AHT20_TRIGGER_PARAM2   = 0x00;

// Status register bits
static const uint8_t AHT20_STATUS_BUSY_MASK = (1 << 7);
static const uint8_t AHT20_STATUS_CAL_MASK  = (1 << 3);

// Timing
static const uint32_t AHT20_POWER_ON_DELAY_MS    = 20;
static const uint32_t AHT20_SOFT_RESET_DELAY_MS  = 20;
static const uint32_t AHT20_INIT_DELAY_MS        = 10;
static const uint32_t AHT20_MEASUREMENT_DELAY_MS = 80;

static const int AHT20_I2C_TIMEOUT_MS = -1;
static const uint32_t AHT20_I2C_SPEED_HZ = 100000;
static const uint8_t AHT20_RAW_DATA_LENGTH = 7;

static const uint8_t AHT20_CRC_INIT = 0xFF;
static const uint8_t AHT20_CRC_POLYNOMIAL = 0x31;
static const uint8_t AHT20_CRC_DATA_LENGTH = 6;

static const float AHT20_RAW_SCALE = 1048576.0f;

static i2c_master_dev_handle_t dev_handle = NULL;


static esp_err_t aht20_initialize(void);

static esp_err_t aht20_trigger_measurement(void);

static esp_err_t aht20_read_raw(uint8_t *raw_data);

static esp_err_t aht20_convert_data(const uint8_t *raw_data, aht20_data_t *data);

static esp_err_t aht20_soft_reset(void);

static bool aht20_verify_crc(const uint8_t *raw_data);

esp_err_t aht20_initialize(void)
{
    uint8_t status_cmd = AHT20_CMD_STATUS;
    uint8_t status = 0;

    // Read the sensor status register.
    esp_err_t ret = i2c_master_transmit(
        dev_handle,
        &status_cmd,
        1,
        AHT20_I2C_TIMEOUT_MS
    );
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send status command");
        return ret;
    }

    ret = i2c_master_receive(
        dev_handle,
        &status,
        1,
        AHT20_I2C_TIMEOUT_MS
    );
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read sensor status");
        return ret;
    }

    // If the calibration bit is already set, the sensor is ready to use.
    if (status & AHT20_STATUS_CAL_MASK)
    {
        ESP_LOGI(TAG, "Sensor already calibrated");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Sensor not calibrated, sending initialization command");

    uint8_t init_cmd[3] =
    {
        AHT20_CMD_INIT,
        AHT20_INIT_PARAM1,
        AHT20_INIT_PARAM2
    };

    ret = i2c_master_transmit(
        dev_handle,
        init_cmd,
        sizeof(init_cmd),
        AHT20_I2C_TIMEOUT_MS
    );
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send initialization command");
        return ret;
    }

    // Wait for the sensor to complete the initialization sequence.
    vTaskDelay(pdMS_TO_TICKS(AHT20_INIT_DELAY_MS));

    // Read the status register again to verify that initialization succeeded.
    ret = i2c_master_transmit(
        dev_handle,
        &status_cmd,
        1,
        AHT20_I2C_TIMEOUT_MS
    );
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send status command");
        return ret;
    }

    ret = i2c_master_receive(
        dev_handle,
        &status,
        1,
        AHT20_I2C_TIMEOUT_MS
    );
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read sensor status");
        return ret;
    }

    if (status & AHT20_STATUS_CAL_MASK)
    {
        ESP_LOGI(TAG, "Sensor initialized successfully");
        return ESP_OK;
    }

    ESP_LOGE(TAG, "Sensor initialization failed");
    return ESP_FAIL;
}

esp_err_t aht20_init(i2c_master_bus_handle_t bus_handle)
{
    // Validate the input I2C bus handle.
    if (bus_handle == NULL)
    {
        ESP_LOGE(TAG, "Invalid I2C bus handle");
        return ESP_ERR_INVALID_ARG;
    }

    // Prevent multiple initializations of the driver.
    if (dev_handle != NULL)
    {
        ESP_LOGW(TAG, "AHT20 is already initialized");
        return ESP_OK;
    }

    // Configure the AHT20 device on the I2C bus.
    i2c_device_config_t dev_config =
    {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = AHT20_I2C_ADDRESS,
        .scl_speed_hz = AHT20_I2C_SPEED_HZ,
    };

    // Add the sensor as an I2C device.
    esp_err_t ret = i2c_bus_add_device(
        bus_handle,
        &dev_config,
        &dev_handle
    );

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add AHT20 device to the I2C bus");
        return ret;
    }

    // Wait for the sensor to complete its power-on sequence.
    vTaskDelay(pdMS_TO_TICKS(AHT20_POWER_ON_DELAY_MS));

    // Verify that the sensor is ready for normal operation.
    ret = aht20_initialize();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize AHT20");

        // Remove the device from the bus to leave the driver in a clean state.
        esp_err_t remove_ret = i2c_bus_remove_device(&dev_handle);
        if (remove_ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to remove AHT20 device from the I2C bus");
        }

        return ret;
    }

    ESP_LOGI(TAG, "AHT20 initialized successfully");

    return ESP_OK;
}

esp_err_t aht20_deinit(void)
{
    if (dev_handle == NULL)
    {
        return ESP_OK;
    }

    // Remove the device from the I2C bus.
    esp_err_t ret = i2c_bus_remove_device(&dev_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to remove AHT20 device from the I2C bus");
        return ret;
    }

    ESP_LOGI(TAG, "AHT20 deinitialized successfully");

    return ESP_OK;
}

esp_err_t aht20_trigger_measurement(void)
{
    uint8_t status = 0;

    // Send the measurement trigger command.
    uint8_t trigger_cmd[3] =
    {
        AHT20_CMD_TRIGGER,
        AHT20_TRIGGER_PARAM1,
        AHT20_TRIGGER_PARAM2
    };

    esp_err_t ret = i2c_master_transmit(
        dev_handle,
        trigger_cmd,
        sizeof(trigger_cmd),
        AHT20_I2C_TIMEOUT_MS
    );

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send measurement trigger command");
        return ret;
    }

    // Allow the sensor enough time to complete the measurement.
    vTaskDelay(pdMS_TO_TICKS(AHT20_MEASUREMENT_DELAY_MS));

    ret = i2c_master_receive(
        dev_handle,
        &status,
        1,
        AHT20_I2C_TIMEOUT_MS
    );

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read sensor status");
        return ret;
    }

    // Check whether the measurement is still in progress.
    if (status & AHT20_STATUS_BUSY_MASK)
    {
        ESP_LOGE(TAG, "Sensor is still busy after measurement delay");
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}

esp_err_t aht20_read_raw(uint8_t *raw_data)
{
    // Validate the output buffer.
    if (raw_data == NULL)
    {
        ESP_LOGE(TAG, "Invalid raw data buffer");
        return ESP_ERR_INVALID_ARG;
    }

    // Read the raw measurement data from the sensor.
    esp_err_t ret = i2c_master_receive(
        dev_handle,
        raw_data,
        AHT20_RAW_DATA_LENGTH,
        AHT20_I2C_TIMEOUT_MS
    );

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read raw measurement data");
        return ret;
    }

    return ESP_OK;
}

static bool aht20_verify_crc(const uint8_t *raw_data)
{
    if (raw_data == NULL)
    {
        return false;
    }

    uint8_t crc = AHT20_CRC_INIT;

    // Process the first 6 bytes.
    for (int i = 0; i < AHT20_CRC_DATA_LENGTH; i++)
    {
        crc ^= raw_data[i];

        // Process each bit.
        for (int bit = 0; bit < 8; bit++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ AHT20_CRC_POLYNOMIAL;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return (crc == raw_data[6]);
}

static esp_err_t aht20_convert_data(const uint8_t *raw_data, aht20_data_t *data)
{
    // Validate the input parameters.
    if (raw_data == NULL || data == NULL)
    {
        ESP_LOGE(TAG, "Invalid input parameters for data conversion");
        return ESP_ERR_INVALID_ARG;
    }

    if (raw_data[0] & AHT20_STATUS_BUSY_MASK)
    {
        ESP_LOGE(TAG, "Sensor is busy, cannot convert data");
        return ESP_ERR_INVALID_STATE;
    }
    if (!aht20_verify_crc(raw_data))
    {
        ESP_LOGE(TAG, "CRC verification failed");
        return ESP_FAIL;
    }

// Extract the raw humidity value.
uint32_t raw_humidity =
    ((uint32_t)raw_data[1] << 12) |
    ((uint32_t)raw_data[2] << 4) |
    ((uint32_t)(raw_data[3] >> 4));

// Extract the raw temperature value.
uint32_t raw_temperature =
    ((uint32_t)(raw_data[3] & 0x0F) << 16) |
    ((uint32_t)raw_data[4] << 8) |
    ((uint32_t)raw_data[5]);

    // Convert the raw values to physical units.
    data->humidity = (float)raw_humidity * 100.0f / AHT20_RAW_SCALE;
    data->temperature = (float)raw_temperature * 200.0f / AHT20_RAW_SCALE - 50.0f;

    return ESP_OK;
}

esp_err_t aht20_read(aht20_data_t *data)
{

    if (dev_handle == NULL)
    {
        ESP_LOGE(TAG, "Sensor is not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Validate the output structure.
    if (data == NULL)
    {
        ESP_LOGE(TAG, "Invalid data pointer");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t raw_data[AHT20_RAW_DATA_LENGTH];

    // Trigger a new measurement.
    esp_err_t ret = aht20_trigger_measurement();
    if (ret != ESP_OK)
    {
        return ret;
    }

    // Read the raw measurement bytes.
    ret = aht20_read_raw(raw_data);
    if (ret != ESP_OK)
    {
        return ret;
    }

    // Convert the raw measurement into physical values.
    ret = aht20_convert_data(raw_data, data);
    if (ret != ESP_OK)
    {
        return ret;
    }

    return ESP_OK;
}

static esp_err_t aht20_soft_reset(void)
{
    if (dev_handle == NULL)
    {
        ESP_LOGE(TAG, "Sensor is not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t reset_cmd = AHT20_CMD_SOFT_RESET;

    // Send the soft reset command to the sensor.
    esp_err_t ret = i2c_master_transmit(
        dev_handle,
        &reset_cmd,
        1,
        AHT20_I2C_TIMEOUT_MS
    );
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send soft reset command");
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(AHT20_SOFT_RESET_DELAY_MS));

    ret = aht20_initialize();
    if (ret != ESP_OK)
    {
        return ret;
    }

    ESP_LOGI(TAG, "AHT20 reset successfully");

    return ESP_OK;
}