# Motion Velocity Tracking and HTTP Data Transmission with ESP32 and MPU9250 (SPI Connection)

This project demonstrates how to use an ESP32 microcontroller to capture motion and velocity data from an MPU9250 sensor via SPI. The data is transmitted to a server via WiFi using HTTP POST requests, where a PHP script on the server receives and stores the data.

## Table of Contents

## Table of Contents
- [Project Overview](#project-overview)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Wiring and SPI Connection](#wiring-and-spi-connection)
- [Installation and Setup](#installation-and-setup)
- [Server-Side PHP Script](#server-side-php-script)
- [Code Overview](#code-overview)
  - [MPU_GET Task](#mpu_get-task)
  - [HTTP_SEND Task](#http_send-task)
  - [Calibration Function](#calibration-function)
  - [WiFi Connection Function](#wifi-connection-function)
- [How to Run](#how-to-run)
- [Author](#author)

## Project Overview
This project uses the ESP32 microcontroller to collect motion and velocity data from the MPU9250 sensor via the SPI interface. The data is gathered in batches and sent to a remote server using HTTP POST requests. A server-side PHP script stores the data in a file.

## Key Features:
* Real-time data collection from MPU9250 (acceleration, gyroscope) using SPI.
* Sends collected data to a server via HTTP.
* Server-side PHP script for storing data.
* Automatic WiFi reconnection.
* Sensor calibration for accurate data readings.

## Hardware Requirements
* **ESP32 development board**
* **MPU9250 sensor**
* **SPI interface connection**
* Breadboard and wires

## Software Requirements
* **Arduino IDE** (latest version)
* **MPU9250_WE library**
* **WiFi library**
* **HTTPClient library**
* **PHP-enabled web serve**r (for receiving and storing the data)

## Wiring and SPI Connection
| ESP32 Pin | MPU9250 Pin        | Description  |
|--------|---------------------|--------------|
| `GPIO 18` (SCK) | `SCL`      | SPI Clock |
| `GPIO 19` (MISO) | `SDA`| SPI Master In Slave Out (data) |
| `GPIO 23` (MOSI)| `AD0`| SPI Master Out Slave In (data)   |
| `GPIO 5` (CS)| `NCS`| Chip Select (active low)   |
| `GND` | `GND`| Ground  |
| `3.3V`| `VCC`|Power supply   |

Make sure to adjust the pin numbers in your code if you are using different GPIO pins for the SPI connection.

## Installation and Setup
1. **Install the ESP32 board support in Arduino IDE:**
   * Go to **File > Preferences.**
   * Add the following URL to the "Additional Boards Manager URLs"
   ```cpp
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
   * Go to **Tools > Board > Boards Manager,** search for "ESP32" and install it.
2. **Install Required Libraries:**
   * In the Arduino IDE, go to **Sketch > Include Library > Manage Libraries.**
   * Search for and install the` MPU9250_WE` library.
3. **Configure the WiFi Credentials:**
   * Replace the `ssid` and `password` values with your WiFi network credentials in the code.
4. **Upload the Code:**
   * Open the project in Arduino IDE.
   * Connect your ESP32 to your PC.
   * Select the appropriate ESP32 board and port from the **Tools** menu.
   * Upload the code to the ESP32.

## Server-Side PHP Script
 To handle the data sent by the ESP32, you will need to set up a PHP server.
## PHP Script Workflow:
1. The script checks for a `POST` request and ensures the ``CHIPID` and `mpudata` fields are present.
2. It generates a filename based on the device's unique `CHIPID` and the current date.
3. The incoming sensor data is timestamped and stored in the file.
4. If successful, a success message is returned. If not, an error message is displayed.

## Code Overview

### MPU_GET Task
The `MPU_GET` task initializes the MPU9250 sensor and continuously collects acceleration and gyroscope data via SPI. The collected data is stored in arrays, and the task prepares the data for transmission to the server.

### HTTP_SEND Task
The `HTTP_SEND` task sends the data collected by `MPU_GET` to the server using HTTP POST requests. It transmits the data over WiFi and handles any reconnection if the WiFi connection is lost.

### Calibration Function
The `calibrate_MPU` function collects calibration data for the accelerometer and gyroscope by taking multiple samples. It computes the bias for both sensors and adjusts the data to eliminate drift.

```cpp
void calibrate_MPU(float acc_bias[], float gyro_bias[]);
```

### WiFi Connection Function
The `connect_to_wifi` function establishes a WiFi connection. If the connection is lost, it attempts to reconnect or restart the ESP32 after a number of failed attempts.
```cpp
void connect_to_wifi(const char* ssid, const char* password);

```
### How to Run
1. **Upload the code** to your ESP32.
2. **Monitor Serial Output:** Open the Serial Monitor in Arduino IDE to view the output, including WiFi connection status, MPU9250 data, and HTTP POST responses.
3. **Set up a PHP server:** Use the provided PHP script to handle data transmission.
4. **View Data:** The motion and velocity data will be sent to the server URL configured in the code.

### Author
Sakshi Mishra


