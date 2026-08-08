# Smart Plant Monitoring System

A modular IoT-based plant monitoring system built with **ESP32** and **ESP-IDF**.

The goal of this project is to create a scalable system capable of monitoring multiple plants by measuring environmental conditions and providing real-time insights through local displays and a web dashboard.

Each plant is equipped with an ESP32-based sensor node that periodically measures:

- Soil moisture
- Ambient temperature
- Relative humidity
- Atmospheric pressure
- Ambient light intensity

The collected data is transmitted over Wi-Fi to a cloud backend for storage and analysis.

A dedicated ESP32 display node retrieves the latest measurements from the cloud and presents them on an LCD, allowing quick access to the status of every plant without requiring a computer or smartphone.

In addition, a web dashboard will provide historical data visualization, overall plant health monitoring, and intelligent recommendations based on the collected sensor data.

---

# Current Status

**Project Stage**

Display UI Development

**Overall Progress**

🟩🟩🟩🟩🟩🟩⬜⬜⬜⬜

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

### In Progress

- Display UI

### Planned

* Cloud backend
* Web dashboard
* Plant health recommendations

---

# Planned Features

## Sensor Node

- ESP32 DevKit
- Capacitive Soil Moisture Sensor
- AHT20 Temperature & Humidity Sensor
- BMP280 Pressure & Temperature Sensor
- BH1750 Light Sensor

## Display Node

* ST7735 LCD driver
* Graphics primitives
* Bitmap font rendering
* Formatted text rendering
* Multiple plant pages
* Simple graphical interface

## Dashboard

* Real-time monitoring
* Historical sensor graphs
* Plant health overview
* Intelligent care suggestions

---

# Hardware

## Sensor Node

* ESP32 DevKit
* Capacitive Soil Moisture Sensor
* BH1750 Light Sensor
* AHT20 Temperature & Humidity Sensor

## Display Node

* ESP32 DevKit
* ST7735 SPI TFT Display

---

# Project Structure

```text
Smart_Plant_Monitoring_System/

├── components/
|   ├── bus/
│   │   └── i2c_bus/
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
│   └── wifi/
│       └── wifi_manager/
│
├── main/
│   └── main.c
│
├── CMakeLists.txt
├── sdkconfig
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
* [ ] Display UI
* [ ] Cloud communication

## Phase 5 — Dashboard

* [ ] Historical data
* [ ] Multiple plant monitoring
* [ ] Plant health suggestions

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

---

# Architecture

```text
Application
      │
      ▼
┌──────────────────────────────────────────────┐
│                Display UI                    │
└──────────────────────────────────────────────┘
      │
      ▼
┌──────────────────────────────────────────────┐
│ Drivers                                      │
│ Display │ Wi-Fi │ Sensors │ Bus              │
└──────────────────────────────────────────────┘
      │
      ▼
┌──────────────────────────────────────────────┐
│                  ESP-IDF                     │
└──────────────────────────────────────────────┘
```

---

# Implemented Components

## Bus

- I2C Master

## Sensors

- Soil Moisture (ADC + Calibration)
- AHT20
- BMP280
- BH1750

## Display

- ST7735 TFT
- Graphics primitives
- Bitmap font engine
- Formatted text rendering

---
# License

This project is currently under development.
