
/* to do: 
Public API
──────────
bmp280_init()               done
bmp280_read()               done
bmp280_deinit()             done

Private Sensor Functions
────────────────────────
bmp280_initialize()         done
bmp280_read_chip_id()       done
bmp280_reset()              done
bmp280_configure()          done
bmp280_read_calibration()   done
bmp280_read_raw()           done
bmp280_compensate()         done

Private Register Access
───────────────────────
bmp280_read_register()      done
bmp280_write_register()     done

*/
#include "bmp280.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

static const char *TAG = "bmp280";

// Calibration registers
static const uint8_t BMP280_REG_CALIB_START = 0x88;

// Identification and configuration registers
static const uint8_t BMP280_REG_ID          = 0xD0;
static const uint8_t BMP280_REG_RESET       = 0xE0;
static const uint8_t BMP280_REG_STATUS      = 0xF3;
static const uint8_t BMP280_REG_CTRL_MEAS   = 0xF4;
static const uint8_t BMP280_REG_CONFIG      = 0xF5;

// Measurement registers
static const uint8_t BMP280_REG_PRESS_MSB   = 0xF7;
// static const uint8_t BMP280_REG_TEMP_MSB    = 0xFA;

// Constants:
static const uint8_t BMP280_CHIP_ID = 0x58;
static const int BMP280_I2C_TIMEOUT_MS = -1;

static const uint8_t BMP280_RESET_VALUE = 0xB6;
// static const uint32_t BMP280_RESET_DELAY_MS = 2;

// static const uint8_t BMP280_I2C_ADDRESS_0 = 0x76;
// static const uint8_t BMP280_I2C_ADDRESS_1 = 0x77;

#define BMP280_I2C_ADDRESS      0x77
#define BMP280_I2C_SPEED_HZ     100000

#define BMP280_STATUS_MEASURING_BIT   BIT3
#define BMP280_STATUS_IM_UPDATE_BIT   BIT0

#define BMP280_RESET_TIMEOUT_MS      10
#define BMP280_STATUS_POLL_DELAY_MS   1
#define BMP280_MEASUREMENT_TIMEOUT_MS    20

#define BMP280_CALIB_DATA_LEN    24
#define BMP280_CALIB_DIG_T1_OFFSET  0
#define BMP280_CALIB_DIG_T2_OFFSET  2
#define BMP280_CALIB_DIG_T3_OFFSET  4
#define BMP280_CALIB_DIG_P1_OFFSET  6
#define BMP280_CALIB_DIG_P2_OFFSET  8
#define BMP280_CALIB_DIG_P3_OFFSET  10
#define BMP280_CALIB_DIG_P4_OFFSET  12
#define BMP280_CALIB_DIG_P5_OFFSET  14
#define BMP280_CALIB_DIG_P6_OFFSET  16
#define BMP280_CALIB_DIG_P7_OFFSET  18
#define BMP280_CALIB_DIG_P8_OFFSET  20
#define BMP280_CALIB_DIG_P9_OFFSET  22

// Calibration offsets macros
#define BMP280_OSRS_SKIPPED    0x00
#define BMP280_OSRS_X1         0x01
#define BMP280_OSRS_X2         0x02
#define BMP280_OSRS_X4         0x03
#define BMP280_OSRS_X8         0x04
#define BMP280_OSRS_X16        0x05

// Raw data offsets
#define BMP280_RAW_PRESS_MSB_OFFSET     0
#define BMP280_RAW_PRESS_LSB_OFFSET     1
#define BMP280_RAW_PRESS_XLSB_OFFSET    2

#define BMP280_RAW_TEMP_MSB_OFFSET      3
#define BMP280_RAW_TEMP_LSB_OFFSET      4
#define BMP280_RAW_TEMP_XLSB_OFFSET     5

// Calibration modes macros
#define BMP280_MODE_SLEEP      0x00
#define BMP280_MODE_FORCED     0x01
#define BMP280_MODE_NORMAL     0x03

#define BMP280_FILTER_OFF      0x00
#define BMP280_SPI3W_DISABLE   0x00
#define BMP280_T_SB_0_5_MS     0x00

