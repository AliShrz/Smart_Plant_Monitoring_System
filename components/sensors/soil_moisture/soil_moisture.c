#include "soil_moisture.h"
#include "esp_check.h"


#define SOIL_ADC_CHANNEL ADC_CHANNEL_0      // GPIO36
#define SOIL_ADC_ATTEN   ADC_ATTEN_DB_12    // 11dB
#define SOIL_ADC_WIDTH   ADC_BITWIDTH_12    // 12-bit resolution
#define SOIL_DEFAULT_RAW_DRY 4000           // Maximum value for soil moisture sensor (adjust based on calibration)
#define SOIL_DEFAULT_RAW_WET 1200           // Minimum value for soil moisture sensor (adjust based on calibration)

static esp_err_t soil_moisture_read_raw(soil_moisture_data_t *data);
static esp_err_t soil_moisture_calibrate(soil_moisture_data_t *data);

static const char *TAG = "soil_moisture";

esp_err_t soil_moisture_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    ESP_RETURN_ON_ERROR(
        adc_oneshot_new_unit(
            &init_config, 
            &soil.adc_handle), 
            TAG, "Failed to create ADC unit");

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = SOIL_ADC_WIDTH,
        .atten = SOIL_ADC_ATTEN,
    };

    ESP_RETURN_ON_ERROR(
        adc_oneshot_config_channel(
            soil.adc_handle, 
            SOIL_ADC_CHANNEL, 
            &config), 
            TAG, "Failed to configure ADC channel");
    soil.initialized = true;
    soil.calibration.raw_dry = SOIL_DEFAULT_RAW_DRY;
    soil.calibration.raw_wet = SOIL_DEFAULT_RAW_WET;

    return ESP_OK;
}

esp_err_t soil_moisture_deinit(void)
{
    if (!soil.initialized)
    {
        ESP_LOGW(TAG, "Soil moisture driver is not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_ERROR(
        adc_oneshot_del_unit(soil.adc_handle),
        TAG,
        "Failed to delete ADC unit");

    memset(&soil, 0, sizeof(soil));

    ESP_LOGI(TAG, "Soil moisture driver deinitialized");
    return ESP_OK;
}

static esp_err_t soil_moisture_read_raw(soil_moisture_data_t *data)
{
    if (!soil.initialized)
    {
        ESP_LOGE(TAG, "Soil moisture driver is not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (data == NULL)
    {
        ESP_LOGE(TAG, "Invalid data pointer");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_RETURN_ON_ERROR(
        adc_oneshot_read(
            soil.adc_handle, 
            SOIL_ADC_CHANNEL, 
            &data->raw_value), 
            TAG, "Failed to read ADC value");

    return ESP_OK;
}

static esp_err_t soil_moisture_calibrate(soil_moisture_data_t *data)
{
    if (data == NULL)
    {
        ESP_LOGE(TAG, "Invalid data pointer");
        return ESP_ERR_INVALID_ARG;
    }

    int range = soil.calibration.raw_dry - soil.calibration.raw_wet;

    if (range == 0)
    {
        ESP_LOGE(TAG, "Invalid calibration values");
        return ESP_ERR_INVALID_STATE;
    }

    data->moisture_percentage =
        (float)(soil.calibration.raw_dry - data->raw_value) * 100.0f / (float)range;

    if (data->moisture_percentage < 0.0f)
    {
        data->moisture_percentage = 0.0f;
    }
    else if (data->moisture_percentage > 100.0f)
    {
        data->moisture_percentage = 100.0f;
    }

    return ESP_OK;
}

esp_err_t soil_moisture_read(soil_moisture_data_t *data)
{
    if (data == NULL)
    {
        ESP_LOGE(TAG, "Invalid data pointer");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_RETURN_ON_ERROR(
        soil_moisture_read_raw(data), 
        TAG, "Failed to read raw soil moisture");
    
    ESP_RETURN_ON_ERROR(
        soil_moisture_calibrate(data), 
        TAG, "Failed to calibrate soil moisture");

    return ESP_OK;
}


