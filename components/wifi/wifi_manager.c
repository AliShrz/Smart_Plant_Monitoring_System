/* todo: add Wi-Fi manager implementation

Public API
──────────
wifi_manager_init()
wifi_manager_connect()
wifi_manager_disconnect()
wifi_manager_is_connected()
wifi_manager_get_ip()
wifi_manager_deinit()

Private Functions
─────────────────
wifi_manager_initialize()
wifi_manager_event_handler()
wifi_manager_start()
wifi_manager_stop()

Private Helpers
───────────────
wifi_manager_create_event_group()
wifi_manager_destroy_event_group()

*/

#include "wifi_manager.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


typedef struct
{
    bool initialized;

    EventGroupHandle_t event_group_handle;

    esp_netif_t *netif;

} wifi_manager_t;

static wifi_manager_t wifi = {0};

static const char *TAG = "wifi_manager";

#define WIFI_CONNECTED_BIT    BIT0
#define WIFI_FAIL_BIT         BIT1