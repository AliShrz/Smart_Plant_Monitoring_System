/*
 * ============================================================================
 * Cloud Communication Milestone — MQTT
 * ============================================================================
 *
 * Goal:
 * Implement reliable MQTT-based communication between the ESP32 and
 * the cloud backend.
 *
 * ---------------------------------------------------------------------------
 * Phase 1 — System State
 * ---------------------------------------------------------------------------
 * [x] Add cloud_status_t
 * [x] Add cloud status to system_status_t
 * [x] Track cloud initialization state
 * [x] Track MQTT connection state
 * [x] Track cloud communication failure state
 *
 * ---------------------------------------------------------------------------
 * Phase 2 — Cloud Manager
 * ---------------------------------------------------------------------------
 * [x] Create cloud_manager component
 * [x] Define cloud_manager.h
 * [x] Implement cloud_manager.cpp
 * [ ] Define cloud manager API
 *     - cloud_manager_init()
 *     - cloud_manager_publish()
 *     - cloud_manager_deinit()
 *
 * ---------------------------------------------------------------------------
 * Phase 3 — MQTT
 * ---------------------------------------------------------------------------
 * [ ] Add ESP-IDF MQTT dependency
 * [ ] Create MQTT client
 * [ ] Implement MQTT event handling
 * [ ] Handle MQTT connection
 * [ ] Handle MQTT disconnection
 * [ ] Handle MQTT errors
 * [ ] Update cloud status in system_state_t
 *
 * ---------------------------------------------------------------------------
 * Phase 4 — Data Publishing
 * ---------------------------------------------------------------------------
 * [ ] Define MQTT topic structure
 * [ ] Define JSON payload structure
 * [ ] Convert system_state.data to JSON
 * [ ] Publish sensor data
 * [ ] Handle publish failures
 *
 * Example topic:
 *
 *     plant/1/data
 *
 * Example payload:
 *
 *     {
 *         "plant_id": 1,
 *         "soil_moisture": 42,
 *         "temperature": 24.5,
 *         "humidity": 58.2,
 *         "pressure": 1012.4,
 *         "light": 350.0
 *     }
 *
 * ---------------------------------------------------------------------------
 * Phase 5 — Application Integration
 * ---------------------------------------------------------------------------
 * [ ] Integrate cloud_manager into main.c
 * [ ] Initialize Cloud Manager after required prerequisites are ready
 * [ ] Publish data from the application loop
 * [ ] Coordinate MQTT state with Wi-Fi state
 * [ ] Handle MQTT recovery
 * [ ] Support deinitialization and reinitialization after failure
 *
 * ---------------------------------------------------------------------------
 * Phase 6 — Testing
 * ---------------------------------------------------------------------------
 * [ ] Build successfully
 * [ ] Connect ESP32 to Wi-Fi
 * [ ] Connect to MQTT broker
 * [ ] Publish sensor data
 * [ ] Verify received messages
 * [ ] Test Wi-Fi disconnection
 * [ ] Test Wi-Fi reconnection
 * [ ] Test MQTT connection failure
 * [ ] Test MQTT publish failure
 * [ ] Verify sensor failures do not break cloud communication
 *
 * ---------------------------------------------------------------------------
 * Phase 7 — Cleanup
 * ---------------------------------------------------------------------------
 * [ ] Clean up logging
 * [ ] Review error handling
 * [ ] Update README
 * [ ] Create final commit
 * [ ] Merge feature branch into main
 *
 * ============================================================================
 */

#include "cloud_manager.h"

#include "esp_err.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "cJSON.h"

static const char *TAG = "cloud_manager";

static esp_mqtt_client_handle_t mqtt_client = NULL;

static void mqtt_event_handler(
    void *handler_args,
    esp_event_base_t base,
    int32_t event_id,
    void *event_data)
{
    system_state_t *state =
        static_cast<system_state_t *>(handler_args);

    switch (event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");
            state->status.cloud.cloud_connected = true;
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT disconnected");
            state->status.cloud.cloud_connected = false;
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT error");
            break;

        default:
            break;
    }
}



esp_err_t cloud_manager_init(system_state_t *state)
{
    if (state == NULL)
    {
        ESP_LOGE(TAG, "Invalid system state pointer");
        return ESP_ERR_INVALID_ARG;
    }

    esp_mqtt_client_config_t mqtt_cfg = {};

    mqtt_cfg.broker.address.uri =
        "mqtt://broker.hivemq.com:1883";

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return ESP_FAIL;
    }

    esp_err_t ret = esp_mqtt_client_register_event(
        mqtt_client,
        MQTT_EVENT_ANY,
        mqtt_event_handler,
        state);

    if (ret != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Failed to register MQTT event handler: %s",
            esp_err_to_name(ret));

        return ret;
    }

    ESP_LOGI(TAG, "MQTT client initialized successfully");

    state->status.cloud.cloud_init = true;

    return ESP_OK;
}

