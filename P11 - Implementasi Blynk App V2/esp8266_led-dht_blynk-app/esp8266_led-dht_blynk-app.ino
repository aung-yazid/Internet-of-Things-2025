#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

#define DHTPIN D2
#define DHTTYPE DHT11

char auth[] = "";
char ssid[] = "";
char pass[] = "";

DHT dht(DHTPIN, DHTTYPE);
int value;

BLYNK_WRITE(V0) {
  value = param.asInt();
  Serial.println(value);
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(D1, OUTPUT);
  dht.begin();
}

void loop() {
  Blynk.run();
  digitalWrite(D1, value);
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  Serial.print("Suhu :");
  Serial.println(temperature);
  Serial.print("Kelembapan :");
  Serial.println(humidity);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Gagal membaca data dari sensor DHT11!");
    return;
  }
  
  Blynk.virtualWrite(V5, humidity);
  Blynk.virtualWrite(V6, temperature);
  delay(2000);
}