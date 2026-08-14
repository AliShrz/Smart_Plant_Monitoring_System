#include "time_manager.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"

#include "freertos/FreeRTOS.h"

#include <time.h>

static const char *TAG = "time_manager";

static bool time_synced = false;

esp_err_t time_manager_init(void)
{
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();

    esp_sntp_config_t config =
        ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");

    esp_err_t ret = esp_netif_sntp_init(&config);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize SNTP");
        return ret;
    }

    return ESP_OK;
}

esp_err_t time_manager_sync(void)
{
    esp_err_t ret = esp_netif_sntp_start();

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start SNTP");
        return ret;
    }

    ret = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000));

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to synchronize time");
        return ret;
    }

    time_synced = true;

    ESP_LOGI(TAG, "Time synchronized");

    return ESP_OK;
}

bool time_manager_is_synced(void)
{
    return time_synced;
}

esp_err_t time_manager_get_time_and_date(
    char *time_buffer,
    size_t time_buffer_size,
    char *date_buffer,
    size_t date_buffer_size)
{
    if (!time_synced)
    {
        ESP_LOGE(TAG, "Time is not synchronized");
        return ESP_ERR_INVALID_STATE;
    }

    if (time_buffer == NULL || time_buffer_size == 0 ||
        date_buffer == NULL || date_buffer_size == 0)
    {
        ESP_LOGE(TAG, "Invalid buffer or buffer size");
        return ESP_ERR_INVALID_ARG;
    }

    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    if (strftime(time_buffer, time_buffer_size, "%I:%M %p", &timeinfo) == 0)
    {
        ESP_LOGE(TAG, "Failed to format time");
        return ESP_ERR_INVALID_ARG;
    }

    if (strftime(date_buffer, date_buffer_size, "%d %b %Y", &timeinfo) == 0)
    {
        ESP_LOGE(TAG, "Failed to format date");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}