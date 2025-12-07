#include <DHT.h>  
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

String apiKey = "TYI8K4DI66C1IKIO";     
const char *ssid =  "ungs";    
const char *pass =  "wuwungrangers";
const char *server = "api.thingspeak.com";

unsigned long myChannelNumber = 3187230;
const char *myWriteAPIKey = "TYI8K4DI66C1IKIO"; 

#define DHTPIN D2         
DHT dht(DHTPIN, DHT11);

WiFiClient client;

void setup() {
  Serial.begin(9600);
  delay(10);
  dht.begin();
  Serial.println("Connecting ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  ThingSpeak.begin(client);
}
 
void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Sensor Tidak Dapat Mendeteksi");
    return;
  }

  Serial.println(t);
  Serial.println(h);
  
  ThingSpeak.writeField(myChannelNumber, 1, t, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 2, h, myWriteAPIKey);
  delay(100);
}