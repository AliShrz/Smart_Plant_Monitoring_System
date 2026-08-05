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
wifi_manager_event_handler()        done
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

    uint8_t retry_count;
    bool reconnect_enabled;

} wifi_manager_t;

static wifi_manager_t wifi = {0};

static const char *TAG = "wifi_manager";

#define WIFI_CONNECTED_BIT    BIT0
#define WIFI_FAIL_BIT         BIT1
#define WIFI_MAX_RETRY    5

// Private Helpers
static esp_err_t wifi_manager_create_event_group(void);
static esp_err_t wifi_manager_destroy_event_group(void);

// Private Functions
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
    (void)arg;

    if (event_base == IP_EVENT &&
        event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event =
            (ip_event_got_ip_t *)event_data;

        wifi.retry_count = 0;

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
    else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        xEventGroupClearBits(
            wifi.event_group_handle,
            WIFI_CONNECTED_BIT);

        if (!wifi.reconnect_enabled)
        {
            ESP_LOGI(TAG, "Wi-Fi disconnected");
            return;
        }

        if (wifi.retry_count < WIFI_MAX_RETRY)
        {
            wifi.retry_count++;

            ESP_LOGW(
                TAG,
                "Connection lost. Retrying... (%d/%d)",
                wifi.retry_count,
                WIFI_MAX_RETRY);

            esp_err_t err = esp_wifi_connect();

            if (err != ESP_OK)
            {
                ESP_LOGE(
                    TAG,
                    "Failed to start reconnect: %s",
                    esp_err_to_name(err));

                xEventGroupSetBits(
                    wifi.event_group_handle,
                    WIFI_FAIL_BIT);
            }

            return;
        }

        wifi.retry_count = 0;
        wifi.reconnect_enabled = false;

        xEventGroupSetBits(
            wifi.event_group_handle,
            WIFI_FAIL_BIT);

        ESP_LOGE(
            TAG,
            "Failed to connect after %d attempts",
            WIFI_MAX_RETRY);
    }
}

esp_err_t wifi_manager_connect(const char *ssid, const char *password)
{
    if (!wifi.initialized)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (ssid == NULL || password == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    wifi_config_t config = {0};

    if (strlen(ssid) >= sizeof(config.sta.ssid))
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (strlen(password) >= sizeof(config.sta.password))
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (ssid[0] == '\0')
    {
        return ESP_ERR_INVALID_ARG;
    }

    strlcpy(
        (char *)config.sta.ssid,
        ssid,
        sizeof(config.sta.ssid));

    strlcpy(
        (char *)config.sta.password,
        password,
        sizeof(config.sta.password));

    ESP_LOGI(TAG, "Connecting to \"%s\"...", ssid);

    /* Start a new connection attempt */
    xEventGroupClearBits(
        wifi.event_group_handle,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);

    ESP_RETURN_ON_ERROR(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &config),
        TAG,
        "Failed to configure Wi-Fi");

    esp_err_t err = esp_wifi_connect();

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Failed to start connection: %s",
            esp_err_to_name(err));

        return err;
    }

    EventBits_t bits = xEventGroupWaitBits(
        wifi.event_group_handle,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,                  // Do not clear bits on exit
        pdFALSE,                  // Wait until any bit is set
        pdMS_TO_TICKS(10000));    // 10 second timeout

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "Successfully connected");

        return ESP_OK;
    }

    if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGE(TAG, "Failed to connect to \"%s\"", ssid);

        return ESP_FAIL;
    }

    ESP_LOGE(TAG, "Connection timed out");

    return ESP_ERR_TIMEOUT;
}

bool wifi_manager_is_connected(void)
{
    if (!wifi.initialized)
    {
        return false;
    }

    EventBits_t bits = xEventGroupGetBits(
        wifi.event_group_handle);

    return ((bits & WIFI_CONNECTED_BIT) != 0);
}

esp_err_t wifi_manager_disconnect(void)
{
    if (!wifi.initialized)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (!wifi_manager_is_connected())
    {
        return ESP_ERR_INVALID_STATE;
    }

    wifi.reconnect_enabled = false;
    wifi.retry_count = 0;

    ESP_RETURN_ON_ERROR(
        esp_wifi_disconnect(),
        TAG,
        "Failed to disconnect Wi-Fi");

    ESP_LOGI(TAG, "Disconnecting from Wi-Fi");

    return ESP_OK;
}
