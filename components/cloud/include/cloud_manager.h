#ifndef CLOUD_MANAGER_H
#define CLOUD_MANAGER_H

#include "esp_err.h"
#include "system_state.h"

#ifdef __cplusplus
extern "C" {
#endif


esp_err_t cloud_manager_init(system_state_t *state);

esp_err_t cloud_manager_connect(system_state_t *state);

esp_err_t cloud_manager_publish_state(const system_state_t *state);

esp_err_t cloud_manager_disconnect(system_state_t *state);

esp_err_t cloud_manager_deinit(system_state_t *state);

#ifdef __cplusplus
}
#endif

#endif // CLOUD_MANAGER_H