esp_err_t cloud_manager_connect(system_state_t *state)
{
    if (state == NULL)
    {
        ESP_LOGE(TAG, "Invalid system state pointer");
        return ESP_ERR_INVALID_ARG;
    }

    if (!state->status.cloud.cloud_init)
    {
        ESP_LOGE(TAG, "Cloud manager is not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "MQTT client is not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = esp_mqtt_client_start(mqtt_client);

    if (ret != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Failed to start MQTT client: %s",
            esp_err_to_name(ret));

        state->status.cloud.cloud_connected = false;

        return ret;
    }

    ESP_LOGI(TAG, "MQTT client started successfully");

    return ESP_OK;
}

static cJSON *system_state_to_json(const system_state_t *state)
{
    if (state == nullptr)
    {
        ESP_LOGE(TAG, "Invalid system state pointer");
        return nullptr;
    }

    cJSON *root = cJSON_CreateObject();
    if (root == nullptr)
    {
        ESP_LOGE(TAG, "Failed to create JSON root object");
        return nullptr;
    }

    cJSON *data = cJSON_CreateObject();
    cJSON *status = cJSON_CreateObject();

    if (data == nullptr || status == nullptr)
    {
        ESP_LOGE(TAG, "Failed to create JSON objects");
        cJSON_Delete(root);
        return nullptr;
    }

    cJSON *core = cJSON_CreateObject();
    cJSON *sensors = cJSON_CreateObject();
    cJSON *wifi = cJSON_CreateObject();
    cJSON *cloud = cJSON_CreateObject();
    cJSON *display = cJSON_CreateObject();
    cJSON *time = cJSON_CreateObject();

    if (core == nullptr || sensors == nullptr || wifi == nullptr || cloud == nullptr || display == nullptr || time == nullptr)
    {
        ESP_LOGE(TAG, "Failed to create JSON sub-objects");

        cJSON_Delete(core);
        cJSON_Delete(sensors);
        cJSON_Delete(wifi);
        cJSON_Delete(cloud);
        cJSON_Delete(display);
        cJSON_Delete(time);

        cJSON_Delete(root);
        cJSON_Delete(data);
        cJSON_Delete(status);
        
        return nullptr;
    }

    cJSON_AddItemToObject(root, "data", data);
    cJSON_AddItemToObject(root, "status", status);

    cJSON_AddItemToObject(status, "core", core);
    cJSON_AddItemToObject(status, "sensors", sensors);
    cJSON_AddItemToObject(status, "wifi", wifi);
    cJSON_AddItemToObject(status, "cloud", cloud);
    cJSON_AddItemToObject(status, "display", display);
    cJSON_AddItemToObject(status, "time", time);

    cJSON_AddNumberToObject(data, "plant_id", state->data.plant_id);
    cJSON_AddStringToObject(data, "date", state->data.date);
    cJSON_AddStringToObject(data, "time", state->data.time);
    cJSON_AddNumberToObject(data, "soil_moisture", state->data.soil_moisture_percent);
    cJSON_AddNumberToObject(data, "temperature", state->data.temperature_c);
    cJSON_AddNumberToObject(data, "pressure", state->data.pressure_hpa);
    cJSON_AddNumberToObject(data, "light", state->data.light_lux);
    cJSON_AddStringToObject(data, "wifi_ip", state->data.wifi_ip);

    cJSON_AddBoolToObject(core, "i2c_bus_init", state->status.core.i2c_bus_init);
    cJSON_AddBoolToObject(core, "nvs_init", state->status.core.nvs_init);
    cJSON_AddBoolToObject(core, "netif_init", state->status.core.netif_init);
    cJSON_AddBoolToObject(core, "event_loop_init", state->status.core.event_loop_init);

    cJSON_AddBoolToObject(sensors, "soil_moisture_init", state->status.sensors.soil_moisture_init);
    cJSON_AddBoolToObject(sensors, "aht20_init", state->status.sensors.aht20_init);
    cJSON_AddBoolToObject(sensors, "bmp280_init", state->status.sensors.bmp280_init);
    cJSON_AddBoolToObject(sensors, "bh1750_init", state->status.sensors.bh1750_init);

    cJSON_AddBoolToObject(wifi, "wifi_init", state->status.wifi.wifi_init);
    cJSON_AddBoolToObject(wifi, "wifi_connected", state->status.wifi.wifi_connected);
    cJSON_AddBoolToObject(wifi, "wifi_failed", state->status.wifi.wifi_failed);

    cJSON_AddBoolToObject(cloud, "cloud_init", state->status.cloud.cloud_init);
    cJSON_AddBoolToObject(cloud, "cloud_connected", state->status.cloud.cloud_connected);
    cJSON_AddBoolToObject(cloud, "cloud_failed", state->status.cloud.cloud_failed);

    cJSON_AddBoolToObject(display, "display_init", state->status.display.display_init);
    cJSON_AddBoolToObject(display, "display_ui_init", state->status.display.display_ui_init);

    cJSON_AddBoolToObject(time, "time_init", state->status.time.time_init);
    cJSON_AddBoolToObject(time, "time_synced", state->status.time.time_synced);


    return root;
}