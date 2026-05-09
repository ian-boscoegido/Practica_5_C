#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MAX30105.h>
#include <spo2_algorithm.h>

// ========== CONFIGURACIÓN WiFi (CÁMBIALAS) ==========
const char* ssid = "TU_WIFI";        // <--- CAMBIA ESTO
const char* password = "TU_CONTRASENA"; // <--- CAMBIA ESTO

// ========== CONFIGURACIÓN OLED ==========
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ========== CONFIGURACIÓN MAX30102 ==========
MAX30105 particleSensor;

// Buffers para el algoritmo (100 muestras)
uint32_t irBuffer[100];
uint32_t redBuffer[100];

// Resultados
int32_t heartRate = 0;
int32_t spo2 = 0;
int8_t validHeartRate = 0;
int8_t validSPO2 = 0;

// Variables para la web
String lastHeartRate = "--";
String lastSpO2 = "--";
unsigned long lastSensorRead = 0;
bool sensorReady = false;

// ========== SERVIDOR WEB ==========
AsyncWebServer server(80);

// ========== PÁGINA WEB (HTML + CSS + JS) ==========
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Pulsioxímetro ESP32</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #1a1a2e 0%, #16213e 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 30px;
            padding: 30px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            text-align: center;
            max-width: 500px;
            width: 100%;
        }
        h1 {
            color: #1a1a2e;
            margin-bottom: 10px;
            font-size: 28px;
        }
        .subtitle {
            color: #666;
            margin-bottom: 30px;
            font-size: 14px;
        }
        .heart-icon {
            font-size: 80px;
            animation: latido 1s infinite;
            display: inline-block;
            margin-bottom: 20px;
        }
        @keyframes latido {
            0% { transform: scale(1); opacity: 1; }
            50% { transform: scale(1.2); opacity: 0.7; }
            100% { transform: scale(1); opacity: 1; }
        }
        .card {
            background: linear-gradient(135deg, #e74c3c 0%, #c0392b 100%);
            border-radius: 20px;
            padding: 25px;
            margin: 20px 0;
            color: white;
        }
        .card-spo2 {
            background: linear-gradient(135deg, #3498db 0%, #2980b9 100%);
        }
        .label {
            font-size: 16px;
            opacity: 0.9;
            letter-spacing: 2px;
        }
        .value {
            font-size: 64px;
            font-weight: bold;
            margin: 10px 0;
        }
        .unit {
            font-size: 24px;
        }
        .status {
            margin-top: 20px;
            padding: 12px;
            border-radius: 10px;
            font-size: 14px;
        }
        .status-good {
            background: #d4edda;
            color: #155724;
            border-left: 4px solid #28a745;
        }
        .status-warning {
            background: #fff3cd;
            color: #856404;
            border-left: 4px solid #ffc107;
        }
        .status-danger {
            background: #f8d7da;
            color: #721c24;
            border-left: 4px solid #dc3545;
        }
        .footer {
            margin-top: 20px;
            font-size: 11px;
            color: #999;
        }
        .refresh {
            font-size: 12px;
            color: #666;
            margin-top: 10px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>❤️ Pulsioxímetro IoT</h1>
        <div class="subtitle">Monitor de salud en tiempo real</div>
        
        <div class="heart-icon">❤️</div>
        
        <div class="card">
            <div class="label">FRECUENCIA CARDÍACA</div>
            <div class="value">
                <span id="hr">--</span>
                <span class="unit">BPM</span>
            </div>
        </div>
        
        <div class="card card-spo2">
            <div class="label">OXÍGENO EN SANGRE</div>
            <div class="value">
                <span id="spo2">--</span>
                <span class="unit">%</span>
            </div>
        </div>
        
        <div id="status" class="status status-warning">
            🟡 Coloca tu dedo en el sensor...
        </div>
        
        <div class="refresh">🔄 Actualización automática cada 2 segundos</div>
        <div class="footer">Conectado a ESP32 | Datos vía I2C</div>
    </div>
    
    <script>
        function actualizarDatos() {
            fetch('/datos')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('hr').innerHTML = data.hr;
                    document.getElementById('spo2').innerHTML = data.spo2;
                    
                    const statusDiv = document.getElementById('status');
                    if(data.hr != '--' && data.spo2 != '--') {
                        if(data.spo2 >= 95 && data.hr >= 60 && data.hr <= 100) {
                            statusDiv.innerHTML = '✅ ✅ ✅ Normal - Todo bien';
                            statusDiv.className = 'status status-good';
                        } else if(data.spo2 < 90) {
                            statusDiv.innerHTML = '⚠️ ⚠️ ⚠️ Bajo nivel de oxígeno - Consulta a un médico';
                            statusDiv.className = 'status status-danger';
                        } else if(data.hr < 60) {
                            statusDiv.innerHTML = '⚠️ Frecuencia cardíaca baja (Bradicardia)';
                            statusDiv.className = 'status status-warning';
                        } else if(data.hr > 100) {
                            statusDiv.innerHTML = '⚠️ Frecuencia cardíaca alta (Taquicardia)';
                            statusDiv.className = 'status status-warning';
                        } else {
                            statusDiv.innerHTML = '✅ Mediciones dentro de rango normal';
                            statusDiv.className = 'status status-good';
                        }
                    } else {
                        statusDiv.innerHTML = '🟡 Esperando datos válidos - Mantén el dedo quieto en el sensor';
                        statusDiv.className = 'status status-warning';
                    }
                });
        }
        
        setInterval(actualizarDatos, 2000);
        actualizarDatos();
    </script>
</body>
</html>
)rawliteral";

