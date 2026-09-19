# Smart Plant Monitoring System

A modular IoT-based plant monitoring system built with **ESP32** and **ESP-IDF**.

The goal of this project is to create a scalable system capable of monitoring multiple plants by measuring environmental conditions and providing real-time insights through local displays and a web dashboard.

Each plant is equipped with an ESP32-based sensor node that periodically measures:

- Soil moisture
- Ambient temperature
- Relative humidity
- Atmospheric pressure
- Ambient light intensity

The collected data is transmitted over Wi-Fi using MQTT to an MQTT broker for cloud communication. A dedicated backend for data storage, analysis, and dashboard visualization is planned.

A dedicated ESP32 display node will retrieve measurements from the backend and present them on an LCD, allowing quick access to the status of every plant without requiring a computer or smartphone.

In addition, a web dashboard will provide historical data visualization, overall plant health monitoring, and intelligent recommendations based on the collected sensor data.

---

# Current Status

**Project Stage**

Cloud Communication

**Overall Progress**

🟩🟩🟩🟩🟩🟩🟩🟩⬜⬜

### Completed

- ESP-IDF project initialized
- Git repository created
- GitHub repository created
- Modular component-based architecture

#### Bus Drivers

- I2C Master driver

#### Sensor Drivers

- Soil moisture sensor driver (ADC One-Shot + calibration)
- AHT20 temperature & humidity sensor driver
- BMP280 pressure & temperature sensor driver
- BH1750 ambient light sensor driver

#### Display Driver

- ST7735 LCD driver

#### Graphics

- Pixel
- Lines
- Rectangles
- Circles
- Triangles

#### Text Engine

- Bitmap font engine
- 5×7 font
- Text rendering
- Transparent text rendering
- printf-style text rendering

#### Connectivity

- Wi-Fi manager
- Automatic reconnect
- Connection management
- IP address API
- RSSI API
- Internet time and date synchronization

#### Display UI

- Modular widget-based UI
- Plant pot widget
- Soil moisture visualization
- Plant / cactus graphic
- Sun widget
- Dynamic sun rays
- Light intensity display
- Temperature thermometer
- Dynamic temperature fill
- Humidity drop widget
- Wi-Fi status widget
- Real sensor data integration
- Golden-ratio based layout
- Widget spacing and alignment

#### System State

- Centralized system state structure
- Centralized system data structure
- Component initialization status tracking
- Component deinitialization and recovery handling
- Application data flow

#### Cloud Communication

- MQTT client
- MQTT broker connection
- MQTT connection state tracking
- MQTT disconnection handling
- MQTT automatic reconnect
- JSON serialization of system state
- Sensor data publishing
- QoS 1 publishing
- Publish failure handling
- Cloud connection state tracking

### In Progress

- NanoPi-based MQTT infrastructure
- MQTT failure and recovery testing

### Planned

- Cloud backend
- Web dashboard
- Historical data visualization
- Multiple plant monitoring
- Plant health recommendations

---

# Planned Features

## Sensor Node

- ESP32 DevKit
- Capacitive Soil Moisture Sensor
- AHT20 Temperature & Humidity Sensor
- BMP280 Pressure & Temperature Sensor
- BH1750 Light Sensor

## Display Node

- ESP32 DevKit
- ST7735 SPI TFT Display
- Graphics primitives
- Bitmap font rendering
- Multiple plant pages
- Real-time sensor visualization
- Internet-synchronized time and date
- Simple graphical interface

## Dashboard

- Real-time monitoring
- Historical sensor graphs
- Plant health overview
- Multiple plant monitoring
- Intelligent care suggestions

---

# Hardware

## Sensor Node

- ESP32 DevKit
- Capacitive Soil Moisture Sensor
- BH1750 Light Sensor
- AHT20 Temperature & Humidity Sensor
- BMP280 Pressure & Temperature Sensor

## Display Node

- ESP32 DevKit
- ST7735 SPI TFT Display

---

# Project Structure

```text
Smart_Plant_Monitoring_System/

├── components/
|   ├── bus/
│   │   └── i2c_bus/
│   │
│   ├── cloud/
│   │
|   ├── display/
│   ├── display_ui/
│   │
│   ├── sensors/
│   │   ├── soil_moisture/
│   │   ├── aht20/
│   │   ├── bmp280/
│   │   └── bh1750/
│   │
│   ├── time/
│   ├── wifi/
│   └── system_state/
│
├── config/
│   ├── credentials.h
│   └── credentials.example.h   
│
├── main/
│   └── main.c
│
├── CMakeLists.txt
├── sdkconfig
├── dependencies.lock
└── README.md
```

---

# Getting Started

## Activate environment
```bash
source ~/.espressif/v6.0.1/esp-idf/export.sh
```

## Build

```bash
idf.py build
```

## Flash

```bash
idf.py flash
```

## Monitor

```bash
idf.py monitor
```

---

# Development Roadmap

## Phase 1 — Foundation

* [x] ESP-IDF project setup
* [x] GitHub repository
* [x] Modular component architecture
* [x] Soil moisture ADC driver

