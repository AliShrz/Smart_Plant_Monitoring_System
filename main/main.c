#include <stdio.h>

#include "soil_moisture.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    soil_moisture_init();

    while (1)
    {
        int value = soil_moisture_read_raw();

        printf("Raw value: %d\n", value);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}