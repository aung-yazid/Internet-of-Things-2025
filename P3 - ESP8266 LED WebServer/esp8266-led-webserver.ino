#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "html.h"  // Import file HTML dari header

const char* ssid = "ungs";
const char* password = "wuwungrangers";

ESP8266WebServer server(80);

void setup() {
  pinMode(D1, OUTPUT);
  digitalWrite(D1, LOW);
  pinMode(D2, OUTPUT);
  digitalWrite(D2, LOW);

  Serial.begin(9600);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup routes
  server.on("/", handleRoot);
  server.on("/led-on-1", handleLED1On);
  server.on("/led-off-1", handleLED1Off);
  server.on("/led-on-2", handleLED2On);
  server.on("/led-off-2", handleLED2Off);

  server.begin();
  Serial.println("HTTP server started");
}

void handleRoot() {
  String html = FPSTR(MAIN_page); // Ambil HTML dari PROGMEM
  server.send(200, "text/html", html);
}

void handleLED1On() {
  digitalWrite(D1, HIGH);
  Serial.println("LED 1 is ON");
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLED1Off() {
  digitalWrite(D1, LOW);
  Serial.println("LED 1 is OFF");
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLED2On() {
  digitalWrite(D2, HIGH);
  Serial.println("LED 2 is ON");
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLED2Off() {
  digitalWrite(D2, LOW);
  Serial.println("LED 2 is OFF");
  server.sendHeader("Location", "/");
  server.send(303);
}

void loop() {
  server.handleClient();
}