// ========== FUNCIONES AUXILIARES ==========

// Conectar a WiFi
void conectarWiFi() {
    Serial.print("Conectando a WiFi");
    WiFi.begin(ssid, password);
    
    int intentos = 0;
    while (WiFi.status() != WL_CONNECTED && intentos < 40) {
        delay(500);
        Serial.print(".");
        intentos++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ WiFi conectado!");
        Serial.print("📡 IP: ");
        Serial.println(WiFi.localIP());
        
        // Mostrar IP en OLED
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("WiFi OK");
        display.print("IP: ");
        display.println(WiFi.localIP());
        display.display();
        delay(2000);
    } else {
        Serial.println("\n❌ Error WiFi - Modo AP");
        WiFi.softAP("ESP32_Pulsioxi", "12345678");
        Serial.print("IP AP: ");
        Serial.println(WiFi.softAPIP());
        
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("MODO AP");
        display.print("IP: ");
        display.println(WiFi.softAPIP());
        display.display();
        delay(2000);
    }
}

// Inicializar OLED
void iniciarOLED() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("❌ Error: OLED no encontrado");
        for(;;);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    Serial.println("✅ OLED iniciado");
}

// Inicializar MAX30102
void iniciarSensor() {
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("❌ Error: MAX30102 no encontrado");
        display.println("Error MAX30102");
        display.display();
        sensorReady = false;
        return;
    }
    
    // Configuración óptima
    byte ledBrightness = 50;
    byte sampleAverage = 4;
    byte ledMode = 2;
    byte sampleRate = 100;
    int pulseWidth = 411;
    int adcRange = 4096;
    
    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
    particleSensor.enableDIETEMPRDY();  // Habilitar temperatura
    
    Serial.println("✅ MAX30102 configurado");
    sensorReady = true;
}

// Inicializar servidor web
void iniciarServidor() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    });
    
    server.on("/datos", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = "{\"hr\":\"" + lastHeartRate + "\",\"spo2\":\"" + lastSpO2 + "\"}";
        request->send(200, "application/json", json);
    });
    
    server.begin();
    Serial.println("✅ Servidor web iniciado en puerto 80");
}

