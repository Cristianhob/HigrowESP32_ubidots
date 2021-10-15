/*
   Codigo de prueba para la placa t-Higrow ESP32
   Written by Cristian Oyarzún


*/

#include "DHT.h"

#define DHTPIN 22     // Pin digital conectado al sensor DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println(F("Conexión Higrow ESP32"));
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hic = dht.computeHeatIndex(t, h, false); //sensación termica

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C  Heat index: "));
  Serial.print(hic);
  Serial.println(F("°C "));

  
  delay(2000);
}
