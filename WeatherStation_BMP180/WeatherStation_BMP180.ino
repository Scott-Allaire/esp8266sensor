#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "wifi-creds.h"
//const char* ssid = "";
//const char* password = "";
//const char* mqtt_host = "";
//const int mqtt_port = 1883;
//const char* topic = "";

#include <Wire.h>
#include <Adafruit_BMP085.h>

WiFiClient espClient;
PubSubClient client(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, -6 * 3600);

Adafruit_BMP085 bmp;

unsigned long timer; // the timer
unsigned long INTERVAL = 15 * 60 * 1000; // the repeat interval

double pressure;
double temp_c;
double temp_f;

char msg[200];

void connectWifi(void) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void connectQueue(char *host, int port) {
  client.setServer(host,port);
  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);    
    
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void readSensor() {
  float p = bmp.readPressure()/100; //millibars
  float t = bmp.readTemperature();

  if (!isnan(p)) pressure = p;
  if (!isnan(t)) {
    temp_c = t;
    temp_f = t * 9/5 + 32;;
  }
}

void sendResults() {
  connectQueue((char*)mqtt_host, mqtt_port);
  timeClient.update();

  String m = "{\"pressure\":" + String(pressure,1) + ","
              "\"tempc\":" + String(temp_c,1) +  ","
              "\"tempf\":" + String(temp_f,1) +  ","
              "\"time\":\"" + timeClient.getFormattedTime() + "\"," +
              "\"epoch\":" + String(timeClient.getEpochTime()) + "}";
  client.publish(topic, m.c_str());
  Serial.println(m.c_str());
}

void setup() {
  timer = millis();
  Serial.begin(9600);
  delay(100);
  connectWifi();
  timeClient.begin();
  
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
  
  timer = millis();

  readSensor();
}

void loop() {
  delay(500);

  if (millis() > timer) {
    readSensor();
    sendResults();
    timer = millis() + INTERVAL;
  }
}

