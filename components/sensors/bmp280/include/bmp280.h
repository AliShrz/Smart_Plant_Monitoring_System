#include "driver/i2c_master.h"
#include "stdint.h"

// BMP280 calibration data structure
typedef struct
{
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

} bmp280_calibration_t;

// BMP280 device structure
typedef struct
{
    i2c_master_dev_handle_t device_handle;
    bmp280_calibration_t calibration;
    int32_t t_fine;
    bool initialized;

} bmp280_t;

static bmp280_t bmp280 = {0};

// Status structure for BMP280 sensor
typedef struct
{
    bool measuring;
    bool im_update;
} bmp280_status_t;

// Raw data structure for BMP280 sensor readings
typedef struct
{
    int32_t adc_temperature;
    int32_t adc_pressure;
} bmp280_raw_data_t;

// Compensated data structure for BMP280 sensor readings
typedef struct
{
    float temperature;
    float pressure;
} bmp280_data_t;


esp_err_t bmp280_deinit(void);
esp_err_t bmp280_init(i2c_master_bus_handle_t bus_handle);
esp_err_t bmp280_read(bmp280_data_t *data);