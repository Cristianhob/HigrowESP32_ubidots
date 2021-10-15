/*
   Codigo de prueba para la placa t-Higrow ESP32
   Written by Cristian Oyarzún


*/

#include "DHT.h"

#define DHTPIN 22     // Pin digital conectado al sensor DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SOILCAPPIN 32
int aire = 3550;
int agua = 1200;

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

  unsigned int hum_suelo = analogRead(SOILCAPPIN);
  hum_suelo = map(hum_suelo, agua, aire, 100, 0);
  //hum_suelo = constrain(hum_suelo, 100, 0);

  Serial.print(F("Hum: "));
  Serial.print(h);
  Serial.print(F("% / Temp: "));
  Serial.print(t);
  Serial.print(F("°C / Sensación termica: "));
  Serial.print(hic);
  Serial.print(F("°C / Humedad suelo: "));
  Serial.print(hum_suelo);
  Serial.println(F("% "));


  delay(2000);
}
