#include "soil_moisture.h"

#include "esp_adc/adc_oneshot.h"

#define SOIL_ADC_CHANNEL ADC_CHANNEL_0      // GPIO36
#define SOIL_ADC_ATTEN   ADC_ATTEN_DB_12    // 11dB
#define SOIL_ADC_WIDTH   ADC_BITWIDTH_12    // 12-bit resolution

static adc_oneshot_unit_handle_t adc_handle;

esp_err_t soil_moisture_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = SOIL_ADC_WIDTH,
        .atten = SOIL_ADC_ATTEN,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, SOIL_ADC_CHANNEL, &config));

    return ESP_OK;
    }


int soil_moisture_read_raw(void)
{
    int raw_value;

    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, SOIL_ADC_CHANNEL, &raw_value));

    return raw_value;
}
