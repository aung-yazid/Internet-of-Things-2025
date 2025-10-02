#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ungs";         // ganti
const char* password = "wuwungrangers"; // ganti

ESP8266WebServer server(80);

// Jika pakai onboard NodeMCU LED: D4 -> GPIO2 (aktif LOW)
const int LED_PIN = 2;      // gunakan 2 untuk GPIO2 (D4)
bool ledActiveLow = true;   // true jika LED menyala saat LOW (onboard NodeMCU)

// status internal
bool ledState = false; // false = OFF, true = ON (logical state)

String pageHTML() {
  String stateText = ledState ? "ON" : "OFF";
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'/><title>ESP8266 LED</title></head><body>";
  html += "<h2>ESP8266 WebServer - Kontrol LED</h2>";
  html += "<p>Status LED: <strong>" + stateText + "</strong></p>";
  html += "<p><a href=\"/on\"><button style='padding:10px 20px;font-size:16px;'>ON</button></a> ";
  html += "<a href=\"/off\"><button style='padding:10px 20px;font-size:16px;'>OFF</button></a></p>";
  html += "<p>IP: " + WiFi.localIP().toString() + "</p>";
  html += "</body></html>";
  return html;
}

void handleRoot() {
  if(!server.authenticate("admin", "iot2025")) {
    return server.requestAuthentication();
  }
  server.send(200, "text/html", pageHTML());
}

void handleOff() {
  if(!server.authenticate("admin", "iot2025")) {
    return server.requestAuthentication();
  }
  // set LED sesuai sifat activeLow
  if (ledActiveLow) digitalWrite(LED_PIN, LOW); else digitalWrite(LED_PIN, HIGH);
  ledState = true;
  server.send(200, "text/html", pageHTML());
}

void handleOn() {
  if(!server.authenticate("admin", "iot2025")) {
    return server.requestAuthentication();
  }
  if (ledActiveLow) digitalWrite(LED_PIN, HIGH); else digitalWrite(LED_PIN, LOW);
  ledState = false;
  server.send(200, "text/html", pageHTML());
}

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(LED_PIN, OUTPUT);
  // set initial OFF state
  if (ledActiveLow) digitalWrite(LED_PIN, HIGH); else digitalWrite(LED_PIN, LOW);
  ledState = false;

  Serial.println();
  Serial.println("Menghubungkan ke WiFi...");
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
    if (millis() - start > 20000) {
      Serial.println();
      Serial.println("Gagal terhubung dalam 20s. Periksa SSID/password.");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi Terhubung!");
    Serial.print("IP Address ESP8266: ");
    Serial.println(WiFi.localIP());
  }

  // routes
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();
  Serial.println("WebServer dimulai.");
}

void loop() {
  server.handleClient();
}
