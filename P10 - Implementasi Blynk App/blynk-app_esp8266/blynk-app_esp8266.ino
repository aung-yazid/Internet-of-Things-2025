// DATA TEMPLATE BLYNK (Isi sesuai punya kalian) 
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME "LED Controlling"
#define BLYNK_AUTH_TOKEN "eRxEHRZObJwV5H1ghcmDNa0HulY5cNuA"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// ---- GANTI DENGAN WIFI ANDA ----
char ssid[] = "ungs";
char pass[] = "wuwungrangers";

// Pin LED di ESP8266 (pin D2)
int ledPin = D2;
int ledPin_2 = D3;

// Virtual Pin dari Blynk (V0)
BLYNK_WRITE(V0)
{
  int value = param.asInt();      // ambil nilai 0/1 dari Blynk
  digitalWrite(ledPin, value);    // Kirim nilai ke LED (1 = ON, 0 = OFF)
  
  // Debug ke Serial Monitor
  Serial.print("LED status: ");
  Serial.println(value ? "ON" : "OFF");
}

// Virtual Pin dari Blynk (V0)
BLYNK_WRITE(V1)
{
  int value = param.asInt();      // ambil nilai 0/1 dari Blynk
  digitalWrite(ledPin_2, value);    // Kirim nilai ke LED (1 = ON, 0 = OFF)
  
  // Debug ke Serial Monitor
  Serial.print("LED 2 status: ");
  Serial.println(value ? "ON" : "OFF");
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin_2, OUTPUT);
  digitalWrite(ledPin, LOW);   // LED awalnya mati
  
  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Connected to Blynk!");
}

void loop()
{
  Blynk.run();
}
