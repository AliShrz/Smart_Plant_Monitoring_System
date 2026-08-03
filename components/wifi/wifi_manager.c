/* todo: add Wi-Fi manager implementation

Public API
──────────
wifi_manager_init()             done
wifi_manager_connect()
wifi_manager_disconnect()
wifi_manager_is_connected()
wifi_manager_get_ip()
wifi_manager_get_rssi()
wifi_manager_deinit()

Private Functions
─────────────────
wifi_manager_initialize()
wifi_manager_event_handler()
wifi_manager_start()
wifi_manager_stop()

Private Helpers
───────────────
wifi_manager_create_event_group()   done
wifi_manager_destroy_event_group()  done

*/

#include "wifi_manager.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"



typedef struct
{
    bool initialized;

    EventGroupHandle_t event_group_handle;

    esp_netif_t *netif;

    esp_event_handler_instance_t wifi_event_instance;

    esp_event_handler_instance_t ip_event_instance;

} wifi_manager_t;

static wifi_manager_t wifi = {0};

static const char *TAG = "wifi_manager";

#define WIFI_CONNECTED_BIT    BIT0
#define WIFI_FAIL_BIT         BIT1

// Private Helpers
static esp_err_t wifi_manager_create_event_group(void);
static esp_err_t wifi_manager_destroy_event_group(void);

static void wifi_manager_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data);


/************** Definitions **************/

static esp_err_t wifi_manager_create_event_group(void)
{
    if (wifi.event_group_handle != NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    wifi.event_group_handle = xEventGroupCreate();

    if (wifi.event_group_handle == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

static esp_err_t wifi_manager_destroy_event_group(void)
{
    if (wifi.event_group_handle == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    vEventGroupDelete(wifi.event_group_handle);

    wifi.event_group_handle = NULL;

    return ESP_OK;
}

esp_err_t wifi_manager_init(void)
{
    // Check initialized
    if (wifi.initialized)
    {
        return ESP_ERR_INVALID_STATE;
    }

    // Create Event Group
    ESP_RETURN_ON_ERROR(wifi_manager_create_event_group(), TAG, "Failed to create event group");
    
    // Create default network interface
    wifi.netif = esp_netif_create_default_wifi_sta();

    // Initialize Wi-Fi driver
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(esp_wifi_init(&config), TAG, "Failed to initialize Wi-Fi driver");

    // Set station mode
    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), TAG, "Failed to set Wi-Fi mode");

    // Register event handlers
    ESP_RETURN_ON_ERROR(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            WIFI_EVENT_STA_DISCONNECTED,
            wifi_manager_event_handler,
            NULL,
            &wifi.wifi_event_instance),
            TAG,
            "Failed to register Wi-Fi event handler");

    ESP_RETURN_ON_ERROR(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            wifi_manager_event_handler,
            NULL,
            &wifi.ip_event_instance),
            TAG,
            "Failed to register IP event handler");

    // Start Wi-Fi driver
    ESP_RETURN_ON_ERROR(
        esp_wifi_start(),
        TAG,
        "Failed to start Wi-Fi");

    wifi.initialized = true;

    return ESP_OK;
}

static void wifi_manager_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data)
{
    if (event_base == IP_EVENT &&
        event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event =
            (ip_event_got_ip_t *)event_data;

        xEventGroupClearBits(
            wifi.event_group_handle,
            WIFI_FAIL_BIT);

        xEventGroupSetBits(
            wifi.event_group_handle,
            WIFI_CONNECTED_BIT);

        ESP_LOGI(
            TAG,
            "Got IP: " IPSTR,
            IP2STR(&event->ip_info.ip));
    }
    else if(event_base == WIFI_EVENT &&
            event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        xEventGroupClearBits(
            wifi.event_group_handle,
            WIFI_CONNECTED_BIT);

        ESP_LOGW(TAG, "Disconnected from Wi-Fi");
    }
}