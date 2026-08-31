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
 * [ ] Create cloud_manager component
 * [ ] Define cloud_manager.h
 * [ ] Implement cloud_manager.cpp
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

static const char *TAG = "cloud_manager";

static esp_mqtt_client_handle_t mqtt_client = NULL;


esp_err_t cloud_manager_init(system_state_t *state)
{
    if (state == NULL)
    {
        ESP_LOGE(TAG, "Invalid system state pointer");
        return ESP_ERR_INVALID_ARG;
    }

    esp_mqtt_client_config_t mqtt_cfg = {};

    mqtt_cfg.broker.address.uri =
        "mqtt://mqtt.eclipseprojects.io";

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return ESP_FAIL;
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