#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "SSD1306.h"

#include "wifi-creds.h"

#define DHTPIN  D6
#define DHTTYPE DHT11

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHTPIN, DHTTYPE);
SSD1306  display(0x3c, D3, D4);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, -6 * 3600);

unsigned long timer; // the timer
unsigned long INTERVAL = 15 * 60 * 1000; // the repeat interval

double humidity;
double temp_c;
double temp_f;
double feels_like;

char msg[200];

void connectWifi(void) {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void connectQueue(char *ip) {
  client.setServer(ip,1883);
  
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
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (!isnan(h)) humidity = h;
  if (!isnan(t)) temp_c = t;
  if (!isnan(f)) {
    temp_f = f;
    feels_like = dht.computeHeatIndex(temp_f, humidity, true);
  }
}

void updateDisplay() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, String(humidity, 1) + "%");
  display.drawString(0, 17, String(temp_f, 1) + "°F");
  display.drawString(64, 17, String(feels_like, 1) + "°F");
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 0, "Feels Like");
  display.drawString(0, 50, timeClient.getFormattedTime());
  display.display();  
}

void sendResults() {
  connectQueue("192.168.1.2");
  timeClient.update();

  String m = "{\"hum\":" + String(humidity,1) + ","
              "\"tempc\":" + String(temp_c,1) +  ","
              "\"tempf\":" + String(temp_f,1) +  ","
              "\"feels_like\":" + String(feels_like,1) +  ","
              "\"time\":\"" + timeClient.getFormattedTime() + "\"," +
              "\"epoch\":" + String(timeClient.getEpochTime()) + "}";
  client.publish("topic/weather",m.c_str());
  Serial.println(m.c_str());
}

void setup() {
  timer = millis();
  Serial.begin(115200);
  delay(10);
  connectWifi();
  timeClient.begin();
  
  // Initialize screeen
  display.init();
//  display.flipScreenVertically();

  timer = millis();

  readSensor();
  updateDisplay();
}

void loop() {
  delay(500);

  readSensor();

  if (millis() > timer) {
    sendResults();
    timer = millis() + INTERVAL;
  }

  updateDisplay();
}

