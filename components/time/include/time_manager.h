#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

esp_err_t time_manager_init(void);

esp_err_t time_manager_sync(void);

bool time_manager_is_synced(void);

esp_err_t time_manager_get_time_and_date(
    char *time_buffer,
    size_t time_buffer_size,
    char *date_buffer,
    size_t date_buffer_size);

#endif /* TIME_MANAGER_H */