#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is connected to digital pin 2
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  sensors.begin();
  Serial.println("DS18B20 Body Temperature Measurement");
}

void loop() {
  sensors.requestTemperatures(); // Send command to get temperatures
  float temperatureC = sensors.getTempCByIndex(0);

  if (temperatureC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature data");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" °C");

    // Optional: Check if within human body temperature range
    if (temperatureC > 35 && temperatureC < 42) {
      Serial.println("Body temperature in range.");
    } else {
      Serial.println("Out of normal body temp range.");
    }
  }

  delay(1000); // Wait 1 second before next read
}
