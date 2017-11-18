
#include <DHT.h>
#include <ESP8266WiFi.h>
#include "SSD1306.h"

const char* ssid     = "BrennerPl"; // Your ssid
const char* password = "GoPackGo"; // Your Password

#define DHTPIN  D6
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
SSD1306  display(0x3c, D3, D4);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
   
  // Print the IP address
  Serial.println(WiFi.localIP());

  // Initialize screeen
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
}

void loop() {
   delay(2000);
   
  // put your main code here, to run repeatedly:
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.println(" *F\t");

  // clear the display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 14, "Humidity: " + String(h, 1));
  display.drawString(0, 25, "Temp(C):  " + String(t, 1));
  display.drawString(0, 36, "Temp(F):  " + String(f, 1));
  display.display();
}

