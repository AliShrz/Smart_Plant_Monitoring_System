#include "time_manager.h"

#include "esp_log.h"

#include "esp_netif_sntp.h"

#include <time.h>

static const char *TAG = "time_manager";

esp_err_t time_manager_init(void)
{
    return ESP_OK;
}

esp_err_t time_manager_sync(void)
{
    return ESP_OK;
}

bool time_manager_is_synced(void)
{
    return false;
}

esp_err_t time_manager_get_time(
    char *buffer,
    size_t buffer_size)
{
    return ESP_OK;
}

esp_err_t time_manager_get_date(
    char *buffer,
    size_t buffer_size)
{
    return ESP_OK;
}