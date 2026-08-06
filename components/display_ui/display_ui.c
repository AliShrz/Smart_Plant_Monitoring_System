#include "display_ui.h"






esp_err_t display_ui_init(void)
{
    return ESP_OK;
}

esp_err_t display_ui_show(const display_ui_data_t *data)
{
    if (data == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t display_ui_deinit(void)
{
    return ESP_OK;
}