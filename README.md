# Plant Real Time Monitoring

## Overview

**Plant Real Time Monitoring** is an Arduino and ESP8266-based plant monitoring and irrigation system that uses a **Rule-Based Algorithm** to evaluate plant and environmental conditions. The system monitors soil moisture, temperature, humidity, and rain conditions, then uses predefined rules to predict rain and determine whether the plants require watering.

Sensor data is processed by the Arduino and transmitted to an ESP8266, which connects to **Blynk** to provide real-time monitoring through a dashboard. The system is designed to automate irrigation based on measured conditions rather than relying on a fixed watering schedule.

## System Architecture

The project consists of two main modules:

1. **Arduino** — Collects sensor data, performs data averaging and rule-based analysis, predicts rain conditions, and controls the water pumps.
2. **ESP8266** — Receives processed sensor data from the Arduino, connects to Blynk, and displays the information on the monitoring dashboard.

## Features

* Real-time soil moisture monitoring
* Dual soil moisture sensors
* Temperature monitoring
* Humidity monitoring
* Rain condition monitoring
* Rule-based rain prediction
* Environmental data averaging
* Automatic irrigation
* Dual water pump control
* Blynk-based remote monitoring
* JSON-based communication between Arduino and ESP8266
* Plant condition classification

## Hardware

* Arduino
* ESP8266
* 2x Soil Moisture Sensor
* DHT11 Temperature & Humidity Sensor
* Rain Sensor
* 2x Water Pump
* Motor Driver
* Water supply
* Wi-Fi connection

## Sensor Monitoring

### Soil Moisture

Two soil moisture sensors are used to monitor different areas of the planting medium. The raw analog values are converted into percentages using a calibrated range.

The soil condition is divided into three levels:

| Moisture | Status |
| -------- | ------ |
| `< 41%`  | Dry    |
| `41–60%` | Normal |
| `≥ 61%`  | Wet    |

The system evaluates both sensors before deciding whether irrigation is required.

### Temperature

Temperature readings are obtained from the DHT11 sensor and classified into three levels:

| Temperature | Level      |
| ----------- | ---------- |
| `< 31°C`    | Low/Normal |
| `31–40°C`   | Warm       |
| `≥ 41°C`    | Hot        |

### Humidity

Relative humidity is also measured using the DHT11 and classified as:

| Humidity | Level  |
| -------- | ------ |
| `< 31%`  | Low    |
| `31–50%` | Normal |
| `≥ 51%`  | High   |

## Data Averaging

The system does not immediately use a single DHT11 or rain sensor reading for prediction. Instead, it collects measurements over a defined period and calculates an average.

Temperature and humidity are sampled once per second for **900 samples**, resulting in approximately **15 minutes of data**.

The rain sensor also collects **900 samples** and evaluates the average value as well as the stability and trend of the readings.

This approach reduces the influence of short-term fluctuations and provides a more stable basis for the rule-based prediction.

## Rule-Based Algorithm

The core of the system is a predefined **Rule-Based Algorithm**. Instead of using machine learning or an external AI model, the Arduino evaluates sensor data against predetermined conditions.

The prediction considers:

* Average temperature
* Average humidity
* Rain sensor average
* Stability of rain sensor readings
* Trend of rain sensor readings

The environmental conditions are first converted into discrete levels. These levels are then combined with the rain sensor result to produce the final rain prediction.

The output is classified as:

* **Tidak Berpotensi Hujan** — Rain is not predicted.
* **Berpotensi Hujan** — Rain is predicted.

## Rain Sensor Analysis

The rain sensor is analyzed using two methods.

### Stability Analysis

The system checks whether at least **85%** of the collected rain sensor readings remain within a defined range around the average.

This is used to determine whether the rain sensor condition is stable.

### Trend Analysis

The system also compares consecutive readings to determine whether the sensor value is consistently increasing or decreasing.

If the percentage of increasing readings reaches the configured tolerance, the system interprets the trend accordingly. This helps the system respond to a changing environmental condition rather than relying only on a single measurement.

## Watering Logic

The watering system uses the soil moisture classification together with the final rain prediction.

Watering is permitted when the final prediction indicates that rain is **not expected**.

The system evaluates the condition of both soil sensors:

* If both areas are dry, both pumps are activated.
* If one area is dry and the other is normal, both pumps can be activated.
* If one area is dry while the other is wet, only the required pump is activated.
* If the conditions do not satisfy the predefined rules, the pumps remain off.

When watering is activated, the pump operates for approximately **30 seconds** before being turned off.

## Pump Control

Two pumps are controlled using a motor driver.

**Pump 1** is connected to:

```text
AIN1
AIN2
```

**Pump 2** is connected to:

```text
AIN3
AIN4
```

The Arduino controls the direction pins to turn each pump on or off.

The pump system is intentionally controlled using predefined combinations of soil conditions and rain prediction rather than continuously operating based on a single sensor reading.

## Blynk Monitoring

The ESP8266 acts as the communication interface between the Arduino and the Blynk platform.

The Arduino sends sensor information in JSON format:

```json
{
  "soil1": 75,
  "soil2": 62,
  "temp": 29.50,
  "hum": 68.20,
  "rain": "Tidak Berpotensi Hujan"
}
```

The ESP8266 parses the JSON data and sends the values to the corresponding Blynk virtual pins.

| Data            | Blynk Pin |
| --------------- | --------- |
| Soil Moisture 1 | V8        |
| Soil Moisture 2 | V9        |
| Temperature     | V5        |
| Humidity        | V6        |
| Rain Prediction | V10       |

This allows the operator to monitor the plant environment remotely through the Blynk dashboard.

## Communication

The communication between the system components follows this structure:

```text
Sensors > Arduino > ESP8266 > Wi-Fi > Blynk
```

The Arduino handles sensor processing, rule-based decision-making, and pump control, while the ESP8266 transfers the processed data to Blynk for remote monitoring.

## Purpose

The purpose of **Plant Real Time Monitoring** is to create an automated plant monitoring system that can evaluate environmental conditions using a transparent and predefined **Rule-Based Algorithm**. The system combines sensor monitoring, rain prediction, automatic irrigation, and Blynk-based real-time visualization to help maintain suitable plant conditions while reducing unnecessary watering.