## Phase 2 — Display 

* [x] ST7735 driver
* [x] Graphics primitives
* [x] Font engine
* [x] Text rendering
* [x] printf API

## Phase 3 — Sensor Drivers

* [x] I2C Master
* [x] AHT20
* [x] BMP280
* [x] BH1750
* [x] ADC calibration

## Phase 4 — Connectivity

* [x] Wi-Fi
* [x] Display UI
* [x] Time synchronization
* [x] System state and data flow
* [x] Application error handling
* [x] MQTT cloud communication
* [x] Wi-Fi disconnect and reconnect handling

## Phase 5 — Local Cloud Infrastructure

- [ ] NanoPi setup
- [ ] MQTT broker
- [ ] ESP32 → NanoPi communication
- [ ] MQTT failure and recovery testing
- [ ] MQTT publish failure testing
- [ ] Sensor failure isolation testing

## Phase 6 — Dashboard

- [ ] Cloud backend
- [ ] Historical data
- [ ] Multiple plant monitoring
- [ ] Real-time monitoring
- [ ] Plant health suggestions

---

# Development Log

| Date    | Description                                     |
|---------|-------------------------------------------------|
| 2026-06 | ESP-IDF project initialized                     |
| 2026-06 | GitHub repository created                       |
| 2026-07 | Implemented soil moisture ADC driver            |
| 2026-07 | Implemented ST7735 LCD driver                   |
| 2026-07 | Added graphics primitives                       |
| 2026-07 | Added bitmap font engine                        |
| 2026-07 | Added transparent text rendering                |
| 2026-07 | Added printf-style text rendering               |
| 2026-07 | Refactored project into modular components      |
| 2026-07 | Implemented I2C master driver                   |
| 2026-07 | Added AHT20 temperature & humidity driver       |
| 2026-07 | Added BMP280 pressure & temperature driver      |
| 2026-07 | Added BH1750 ambient light sensor driver        |
| 2026-08 | Added soil moisture calibration                 |
| 2026-08 | Implemented Wi-Fi manager driver                |
| 2026-08 | Implemented modular display UI widgets          |
| 2026-08 | Added NTP time synchronization                  |
| 2026-08 | Added system data structure and data flow       |
| 2026-08 | Implemented application error handling and component recovery |
| 2026-09 | Implemented MQTT cloud communication and JSON state publishing |
| 2026-09 | Implemented Wi-Fi connection, disconnection, and reconnect handling |

---

# Architecture

```text
```text
                              ESP32 SENSOR NODE
                                      │
                                      ▼
                              ┌───────────────┐
                              │  APPLICATION  │
                              │    main.c     │
                              └───────┬───────┘
                                      │
                                      │ owns & updates
                                      ▼
                            ┌───────────────────┐
                            │  system_state_t   │
                            ├───────────────────┤
                            │ system_data_t     │
                            │ system_status_t   │
                            └─────────┬─────────┘
                                      │
                 ┌────────────────────┼────────────────────┐
                 │                    │                    │
                 ▼                    ▼                    ▼
          ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
          │   Sensors    │     │  Display UI  │     │    Cloud     │
          │              │     │              │     │   Manager    │
          └──────┬───────┘     └──────┬───────┘     └──────┬───────┘
                 │                    │                    │
       ┌─────────┼─────────┐          ▼                    │
       │         │         │   ┌──────────────┐            │
       ▼         ▼         ▼   │ Graphics +   │            │
     AHT20    BMP280     BH1750│ Font Engine  │            │
       │         │         │   └──────┬───────┘            │
       └─────────┼─────────┘          │                    │
                 │                    ▼                    │
                 ▼               ST7735 LCD                │
        Soil Moisture                                      │
         (ADC One-Shot)                                    │
                                                          │
                                                          │ MQTT
                                                          ▼
                                                   ┌──────────────┐
                                                   │ MQTT Broker  │
                                                   └──────────────┘


      ┌────────────────┐
      │  Wi-Fi Manager │──── connection / RSSI / IP ────► system_state_t
      └───────┬────────┘
              │
              │ Wi-Fi
              ▼
         Wi-Fi Network


      ┌────────────────┐
      │  Time Manager  │──── time / date ───────────────► system_state_t
      └────────────────┘
```

```

---

# Implemented Components

## Bus

- I2C Master

## Sensors

- Soil Moisture (ADC One-Shot + Calibration)
- AHT20
- BMP280
- BH1750

## Display

- ST7735 TFT
- Graphics primitives
- Bitmap font engine
- Formatted text rendering
- Display UI framework
- Sensor visualization widgets

## Connectivity

- Wi-Fi Manager
- Automated reconnect
- Connection status
- IP address API
- RSSI API
- Internet time and date synchronization
- MQTT client
- MQTT broker communication
- JSON state publishing
- MQTT connection state tracking

## Application State

- Centralized system state structure
- Centralized system data
- Component initialization status tracking
- Application data flow
- Component error handling
- Component recovery through deinitialization and reinitialization

---
# License

This project is currently under development.
