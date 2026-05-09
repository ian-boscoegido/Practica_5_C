# Pulsioxímetro IoT con ESP32 (MAX30102)

Este proyecto implementa un sistema de monitorización biométrica en tiempo real utilizando un ESP32. El dispositivo captura la frecuencia cardíaca (BPM) y la saturación de oxígeno en sangre (SpO2).

## 🚀 Características Principales

- **Sensor Biométrico:** Integración con el sensor MAX30102 para cálculo preciso de SpO2 y frecuencia cardíaca.
- **Visualización Local:** Pantalla OLED SSD1306 (128x64) para lectura instantánea de datos.
- **Conectividad IoT:** Servidor web integrado (ESPAsyncWebServer) para visualizar las constantes vitales desde cualquier navegador en la misma red.
- **Procesamiento de Datos:** Algoritmos de filtrado y cálculo de SpO2 en tiempo real sobre ESP32.

## 🛠 Hardware Requerido

- **Microcontrolador:** ESP32 (DevKit V1)
- **Sensor:** MAX30102 (Pulsioxímetro y ritmo cardíaco)
- **Pantalla:** OLED I2C 128x64 (Driver SSD1306)
- **Conexión:** Interfaz I2C para ambos periféricos

## 📦 Librerías Utilizadas

El proyecto requiere las siguientes dependencias (gestionadas en `platformio.ini`):

- `adafruit/Adafruit SSD1306`
- `adafruit/Adafruit GFX Library`
- `sparkfun/SparkFun MAX3010x Pulse and Proximity Sensor Library`
- `ottowinter/ESPAsyncWebServer-esphome`

## ⚙️ Configuración Inicial

1. Abre el archivo `main.cpp`
2. Busca la sección de **CONFIGURACIÓN WiFi**
3. Sustituye `"TU_WIFI"` y `"TU_CONTRASENA"` por los datos de tu red local
4. Compila y carga el código a tu ESP32

## 🔌 Esquema de Conexión (I2C)

Ambos dispositivos (OLED y MAX30102) comparten el bus I2C:

| Componente | SDA | SCL | VCC | GND |
|:---|:---|:---|:---|:---|
| **ESP32** | GPIO 21 | GPIO 22 | 3.3V | GND |

*Nota: Asegúrate de que las resistencias pull-up del bus I2C estén activas.*

## 🌐 Acceso al Servidor Web

1. Abre el Monitor Serie en PlatformIO tras cargar el código
2. Espera a que el ESP32 se conecte a tu WiFi
3. El sistema imprimirá la dirección IP asignada (ej: `http://192.168.1.XX`)
4. Introduce esa dirección en el navegador de tu móvil o PC para ver el dashboard en tiempo real

---

*Desarrollado para prácticas de sistemas embebidos y computación biomédica.*