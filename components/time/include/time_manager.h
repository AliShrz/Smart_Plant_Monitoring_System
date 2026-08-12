#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

esp_err_t time_manager_init(void);

esp_err_t time_manager_sync(void);

bool time_manager_is_synced(void);

esp_err_t time_manager_get_time(
    char *buffer,
    size_t buffer_size);

esp_err_t time_manager_get_date(
    char *buffer,
    size_t buffer_size);

#endif /* TIME_MANAGER_H */