#define BMP280_CONFIG_T_SB_POS      5
#define BMP280_CONFIG_FILTER_POS    2
#define BMP280_CONFIG_SPI3W_POS     0

#define BMP280_CTRL_MEAS_OSRS_T_POS    5
#define BMP280_CTRL_MEAS_OSRS_P_POS    2
#define BMP280_CTRL_MEAS_MODE_POS      0

// Raw data length
#define BMP280_RAW_DATA_LEN    6



// Helpers:
static esp_err_t bmp280_read_register(uint8_t reg_addr, uint8_t *data, size_t len);
static esp_err_t bmp280_write_register(uint8_t reg_addr, uint8_t value);
static uint16_t bmp280_u16(const uint8_t *data);
static int16_t bmp280_s16(const uint8_t *data);

static esp_err_t bmp280_read_status(bmp280_status_t *status);
static esp_err_t bmp280_read_chip_id(void);
static esp_err_t bmp280_read_calibration(void);
static esp_err_t bmp280_configure(void);
static esp_err_t bmp280_read_raw(bmp280_raw_data_t *raw_data);
static void bmp280_compensate(const bmp280_raw_data_t *raw_data, bmp280_data_t *data);




static esp_err_t bmp280_reset(void);
static esp_err_t bmp280_initialize(void);



// Private sensor functions
static esp_err_t bmp280_read_register(uint8_t reg_addr, uint8_t *data, size_t len)
{
    if (data == NULL) 
    {
        ESP_LOGE(TAG, "Invalid data pointer");
        return ESP_ERR_INVALID_ARG;
    }

    if (len == 0) 
    {
        ESP_LOGE(TAG, "Invalid data length");
        return ESP_ERR_INVALID_ARG;
    }

    return i2c_master_transmit_receive(
        bmp280.device_handle,
        &reg_addr,
        sizeof(reg_addr),
        data,
        len,
        BMP280_I2C_TIMEOUT_MS
    );

}

static esp_err_t bmp280_write_register(uint8_t reg_addr, uint8_t value)
{
    uint8_t tx_buffer[2] = {reg_addr, value};

    return i2c_master_transmit(
        bmp280.device_handle,
        tx_buffer,
        sizeof(tx_buffer),
        BMP280_I2C_TIMEOUT_MS
    );
}

