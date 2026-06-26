# 🌱 Smart Plant Monitoring System

A modular IoT-based plant monitoring system built with **ESP32** and **ESP-IDF**.

The goal of this project is to create a scalable system capable of monitoring multiple plants by measuring environmental conditions and providing real-time insights through local displays and a web dashboard.

Each plant is equipped with an ESP32-based sensor node that periodically measures soil moisture, ambient temperature, humidity, and light intensity. The collected data is transmitted over Wi-Fi to a cloud backend for storage and analysis.

A dedicated ESP32 display node retrieves the latest measurements from the cloud and presents them on an LCD, allowing quick access to the status of every plant without requiring a computer or smartphone.

In addition, a web dashboard will provide historical data visualization, overall plant health monitoring, and intelligent recommendations based on the collected sensor data.

---

# 🚧 Current Status

**Project Stage**

Foundation

**Overall Progress**

🟩⬜⬜⬜⬜⬜⬜⬜⬜⬜

### ✅ Completed

* ESP-IDF project initialized
* Git repository created
* GitHub repository created
* Modular project structure
* Soil moisture sensor driver (ADC One-Shot)

### 🔄 In Progress

* Soil moisture sensor calibration

### 📌 Planned

* BH1750 light sensor
* AHT20 temperature & humidity sensor
* LCD display node
* Wi-Fi communication
* Cloud backend
* Web dashboard
* Plant health recommendations

---

# ✨ Planned Features

## Sensor Node

* Soil moisture measurement
* Ambient light measurement
* Temperature measurement
* Humidity measurement
* Periodic sensor acquisition
* Wi-Fi communication

## Display Node

* LCD interface
* View multiple plants
* Navigate between plants
* Display latest sensor values

## Dashboard

* Real-time monitoring
* Historical sensor graphs
* Plant health overview
* Intelligent care suggestions

---

# 🔧 Hardware

## Sensor Node

* ESP32 DevKit
* Capacitive Soil Moisture Sensor
* BH1750 Light Sensor
* AHT20 Temperature & Humidity Sensor

## Display Node

* ESP32 DevKit
* I2C LCD Display

---

# 📂 Project Structure

```text
Smart_Plant_Monitoring_System/

├── components/
│   └── sensors/
│       ├── soil_moisture.c
│       └── soil_moisture.h
│
├── main/
│   └── main.c
│
├── CMakeLists.txt
├── sdkconfig
└── README.md
```

---

# 🚀 Getting Started

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

# 🗺️ Development Roadmap

## Phase 1 — Foundation

* [x] ESP-IDF project setup
* [x] GitHub repository
* [x] Soil moisture ADC driver
* [ ] ADC calibration

## Phase 2 — Sensor Drivers

* [ ] BH1750 driver
* [ ] AHT20 driver

## Phase 3 — Display Node

* [ ] LCD driver
* [ ] User interface

## Phase 4 — Connectivity

* [ ] Wi-Fi
* [ ] Cloud communication

## Phase 5 — Dashboard

* [ ] Historical data
* [ ] Multiple plant monitoring
* [ ] Plant health suggestions

---

# 📝 Development Log

| Date    | Description                          |
| ------- | ------------------------------------ |
| 2026-06 | Project initialized using ESP-IDF    |
| 2026-06 | Implemented soil moisture ADC driver |
| 2026-06 | GitHub repository created            |

---

# 📄 License

This project is currently under development.