// Mostrar datos en OLED
void mostrarEnOLED() {
    display.clearDisplay();
    
    // Marco decorativo
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.drawRect(2, 2, SCREEN_WIDTH - 4, SCREEN_HEIGHT - 4, SSD1306_WHITE);
    
    // Título
    display.setTextSize(1);
    display.setCursor(25, 8);
    display.println("PULSIOXIMETRO");
    
    // Línea separadora
    display.drawLine(10, 18, SCREEN_WIDTH - 10, 18, SSD1306_WHITE);
    
    // Corazón animado (parpadea según el pulso)
    if(millis() % 1000 < 300) {
        display.setCursor(10, 28);
        display.print("❤️");
    } else {
        display.setCursor(10, 28);
        display.print("🖤");
    }
    
    // Valores
    display.setTextSize(2);
    display.setCursor(30, 28);
    display.print(lastHeartRate);
    display.setTextSize(1);
    display.print(" BPM");
    
    display.setTextSize(2);
    display.setCursor(30, 48);
    display.print(lastSpO2);
    display.setTextSize(1);
    display.print(" %");
    
    display.display();
}

// Leer sensor y calcular
void leerSensor() {
    if(!sensorReady) return;
    
    Serial.println("📊 Leyendo sensor... Coloca tu dedo");
    
    // Mostrar mensaje en OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 25);
    display.println("Coloca tu dedo");
    display.setCursor(10, 40);
    display.println("en el sensor...");
    display.display();
    
    // Tomar 100 muestras
    for (byte i = 0; i < 100; i++) {
        while (!particleSensor.available()) {
            particleSensor.check();
        }
        
        redBuffer[i] = particleSensor.getRed();
        irBuffer[i] = particleSensor.getIR();
        particleSensor.nextSample();
        
        // Barra de progreso en OLED
        if(i % 10 == 0) {
            display.fillRect(10, 55, i, 4, SSD1306_WHITE);
            display.display();
        }
    }
    
    // Calcular
    maxim_heart_rate_and_oxygen_saturation(irBuffer, 100, redBuffer, 
                                           &spo2, &validSPO2, 
                                           &heartRate, &validHeartRate);
    
    // Actualizar variables
    if (validHeartRate && heartRate > 30 && heartRate < 200) {
        lastHeartRate = String(heartRate);
        Serial.print("❤️ HR: ");
        Serial.print(heartRate);
        Serial.println(" BPM");
    } else {
        lastHeartRate = "--";
        Serial.println("⚠️ Pulso no válido");
    }
    
    if (validSPO2 && spo2 > 70 && spo2 < 100) {
        lastSpO2 = String(spo2);
        Serial.print("💨 SpO2: ");
        Serial.print(spo2);
        Serial.println("%");
    } else {
        lastSpO2 = "--";
        Serial.println("⚠️ SpO2 no válido");
    }
    
    Serial.println("---");
}

// ========== SETUP ==========
void setup() {
    Serial.begin(115200);
    delay(100);
    
    Serial.println("\n\n╔════════════════════════════════╗");
    Serial.println("║   PULSIOXÍMETRO IOT - ESP32   ║");
    Serial.println("║       I2C + WiFi + Web        ║");
    Serial.println("╚════════════════════════════════╝\n");
    
    // Inicializar componentes
    iniciarOLED();
    iniciarSensor();
    conectarWiFi();
    iniciarServidor();
    
    // Mensaje final
    Serial.println("\n✅ Sistema listo!");
    Serial.print("📱 Abre tu navegador en: http://");
    Serial.println(WiFi.localIP());
    
    // Mostrar en OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(5, 20);
    display.println("SISTEMA LISTO");
    display.setCursor(5, 35);
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.display();
    delay(3000);
}

// ========== LOOP ==========
void loop() {
    // Leer sensor cada 5 segundos
    if(millis() - lastSensorRead > 5000) {
        leerSensor();
        lastSensorRead = millis();
    }
    
    // Actualizar OLED constantemente
    mostrarEnOLED();
    
    delay(100);
}