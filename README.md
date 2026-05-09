# ❤️ ESP32 Smart Pulse Oximeter

Real-time biometric monitoring system based on ESP32 for heart rate and blood oxygen saturation measurement using the MAX30102 sensor, including local OLED visualization and wireless web monitoring.

---

# 📖 Overview

This project implements an IoT pulse oximeter capable of measuring:

* Heart Rate (BPM)
* Blood Oxygen Saturation (SpO2)

The system uses an ESP32 as the main controller and processes biometric data obtained from the MAX30102 sensor. Measurements are displayed locally on an OLED screen and remotely through a web interface hosted directly on the ESP32.

---

# ⚡ Core Features

## 🫀 Biometric Monitoring

* Heart rate detection in real time
* SpO2 calculation using MAX30102
* Continuous sensor sampling and processing

## 🖥️ OLED Display

* Live visualization of BPM and oxygen saturation
* SSD1306 128x64 OLED support
* I2C communication interface

## 🌐 Wireless Web Interface

* Embedded asynchronous web server
* Real-time access from devices connected to the same WiFi network
* Browser-based monitoring dashboard

## ⚙️ Signal Processing

* Real-time biometric data filtering
* Pulse detection algorithms
* Oxygen saturation estimation

---

# 🛠️ Hardware Components

| Component           | Description                          |
| ------------------- | ------------------------------------ |
| **ESP32 DevKit V1** | Main microcontroller                 |
| **MAX30102**        | Pulse oximeter and heart rate sensor |
| **OLED SSD1306**    | 128x64 I2C display                   |
| **WiFi Network**    | Remote monitoring access             |

---

# 📚 Libraries & Dependencies

The following libraries are required for compilation:

| Library                | Function                      |
| ---------------------- | ----------------------------- |
| `Adafruit SSD1306`     | OLED display control          |
| `Adafruit GFX Library` | Graphic rendering             |
| `SparkFun MAX3010x`    | MAX30102 sensor communication |
| `ESPAsyncWebServer`    | Asynchronous web server       |

---

# 🔌 I2C Wiring Configuration

The OLED display and MAX30102 sensor share the same I2C communication bus.

| Signal  | ESP32 Pin |
| ------- | --------- |
| **SDA** | GPIO 21   |
| **SCL** | GPIO 22   |
| **VCC** | 3.3V      |
| **GND** | GND       |

> Ensure that the I2C pull-up resistors are enabled for stable communication.

---

# ⚙️ Initial Setup

1. Open the `main.cpp` source file
2. Locate the WiFi configuration section
3. Replace the default SSID and password with your local network credentials
4. Compile and upload the project to the ESP32 board

---

# 🌐 Accessing the Web Dashboard

After uploading the firmware:

1. Open the Serial Monitor
2. Wait until the ESP32 connects to the WiFi network
3. The assigned local IP address will appear in the console
4. Open the displayed IP address in a browser

Example:

```cpp id="9dzd7t"
http://192.168.1.XX
```

The webpage will display real-time BPM and SpO2 readings directly from the sensor.

---

# 📂 Project Architecture

```bash id="ih11kw"
project/
│
├── src/
│   └── main.cpp
│
├── include/
│
├── lib/
│
├── platformio.ini
│
└── README.md
```

---

# ✅ Expected Results

* Successful biometric data acquisition
* Stable BPM and SpO2 measurements
* OLED real-time visualization
* Functional wireless monitoring through web browser
* Correct ESP32 WiFi connectivity

---

# 🔮 Future Improvements

Potential extensions for the project:

* Data logging and storage
* MQTT or cloud integration
* Mobile application support
* Historical graph visualization
* Battery-powered portable version

---

# 📚 Applications

This system can be adapted for:

* Educational biomedical projects
* IoT healthcare monitoring
* Embedded sensor systems
* Remote physiological monitoring
* Smart wearable prototypes

---

*Embedded systems project focused on biomedical sensing and wireless IoT monitoring using ESP32.*
