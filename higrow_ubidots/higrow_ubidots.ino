/*
   Codigo de prueba para la placa t-Higrow ESP32
   Written by Cristian Oyarzún
  PLACA HIGROW ESP32
  sensor dht11        pin 22
  Sensor capacitivo   pin 32
  Led azul            pin 16

  API Ubidots
  https://ubidots.com/docs/hw/#mqtt

  PENDIENTE
  - SUBIR PARAMETROS A UBIDOTS
  - AGREGAR PAREMETRO DE ID
  - AGREGAR NIVEL DE VOLTAJE BATERIA
  - DOCUMENTAR, REDACTAR README EN GITHUB
  - INTEGRAR MODO SLEEP

*/


#include <WiFi.h>         //https://www.arduino.cc/en/Reference/WiFiClientConnected
#include <PubSubClient.h> //https://pubsubclient.knolleary.net/api
#include "DHT.h"          //http://www.codigoelectronica.com/blog/libreria-dht-arduino#computeHeatIndex

//PARAMETROS DE CONEXION A INTERNET//
const char* ssid = "HartTech";
const char* password = "harttech";

const char* mqtt_server = "industrial.api.ubidots.com";
#define mqtt_port 1883
const char topic[] = "/v1.6/devices/test";
String device = "sensor_test";
String data_mqtt = "";
char mensaje[126] = "";
char vacio[] = "";
unsigned int len_msg = 0;

WiFiClient espClient;
PubSubClient client(espClient);

#define DHTPIN 22     // Pin digital conectado al sensor DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SOILCAPPIN 32
int aire = 3550;
int agua = 1200;

#define BUILTIN_LED 16

unsigned long lastMsg = 0;

String Humedad = "hum";
String Temperatura = "temp";
String Suelo = "hum_suelo";
String Sensacion_termica = "sen_term";
String Hum;
String Temp;
String Cap;
String Sensa;


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    const char username[] = "BBFF-0wdH6q6HmvTut4KSa2E63xGYyTRzZj";
    const char pass[] = " ";

    // Attempt to connect
    if (client.connect(clientId.c_str(), username, pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic, "{\"sensor_test\": {\"value\" : 1}}");
      // ... and resubscribe
      client.subscribe("/v1.6/devices/test/test/lv");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("Conexión Higrow ESP32"));
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {
  //Chequea conexión a broker MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //Captura valores desde el sensor DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hic = dht.computeHeatIndex(t, h, false); //sensación termica

  //Captura dato sensor capacitivo
  unsigned int hum_suelo = analogRead(SOILCAPPIN);
  hum_suelo = map(hum_suelo, agua, aire, 100, 0);
  //  Serial.print(F("Hum: "));
  //  Serial.print(h);
  //  Serial.print(F("% / Temp: "));
  //  Serial.print(t);
  //  Serial.print(F("°C / Sensación termica: "));
  //  Serial.print(hic);
  //  Serial.print(F("°C / Humedad suelo: "));
  //  Serial.print(hum_suelo);
  //  Serial.println(F("% "));
  strcpy(mensaje, vacio);
  String value = String(random(0, 100));
  Hum = String(h);
  Temp = String(t);
  Cap = String(hum_suelo);
  Sensa = String(hic);
    data_mqtt = "{\"sensor_test\": {\"value\": 200},\"hum\":{\"value\": 60},\"temp\":{\"value\": 60},\"hum_suelo\":{\"value\": 60},\"sen_term\":{\"value\": 60}}";
//  data_mqtt = "{\"";
//  data_mqtt += "hum";//Suelo;
//  data_mqtt += "\":{value:";
//  data_mqtt += Hum;
//  data_mqtt += "},\"";
//  data_mqtt += "temp";
//  data_mqtt += "\":{value:";
//  data_mqtt += Temp;
//  data_mqtt += "}";
//  data_mqtt += "}";
  
  len_msg = data_mqtt.length() + 1;
  data_mqtt.toCharArray(mensaje, len_msg);
  Serial.print(len_msg);
  Serial.print(" - ");
  Serial.println(sizeof(mensaje));


  unsigned long now = millis();
  if (now - lastMsg > 6000) {
    lastMsg = now;
    Serial.print(data_mqtt);
    Serial.print(" - ");
    Serial.println(mensaje);
    client.publish(topic, mensaje);
  }

  delay(6000);
}
