#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include "index.h"  // Include file HTML

// WiFi credentials
const char* ssid = "Nama WiFi";
const char* password = "Password WiFi";

ESP8266WebServer server(80);

// SoftwareSerial untuk komunikasi dengan Arduino
// RX=D6 (GPIO12), TX=D5 (GPIO14)
SoftwareSerial arduinoSerial(D6, D5);

// Variabel untuk menyimpan data sensor
String temperature = "0";
String humidity = "0";
String ledStatus = "OFF";

void setup() {
  Serial.begin(9600);        // Untuk Serial Monitor (debugging)
  arduinoSerial.begin(9600);   // Komunikasi dengan Arduino
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/led/on", handleLEDOn);
  server.on("/led/off", handleLEDOff);
  
  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();
  
  // Baca data dari Arduino
  if (arduinoSerial.available() > 0) {
    String data = arduinoSerial.readStringUntil('\n');
    data.trim();
    
    Serial.print("Received from Arduino: ");
    Serial.println(data);
    
    if (data.startsWith("T:")) {
      int commaIndex = data.indexOf(',');
      if (commaIndex > 0) {
        temperature = data.substring(2, commaIndex);
        humidity = data.substring(commaIndex + 3);
        
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" | Humidity: ");
        Serial.println(humidity);
      }
    }
    else if (data.startsWith("LED:")) {
      ledStatus = data.substring(4);
      Serial.print("LED Status updated: ");
      Serial.println(ledStatus);
    }
  }
}

// Handler untuk halaman utama
void handleRoot() {
  String html = FPSTR(MAIN_page);  // Ambil HTML dari PROGMEM
  server.send(200, "text/html", html);
}

// Handler untuk mendapatkan data sensor (JSON)
void handleData() {
  String json = "{";
  json += "\"temperature\":\"" + temperature + "\",";
  json += "\"humidity\":\"" + humidity + "\",";
  json += "\"led\":\"" + ledStatus + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

// Handler LED ON
void handleLEDOn() {
  arduinoSerial.println("LED_ON");
  Serial.println("Sent to Arduino: LED_ON");
  server.send(200, "text/plain", "LED ON command sent");
}

// Handler LED OFF
void handleLEDOff() {
  arduinoSerial.println("LED_OFF");
  Serial.println("Sent to Arduino: LED_OFF");
  server.send(200, "text/plain", "LED OFF command sent");
}