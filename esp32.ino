#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WebServer.h>

// ==== WiFi Config ==== //
const char* ssid = "Moto";
const char* password = "cocomelon";

// ==== Sensor Pins ==== //
#define ONE_WIRE_BUS 21     // DS18B20
#define HEART_SENSOR_PIN 34   // Analog pin for heart sensor

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
WebServer server(80);

// ==== Heart Rate Function ==== //
int readHeartRate() {
  const int samples = 30;
  long total = 0;
  int minVal = 4095, maxVal = 0;

  for (int i = 0; i < samples; i++) {
    int val = analogRead(HEART_SENSOR_PIN);
    total += val;
    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;
    delay(2);
  }

  int avg = total / samples;

  // If signal is too low OR flat, consider idle
  if (avg < 1000 || (maxVal - minVal) < 20) {
    return 0;  // No finger detected
  }

  // Map to estimated BPM and subtract 22
  int bpm = map(avg, 1000, 4095, 60, 140);
  bpm -= 22;
  return bpm < 0 ? 0 : bpm;
}

// ==== Web Handler ==== //
void handleData() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  int heartRate = readHeartRate();

  String json = "{";
  json += "\"temperature\":" + String(tempC, 2) + ",";
  json += "\"heart_rate\":";
  json += (heartRate == 0 ? "null" : String(heartRate));
  json += "}";

  // Print to Serial Monitor for debugging
  Serial.println("Sending JSON: " + json);

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  sensors.begin();
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}
