#pragma once

#include "esp_err.h"
#include "system_state.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t cloud_manager_init(void);

esp_err_t cloud_manager_publish(
    const system_state_t *state);

esp_err_t cloud_manager_deinit(void);

#ifdef __cplusplus
}
#endif

esp_err_t cloud_manager_init();
esp_err_t cloud_manager_connect();
esp_err_t cloud_manager_publish(const system_data_t *data);
esp_err_t cloud_manager_is_connected();
esp_err_t cloud_manager_disconnect();
esp_err_t cloud_manager_deinit();