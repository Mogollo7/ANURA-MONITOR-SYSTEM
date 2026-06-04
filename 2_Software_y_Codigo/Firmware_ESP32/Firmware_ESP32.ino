#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include "BluetoothSerial.h"

#define BME280_SDA 21
#define BME280_SCL 22
#define LED_PIN 2
#define BUZZER_PIN 4
#define BATTERY_PIN 34
#define VOLTAGE_DIVIDER_FACTOR 2.0

#define WIFI_SSID "TU_SSID"
#define WIFI_PASSWORD "TU_PASSWORD"
#define WIFI_TIMEOUT_SECONDS 8
#define WIFI_MAX_RETRIES 15

Adafruit_BME280 bme;
BluetoothSerial ESP32_BT;

float batteryVoltage = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BATTERY_PIN, INPUT);

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("\n=== Anura AI Monitor Iniciando ===");

  iniciarBluetooth();

  iniciarSensorBME280();

  leerBateria();

  conectarWiFi();

  Serial.println("=== Sistema Listo ===");
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  float temperatura = bme.readTemperature();
  float humedad = bme.readHumidity();
  float presion = bme.readPressure() / 100.0F;

  if (isnan(temperatura) || isnan(humedad) || isnan(presion)) {
    Serial.println("ERROR: Lectura inválida del sensor");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
    return;
  }

  Serial.println("--- Lecturas del Sistema ---");
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");

  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.println(" %");

  Serial.print("Presion: ");
  Serial.print(presion);
  Serial.println(" hPa");

  Serial.print("Bateria: ");
  Serial.print(batteryVoltage);
  Serial.println(" V");

  ESP32_BT.print("T:");
  ESP32_BT.print(temperatura, 1);
  ESP32_BT.print(",H:");
  ESP32_BT.print(humedad, 1);
  ESP32_BT.print(",P:");
  ESP32_BT.print(presion, 0);
  ESP32_BT.print(",B:");
  ESP32_BT.println(batteryVoltage, 2);

  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(4000);
}

void iniciarBluetooth() {
  ESP32_BT.begin("Anura_Monitor");
  ESP32_BT.setPin("1234", 4);
  Serial.println("Bluetooth iniciado - Nombre: Anura_Monitor - PIN: 1234");
}

void iniciarSensorBME280() {
  Wire.begin(BME280_SDA, BME280_SCL);

  if (bme.begin(0x76)) {
    Serial.println("Sensor BME280 detectado en direccion 0x76");
  } else if (bme.begin(0x77)) {
    Serial.println("Sensor BME280 detectado en direccion 0x77");
  } else {
    Serial.println("ERROR: No se pudo encontrar un sensor BME280 valido");
    Serial.println("Verifique las conexiones SDA (GPIO 21) y SCL (GPIO 22)");
    while (1) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500);
    }
  }
}

void leerBateria() {
  int adcValue = analogRead(BATTERY_PIN);
  float voltage = (adcValue * 3.3) / 4095.0;
  batteryVoltage = voltage * VOLTAGE_DIVIDER_FACTOR;

  Serial.print("ADC Value: ");
  Serial.print(adcValue);
  Serial.print(" -> Voltage: ");
  Serial.print(batteryVoltage, 2);
  Serial.println(" V");
}

void conectarWiFi() {
  Serial.print("Conectando a WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(WIFI_PS_NONE);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint8_t intentos = 0;
  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED && intentos < WIFI_MAX_RETRIES) {
    delay(500);
    intentos++;
    Serial.print(".");
  }

  unsigned long elapsedSeconds = (millis() - startTime) / 1000;
  Serial.println();
  Serial.print("Tiempo de conexion: ");
  Serial.print(elapsedSeconds);
  Serial.println(" segundos");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi conectado exitosamente");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    int rssi = WiFi.RSSI();
    Serial.print("RSSI: ");
    Serial.print(rssi);
    Serial.println(" dBm");

    if (rssi >= -60) {
      Serial.println("Calidad de senal: Excelente");
    } else if (rssi >= -70) {
      Serial.println("Calidad de senal: Buena");
    } else if (rssi >= -85) {
      Serial.println("Calidad de senal: Regular");
    } else {
      Serial.println("Calidad de senal: Critica - Considere usar antena externa");
    }

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi deshabilitado para ahorrar energia");
  } else {
    Serial.println("ERROR: No se pudo conectar a WiFi");
    Serial.println("Continuando en modo offline");
  }
}