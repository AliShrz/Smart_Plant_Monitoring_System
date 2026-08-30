#include <stdio.h>

#include "esp_check.h"
#include "esp_log.h"
#include "soil_moisture.h"
#include "i2c_bus.h"
#include "aht20.h"
#include "bmp280.h"
#include "bh1750.h"
#include "display.h"
#include "display_ui.h"
#include "wifi_manager.h"
#include "time_manager.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
// #include "esp_lcd_panel_ops.h"
#include "display_font_5x7.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SSID "HUAWEI-2.4G-k4JK_ext"
#define PASS "3jBc8cpR" // 3jBc8cpR

static const char *TAG = "main";

void app_main(void)
{
    system_state_t system_state;
    system_state.status = (system_status_t){
        .core = {
            .nvs_init = false,
            .netif_init = false,
            .event_loop_init = false
        },
        .sensors = {
            .soil_moisture_init = false,
            .aht20_init = false,
            .bmp280_init = false,
            .bh1750_init = false
        },
        .wifi = {
            .wifi_init = false,
            .wifi_connected = false
        },
        .cloud = {
            .cloud_init = false,
            .cloud_connected = false,
            .cloud_failed = false
        },
        .display = {
            .display_init = false
        }
    };

    system_state.data = (system_data_t){
        .plant_id = 1,
        .time = "",
        .date = "",
        .soil_moisture_percent = 0,
        .temperature_c = 0.0,
        .humidity_percent = 0.0,
        .pressure_hpa = 0,
        .light_lux = 0,
        .wifi_ip = "",
        };

    esp_err_t ret;

    ret = nvs_flash_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(ret));
        system_state.status.core.nvs_init = false;
    }
    else
    {
        system_state.status.core.nvs_init = true;
    }

    ret = esp_netif_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize network interface: %s", esp_err_to_name(ret));
        system_state.status.core.netif_init = false;
    }
    else
    {
        system_state.status.core.netif_init = true;
    }


    ret = esp_event_loop_create_default();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create event loop: %s", esp_err_to_name(ret));
        system_state.status.core.event_loop_init = false;
    }
    else
    {
        system_state.status.core.event_loop_init = true;
    }

    if (system_state.status.core.event_loop_init && system_state.status.core.netif_init)
    {
        ret = wifi_manager_init();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to initialize WiFi: %s", esp_err_to_name(ret));
            system_state.status.wifi.wifi_init = false;
        }
        else
        {
            system_state.status.wifi.wifi_init = true;
        }
    }
    else
    {
        if (system_state.status.core.event_loop_init == false)
            ESP_LOGE(TAG, "Event loop not initialized, skipping WiFi initialization.");
        if (system_state.status.core.netif_init == false)
            ESP_LOGE(TAG, "Network interface not initialized, skipping WiFi initialization.");
        system_state.status.wifi.wifi_init = false;
    }


    if(!system_state.status.display.display_init)
    {
        ret = display_init();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to initialize display: %s", esp_err_to_name(ret));
        }
        else
        {
            system_state.status.display.display_init = true;
        }
    }
    else
    {
        system_state.status.display.display_init = true;
    }


    if (system_state.status.sensors.soil_moisture_init == false)
    {
        ret = soil_moisture_init();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to initialize soil moisture sensor: %s", esp_err_to_name(ret));
        }
        else
        {
            system_state.status.sensors.soil_moisture_init = true;
        }
    }
    else
    {
        system_state.status.sensors.soil_moisture_init = true;
    }

    i2c_master_bus_handle_t bus_handle;

    i2c_bus_config_t bus_config =
    {
        .port = I2C_NUM_0,
        .sda = GPIO_NUM_21,
        .scl = GPIO_NUM_22,
        .enable_internal_pullup = true,
        .glitch_ignore_cnt = 0,
    };

    /* Initialize I2C bus */
    if (!system_state.status.core.i2c_bus_init)
    {
        ret = i2c_bus_init(&bus_config, &bus_handle);

        if (ret != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Failed to initialize I2C bus: %s",
                esp_err_to_name(ret));

            system_state.status.core.i2c_bus_init = false;
        }
        else
        {
            system_state.status.core.i2c_bus_init = true;
        }
    }

    /* Initialize AHT20 */
    if (system_state.status.core.i2c_bus_init &&
        !system_state.status.sensors.aht20_init)
    {
        ret = aht20_init(bus_handle);

        if (ret != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Failed to initialize AHT20 sensor: %s",
                esp_err_to_name(ret));

            system_state.status.sensors.aht20_init = false;
        }
        else
        {
            system_state.status.sensors.aht20_init = true;
        }
    }

    /* Initialize BMP280 */
    if (system_state.status.core.i2c_bus_init &&
        !system_state.status.sensors.bmp280_init)
    {
        ret = bmp280_init(bus_handle);

        if (ret != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Failed to initialize BMP280 sensor: %s",
                esp_err_to_name(ret));

            system_state.status.sensors.bmp280_init = false;
        }
        else
        {
            system_state.status.sensors.bmp280_init = true;
        }
    }

    /* Initialize BH1750 */
    if (system_state.status.core.i2c_bus_init &&
        !system_state.status.sensors.bh1750_init)
    {
        ret = bh1750_init(bus_handle);

        if (ret != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Failed to initialize BH1750 sensor: %s",
                esp_err_to_name(ret));

            system_state.status.sensors.bh1750_init = false;
        }
        else
        {
            system_state.status.sensors.bh1750_init = true;
        }
    }

    esp_ip4_addr_t ip;
    soil_moisture_data_t soil_moisture_data;
    aht20_data_t sensor_data;
    bmp280_data_t bmp280_data;
    bh1750_data_t bh1750_data;

    if(system_state.status.display.display_init)
    {
        display_fill(COLOR_BLACK); // Draw a blank bitmap (black screen)
        vTaskDelay(pdMS_TO_TICKS(1000));

        display_fill(COLOR_WHITE); // Draw a blank bitmap (white screen)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    if(system_state.status.display.display_init && !system_state.status.display.display_ui_init)
    {
        ret = display_ui_init(&system_state);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to initialize display UI: %s", esp_err_to_name(ret));
            system_state.status.display.display_ui_init = false;
        }
        else
        {
            system_state.status.display.display_ui_init = true;
        }
    }

    // int8_t wifi_rssi;
    // char ip_string[16];
    // uint8_t count = 0;

    while (1)
    {
        if (system_state.status.wifi.wifi_init)
        {
            /* Update actual Wi-Fi connection state */
            system_state.status.wifi.wifi_connected =
                wifi_manager_is_connected();
        
            if (!system_state.status.wifi.wifi_connected)
            {
                ESP_LOGI(
                    TAG,
                    "WiFi is disconnected, attempting to connect...");
                
                ret = wifi_manager_connect(SSID, PASS);
                
                if (ret != ESP_OK)
                {
                    ESP_LOGE(
                        TAG,
                        "Failed to connect to WiFi: %s",
                        esp_err_to_name(ret));
                    
                    system_state.status.wifi.wifi_connected = false;
                    system_state.data.wifi_ip[0] = '\0';
                }
                else
                {
                    system_state.status.wifi.wifi_connected = true;
                
                    ESP_LOGI(
                        TAG,
                        "Connected to WiFi");
                    
                    /* Get IP address after successful connection */
                    if (wifi_manager_get_ip(&ip) == ESP_OK)
                    {
                        ESP_LOGI(
                            TAG,
                            "IP: " IPSTR,
                            IP2STR(&ip));
                        
                        esp_ip4addr_ntoa(
                            &ip,
                            system_state.data.wifi_ip,
                            sizeof(system_state.data.wifi_ip));
                    }
                    else
                    {
                        ESP_LOGE(
                            TAG,
                            "Failed to get IP address");
                        
                        system_state.data.wifi_ip[0] = '\0';
                    }
                }
            }
            else
            {
                /* Wi-Fi is already connected */
                system_state.status.wifi.wifi_connected = true;
            
                system_state.data.wifi_rssi =
                    wifi_manager_get_rssi();
            
                ESP_LOGI(
                    TAG,
                    "WiFi connected - RSSI: %d dBm",
                    system_state.data.wifi_rssi);
            }
        }
        else
        {
            ESP_LOGW(
                TAG,
                "WiFi is not initialized, skipping connection attempt.");
            
            system_state.status.wifi.wifi_connected = false;
            system_state.data.wifi_ip[0] = '\0';
        }


        /********* time **********/
        if(system_state.status.wifi.wifi_connected && !system_state.status.time.time_init)
        {
            ret = time_manager_init();
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to initialize time manager: %s", esp_err_to_name(ret));
            }
            else
            {
                ESP_LOGI(TAG, "Time manager initialized successfully.");
                system_state.status.time.time_init = true;
            }
        }

        if(system_state.status.wifi.wifi_connected && system_state.status.time.time_init && !system_state.status.time.time_synced)
        {
            ret = time_manager_sync();

            if (ret != ESP_OK)
            {
                ESP_LOGE(
                    TAG,
                    "Failed to synchronize time: %s",
                    esp_err_to_name(ret));
                
                system_state.status.time.time_synced = false;
            }
            else
            {
                system_state.status.time.time_synced = true;
            }
        }

        /********* update time and date **********/
        if (system_state.status.time.time_init)
        {
            if (system_state.status.time.time_synced)
            {
                ret = time_manager_get_time_and_date(
                    system_state.data.time,
                    sizeof(system_state.data.time),
                    system_state.data.date,
                    sizeof(system_state.data.date));
                
                if (ret != ESP_OK)
                {
                    ESP_LOGE(
                        TAG,
                        "Failed to get time and date: %s",
                        esp_err_to_name(ret));
                }
                else
                {
                    ESP_LOGI(
                        TAG,
                        "Time: %s",
                        system_state.data.time);
                    
                    ESP_LOGI(
                        TAG,
                        "Date: %s",
                        system_state.data.date);
                }
            }
            else
            {
                ESP_LOGW(
                    TAG,
                    "Time is not synchronized yet.");
            }
        }

        /*******************/

        if(system_state.status.sensors.soil_moisture_init)
        {
            ret = soil_moisture_read(&soil_moisture_data);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to read soil moisture data: %s", esp_err_to_name(ret));
                system_state.status.sensors.soil_moisture_init = false;
            }
            else
            {
                system_state.data.soil_moisture_percent = soil_moisture_data.moisture_percentage;
                ESP_LOGI(TAG, "Soil moisture: %.2f%%", system_state.data.soil_moisture_percent);
            }
        }
        else
        {
            ESP_LOGW(TAG, "Soil moisture sensor is not initialized, skipping read.");
        }


        /******* AHT20 *******/
        if (system_state.status.core.i2c_bus_init)
        {
            if (!system_state.status.sensors.aht20_init)
            {
                ESP_LOGI(
                    TAG,
                    "AHT20 is not initialized, attempting to initialize...");
                
                ret = aht20_deinit();
                
                if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
                {
                    ESP_LOGE(
                        TAG,
                        "Failed to deinitialize AHT20 sensor: %s",
                        esp_err_to_name(ret));
                }
                else
                {
                    ret = aht20_init(bus_handle);
                
                    if (ret != ESP_OK)
                    {
                        ESP_LOGE(
                            TAG,
                            "Failed to initialize AHT20 sensor: %s",
                            esp_err_to_name(ret));
                        
                        system_state.status.sensors.aht20_init = false;
                    }
                    else
                    {
                        ESP_LOGI(
                            TAG,
                            "AHT20 initialized successfully.");
                        
                        system_state.status.sensors.aht20_init = true;
                    }
                }
            }
        }

        
        if(system_state.status.sensors.aht20_init)
        {
            ret = aht20_read(&sensor_data);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to read AHT20 data: %s", esp_err_to_name(ret));
                system_state.status.sensors.aht20_init = false;
            }
            else
            {
                ESP_LOGI(TAG, "Humidity: %.2f%%", sensor_data.humidity);
                system_state.data.humidity_percent = sensor_data.humidity;
            }
        }
        else
        {
            ESP_LOGW(TAG, "AHT20 sensor is not initialized, skipping read.");
        }

        /******* BMP280 *******/
        if(system_state.status.core.i2c_bus_init)
        {
            if (!system_state.status.sensors.bmp280_init)
            {
                ESP_LOGI(
                    TAG,
                    "BMP280 is not initialized, attempting to initialize...");
                
                ret = bmp280_deinit();
                
                if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
                {
                    ESP_LOGE(
                        TAG,
                        "Failed to deinitialize BMP280 sensor: %s",
                        esp_err_to_name(ret));
                }
                else
                {
                    ret = bmp280_init(bus_handle);
                
                    if (ret != ESP_OK)
                    {
                        ESP_LOGE(
                            TAG,
                            "Failed to initialize BMP280 sensor: %s",
                            esp_err_to_name(ret));
                        
                        system_state.status.sensors.bmp280_init = false;
                    }
                    else
                    {
                        ESP_LOGI(
                            TAG,
                            "BMP280 initialized successfully.");
                        
                        system_state.status.sensors.bmp280_init = true;
                    }
                }
            }
        }

        if(system_state.status.sensors.bmp280_init)
        {
            ret = bmp280_read(&bmp280_data);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to read BMP280 data: %s", esp_err_to_name(ret));
                system_state.status.sensors.bmp280_init = false;
            }
            else
            {
                ESP_LOGI(TAG, "Temperature: %.2f°C", bmp280_data.temperature);
                ESP_LOGI(TAG, "Pressure: %.2f hPa", bmp280_data.pressure);
                system_state.data.temperature_c = bmp280_data.temperature;
                system_state.data.pressure_hpa = bmp280_data.pressure;
            }
        }
        else
        {
            ESP_LOGW(TAG, "BMP280 sensor is not initialized, skipping read.");
        }

        /******* BH1750 *******/
        if (system_state.status.core.i2c_bus_init)
        {
            if (!system_state.status.sensors.bh1750_init)
            {
                ESP_LOGI(
                    TAG,
                    "BH1750 is not initialized, attempting to initialize...");
                
                ret = bh1750_deinit();
                
                if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
                {
                    ESP_LOGE(
                        TAG,
                        "Failed to deinitialize BH1750 sensor: %s",
                        esp_err_to_name(ret));
                }
                else
                {
                    ret = bh1750_init(bus_handle);
                
                    if (ret != ESP_OK)
                    {
                        ESP_LOGE(
                            TAG,
                            "Failed to initialize BH1750 sensor: %s",
                            esp_err_to_name(ret));
                        
                        system_state.status.sensors.bh1750_init = false;
                    }
                    else
                    {
                        ESP_LOGI(
                            TAG,
                            "BH1750 initialized successfully.");
                        
                        system_state.status.sensors.bh1750_init = true;
                    }
                }
            }
        }

        if(system_state.status.sensors.bh1750_init)
        {
            ret = bh1750_read(&bh1750_data);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to read BH1750 data: %s", esp_err_to_name(ret));
                system_state.status.sensors.bh1750_init = false;
            }
            else
            {
                ESP_LOGI(TAG, "Light: %d lux", bh1750_data.lux);
                system_state.data.light_lux = bh1750_data.lux;
            }
        }
        else
        {
            ESP_LOGW(TAG, "BH1750 sensor is not initialized, skipping read.");
        }


        /********* Display UI **********/
        if(system_state.status.display.display_ui_init)
        {
            ret = display_ui_show(&system_state);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to show display UI: %s", esp_err_to_name(ret));
            }
            else
            {
                ESP_LOGI(TAG, "Display UI updated successfully.");
            }
        
        }


        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}