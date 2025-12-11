// DATA TEMPLATE BLYNK (Isi sesuai Data Blynk kalian) 
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// WiFi Bawaan WokWi
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Pin LED di ESP32 (pin GPIO 2)
int ledPin = 2;

// Virtual Pin dari Blynk (V0)
BLYNK_WRITE(V0)
{
  int value = param.asInt();      // ambil nilai 0/1 dari Blynk
  digitalWrite(ledPin, value);    // Kirim nilai ke LED (1 = ON, 0 = OFF)
  
  // Debug ke Serial Monitor
  Serial.print("LED status: ");
  Serial.println(value ? "ON" : "OFF");
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);   // LED awalnya mati
  
  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Connected to Blynk!");
}

void loop()
{
  Blynk.run();
}