static esp_err_t bmp280_read_status(bmp280_status_t *status)
{
    if (status == NULL)
    {
        ESP_LOGE(TAG, "Invalid status pointer");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t status_byte;

    ESP_RETURN_ON_ERROR(
        bmp280_read_register(BMP280_REG_STATUS, &status_byte, sizeof(status_byte)),
        TAG,
        "Failed to read status register");

    status->measuring = (status_byte & BMP280_STATUS_MEASURING_BIT) != 0;
    status->im_update = (status_byte & BMP280_STATUS_IM_UPDATE_BIT) != 0;

    return ESP_OK;
}

static esp_err_t bmp280_read_chip_id(void)
{
    uint8_t chip_id;

    ESP_RETURN_ON_ERROR(
        bmp280_read_register(BMP280_REG_ID, &chip_id, sizeof(chip_id)),
        TAG,
        "Failed to read chip ID");

    if (chip_id != BMP280_CHIP_ID)
    {
        ESP_LOGE(TAG,
                "Unexpected chip ID: 0x%02X (expected 0x%02X)",
                chip_id,
                BMP280_CHIP_ID);

        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

static esp_err_t bmp280_reset(void)
{
    bmp280_status_t status;

    ESP_RETURN_ON_ERROR(
        bmp280_write_register(BMP280_REG_RESET, BMP280_RESET_VALUE),
        TAG,
        "Failed to reset BMP280");

    for (uint32_t elapsed = 0;
        elapsed < BMP280_RESET_TIMEOUT_MS;
        elapsed += BMP280_STATUS_POLL_DELAY_MS)
    {
        ESP_RETURN_ON_ERROR(
            bmp280_read_status(&status),
            TAG,
            "Failed to read sensor status");

        if (!status.im_update)
        {
            return ESP_OK;
        }

        if (status.im_update)
        {
            vTaskDelay(pdMS_TO_TICKS(BMP280_STATUS_POLL_DELAY_MS));
            continue;
        }
    }

    ESP_LOGE(TAG, "BMP280 reset timed out");
    return ESP_ERR_TIMEOUT;
}

/**
 * @brief Convert two little-endian bytes to an unsigned 16-bit value.
 *
 * @param data Pointer to the least significant byte.
 *
 * @return Decoded unsigned 16-bit value.
 */
static uint16_t bmp280_u16(const uint8_t *data)
{
    return  (uint16_t)data[0] |
            ((uint16_t)data[1] << 8);
}
/**
 * @brief Convert two little-endian bytes to a signed 16-bit value.
 *
 * @param data Pointer to the least significant byte.
 *
 * @return Decoded signed 16-bit value.
 */
static int16_t bmp280_s16(const uint8_t *data)
{
    return (int16_t)(
        (uint16_t)data[0] |
        ((uint16_t)data[1] << 8));
}

static esp_err_t bmp280_read_calibration(void)
{
    uint8_t calib_coef[BMP280_CALIB_DATA_LEN];

    ESP_RETURN_ON_ERROR(
        bmp280_read_register(BMP280_REG_CALIB_START,
                            calib_coef,
                            sizeof(calib_coef)),
        TAG,
        "Failed to read calibration coefficients");

    bmp280.calibration.dig_T1 = bmp280_u16(&calib_coef[BMP280_CALIB_DIG_T1_OFFSET]);
    bmp280.calibration.dig_T2 = bmp280_s16(&calib_coef[BMP280_CALIB_DIG_T2_OFFSET]);
    bmp280.calibration.dig_T3 = bmp280_s16(&calib_coef[BMP280_CALIB_DIG_T3_OFFSET]);

    bmp280.calibration.dig_P1 = bmp280_u16(&calib_coef[BMP280_CALIB_DIG_P1_OFFSET]);
    bmp280.calibration.dig_P2 = bmp280_s16(&calib_coef[BMP280_CALIB_DIG_P2_OFFSET]);
    bmp280.calibration.dig_P3 = bmp280_s16(&calib_coef[BMP280_CALIB_DIG_P3_OFFSET]);
    bmp280.calibration.dig_P4 = bmp280_s16(&calib_coef[BMP280_CALIB_DIG_P4_OFFSET]);
    bmp280.calibration.dig_P5 = bmp280_s16(&calib_coef[BMP280_CALIB_DIG_P5_OFFSET]);
    bmp280.calibration.dig_P6 = bmp280_s16(&calib_coef[BMP280_CALIB_DIG_P6_OFFSET]);
    bmp280.calibration.dig_P7 = bmp280_s16(&calib_coef[BMP280_CALIB_DIG_P7_OFFSET]);
    bmp280.calibration.dig_P8 = bmp280_s16(&calib_coef[BMP280_CALIB_DIG_P8_OFFSET]);
    bmp280.calibration.dig_P9 = bmp280_s16(&calib_coef[BMP280_CALIB_DIG_P9_OFFSET]);

    return ESP_OK;
}


static esp_err_t bmp280_configure(void)
{
    uint8_t config;
    uint8_t ctrl_meas;

    // Build CONFIG register
    config =
    (BMP280_T_SB_0_5_MS   << BMP280_CONFIG_T_SB_POS)   |
    (BMP280_FILTER_OFF    << BMP280_CONFIG_FILTER_POS) |
    (BMP280_SPI3W_DISABLE << BMP280_CONFIG_SPI3W_POS);

    // Write CONFIG register
    ESP_RETURN_ON_ERROR(
    bmp280_write_register(BMP280_REG_CONFIG, config),
    TAG,
    "Failed to configure BMP280");

    // Build CTRL_MEAS register
    ctrl_meas =
    (BMP280_OSRS_X1      << BMP280_CTRL_MEAS_OSRS_T_POS) |
    (BMP280_OSRS_X1      << BMP280_CTRL_MEAS_OSRS_P_POS) |
    (BMP280_MODE_FORCED  << BMP280_CTRL_MEAS_MODE_POS);

    // Write CTRL_MEAS register
    ESP_RETURN_ON_ERROR(
    bmp280_write_register(BMP280_REG_CTRL_MEAS, ctrl_meas),
    TAG,
    "Failed to configure BMP280");

    return ESP_OK;
}

static esp_err_t bmp280_initialize(void)
{
    ESP_RETURN_ON_ERROR(
        bmp280_read_chip_id(),
        TAG,
        "Failed to read chip ID");

    ESP_RETURN_ON_ERROR(
        bmp280_reset(),
        TAG,
        "Failed to reset BMP280");

    ESP_RETURN_ON_ERROR(
        bmp280_read_calibration(),
        TAG,
        "Failed to read calibration coefficients");

    ESP_RETURN_ON_ERROR(
        bmp280_configure(),
        TAG,
        "Failed to configure BMP280");

    return ESP_OK;
}

static esp_err_t bmp280_read_raw(bmp280_raw_data_t *raw_data)
{
    // Validate argument
    if (raw_data == NULL)
    {
        ESP_LOGE(TAG, "Raw data pointer is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t raw_data_buffer[BMP280_RAW_DATA_LEN];

    // Start a new forced measurement
    uint8_t ctrl_meas =
    (BMP280_OSRS_X1 << BMP280_CTRL_MEAS_OSRS_T_POS) |
    (BMP280_OSRS_X1 << BMP280_CTRL_MEAS_OSRS_P_POS) |
    (BMP280_MODE_FORCED << BMP280_CTRL_MEAS_MODE_POS);

    ESP_RETURN_ON_ERROR(
    bmp280_write_register(BMP280_REG_CTRL_MEAS, ctrl_meas),
    TAG,
    "Failed to start forced measurement");

    // Wait for measurement to complete
    uint32_t elapsed_ms = 0;
    bmp280_status_t status;
    
    while (true)
    {
        ESP_RETURN_ON_ERROR(
            bmp280_read_status(&status),
            TAG,
            "Failed to read sensor status");

        if (!status.measuring)
        {
            break;
        }

        if (elapsed_ms >= BMP280_MEASUREMENT_TIMEOUT_MS)
        {
            ESP_LOGE(TAG, "Measurement timeout");
            return ESP_ERR_TIMEOUT;
        }

        vTaskDelay(pdMS_TO_TICKS(BMP280_STATUS_POLL_DELAY_MS));
        elapsed_ms += BMP280_STATUS_POLL_DELAY_MS;
    }

    // Read 6 bytes containing raw pressure and temperature ADC values
    // (Byte0 MSB, Byte1 LSB, Byte2 XLSB for pressure and temperature)
    ESP_RETURN_ON_ERROR(
        bmp280_read_register(
            BMP280_REG_PRESS_MSB,
            raw_data_buffer,
            sizeof(raw_data_buffer)),
        TAG,
        "Failed to read raw measurement data");

    // Convert raw data to 20-bit values
    raw_data->adc_pressure =
        ((int32_t)raw_data_buffer[BMP280_RAW_PRESS_MSB_OFFSET] << 12) |
        ((int32_t)raw_data_buffer[BMP280_RAW_PRESS_LSB_OFFSET] << 4) |
        ((int32_t)raw_data_buffer[BMP280_RAW_PRESS_XLSB_OFFSET] >> 4);

    raw_data->adc_temperature =
        ((int32_t)raw_data_buffer[BMP280_RAW_TEMP_MSB_OFFSET] << 12) |
        ((int32_t)raw_data_buffer[BMP280_RAW_TEMP_LSB_OFFSET] << 4) |
        ((int32_t)raw_data_buffer[BMP280_RAW_TEMP_XLSB_OFFSET] >> 4);

    return ESP_OK;
}

static void bmp280_compensate(const bmp280_raw_data_t *raw_data, bmp280_data_t *data)
{
    int32_t adc_T = raw_data->adc_temperature;
    int32_t adc_P = raw_data->adc_pressure;

    int32_t var1_t;
    int32_t var2_t;

    int64_t var1_p;
    int64_t var2_p;
    int64_t pressure;

    /* Temperature compensation */

    var1_t = ((((adc_T >> 3) - ((int32_t)bmp280.calibration.dig_T1 << 1))) * ((int32_t)bmp280.calibration.dig_T2)) >> 11;

    var2_t = (((((adc_T >> 4) - (int32_t)bmp280.calibration.dig_T1) * ((adc_T >> 4) - (int32_t)bmp280.calibration.dig_T1)) >> 12) * (int32_t)bmp280.calibration.dig_T3) >> 14;

    bmp280.t_fine = var1_t + var2_t;

    /* Temperature in °C */

    data->temperature = ((float)((bmp280.t_fine * 5 + 128) >> 8)) / 100.0f;

    /* Pressure compensation (64-bit Bosch algorithm) */

    var1_p = ((int64_t)bmp280.t_fine) - 128000;
    var2_p = var1_p * var1_p * (int64_t)bmp280.calibration.dig_P6;
    var2_p = var2_p + ((var1_p * (int64_t)bmp280.calibration.dig_P5) << 17);
    var2_p = var2_p + (((int64_t)bmp280.calibration.dig_P4) << 35);

    var1_p = ((var1_p * var1_p * (int64_t)bmp280.calibration.dig_P3) >> 8) + ((var1_p * (int64_t)bmp280.calibration.dig_P2) << 12);

    var1_p = (((((int64_t)1) << 47) + var1_p) * (int64_t)bmp280.calibration.dig_P1) >> 33;

    if (var1_p == 0)
    {
        data->pressure = 0.0f;
        return;
    }

    pressure = 1048576 - adc_P;
    pressure = (((pressure << 31) - var2_p) * 3125) / var1_p;

    var1_p = (((int64_t)bmp280.calibration.dig_P9) * ((pressure >> 13) * (pressure >> 13))) >> 25;

    var2_p = (((int64_t)bmp280.calibration.dig_P8) * pressure) >> 19;

    pressure = ((pressure + var1_p + var2_p) >> 8) + (((int64_t)bmp280.calibration.dig_P7) << 4);

    /* Convert Q24.8 to Pa */

    data->pressure = (float)pressure / 256.0f;
}

esp_err_t bmp280_deinit(void)
{
    if (!bmp280.initialized)
    {
        return ESP_OK;
    }

    esp_err_t ret = i2c_master_bus_rm_device(bmp280.device_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to remove BMP280 device from the I2C bus");
        return ret;
    }

    memset(&bmp280, 0, sizeof(bmp280));

    return ESP_OK;
}

esp_err_t bmp280_init(i2c_master_bus_handle_t bus_handle)
{
    // Validate the input I2C bus handle.
    if (bus_handle == NULL)
    {
        ESP_LOGE(TAG, "Invalid I2C bus handle");
        return ESP_ERR_INVALID_ARG;
    }

    // Prevent multiple initializations of the driver.
    if (bmp280.initialized)
    {
        ESP_LOGW(TAG, "BMP280 is already initialized");
        return ESP_OK;
    }

    // Configure the BMP280 device on the I2C bus.
    i2c_device_config_t dev_config =
    {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BMP280_I2C_ADDRESS,
        .scl_speed_hz = BMP280_I2C_SPEED_HZ,
    };

    // Add the sensor as an I2C device.
    esp_err_t ret = i2c_master_bus_add_device(
        bus_handle,
        &dev_config,
        &bmp280.device_handle);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add BMP280 device to the I2C bus");
        return ret;
    }

    // Initialize the sensor.
    ret = bmp280_initialize();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize BMP280");

        // Remove the device from the bus to leave the driver in a clean state.
        esp_err_t remove_ret =
            i2c_master_bus_rm_device(bmp280.device_handle);

        if (remove_ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to remove BMP280 device from the I2C bus");
        }

        bmp280.device_handle = NULL;

        return ret;
    }

    bmp280.initialized = true;

    ESP_LOGI(TAG, "BMP280 initialized successfully");

    return ESP_OK;
}

esp_err_t bmp280_read(bmp280_data_t *data)
{
    if (data == NULL)
    {
        ESP_LOGE(TAG, "Invalid data pointer");
        return ESP_ERR_INVALID_ARG;
    }

    if (!bmp280.initialized)
    {
        ESP_LOGE(TAG, "BMP280 is not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    bmp280_raw_data_t raw_data;

    ESP_RETURN_ON_ERROR(
        bmp280_read_raw(&raw_data),
        TAG,
        "Failed to read raw data from BMP280");

    bmp280_compensate(&raw_data, data);

    return ESP_OK;
}