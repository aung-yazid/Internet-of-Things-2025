//----------------------------------------Include NodeMCU ESP8266 Library
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//----------------------------------------
// Mengimpor library NodeMCU ESP8266 untuk menangani WiFi dan Web Server

#include "DHT.h"  //--> Memasukkan library untuk sensor DHT 
#define DHTTYPE DHT11 //--> Mendefinisikan tipe sensor DHT yang digunakan (DHT11, DHT21, atau DHT22), di sini yang digunakan adalah DHT11.

#include "HTML.h" //--> Memasukkan isi halaman User Interface (UI) dari web, disimpan di folder yang sama dengan file .ino

#define LEDonBoard 2  //--> Mendefinisikan LED bawaan, digunakan sebagai indikator saat proses koneksi ke router WiFi

//----------------------------------------SSID dan Password WiFi
const char* ssid = "Masukkan Nama WiFi"; //--> Nama WiFi (SSID)
const char* password = "Masukkan Password WiFi"; //--> Password WiFi
//----------------------------------------

ESP8266WebServer server(80);  //--> Server berjalan pada port 80 (port standar HTTP)

const int DHTPin = 5; //--> Pin yang digunakan untuk sensor DHT11 adalah Pin D1 (alias Pin 5 pada NodeMCU)
DHT dht(DHTPin, DHTTYPE); //--> Inisialisasi sensor DHT, dengan pin dan tipe DHT yang telah ditentukan

//----------------------------------------Rutinitas ini akan dijalankan saat membuka alamat IP NodeMCU ESP8266 di browser
void handleRoot() {
 String s = MAIN_page; //--> Membaca konten HTML
 server.send(200, "text/html", s); //--> Mengirim halaman web ke klien
}
//----------------------------------------
//----------------------------------------Prosedur untuk membaca nilai suhu dari sensor DHT11
void handleDHT11Temperature() {
  float t = dht.readTemperature();  //--> Membaca suhu dalam Celsius (default). Jika ingin dalam Fahrenheit, bisa gunakan: float t = dht.readTemperature(true);
  String Temperature_Value = String(t);
 
  server.send(200, "text/plane", Temperature_Value); //--> Mengirim hanya nilai suhu ke permintaan AJAX klien
  
  if (isnan(t)) { //--> Cek apakah pembacaan gagal, jika ya, keluar dari prosedur untuk mencoba lagi.
    Serial.println("Gagal membaca dari sensor DHT!");
  }
  else {
    Serial.print("DHT11 || Suhu : ");
    Serial.print(t);
    Serial.print(" || ");
  }
}
//----------------------------------------
//----------------------------------------Prosedur untuk membaca nilai kelembaban dari sensor DHT11
void handleDHT11Humidity() {
  float h = dht.readHumidity();  //--> Membaca nilai kelembaban
  String Humidity_Value = String(h);
 
  server.send(200, "text/plane", Humidity_Value); //--> Mengirim hanya nilai kelembaban ke permintaan AJAX klien
  if (isnan(h)) {
    Serial.println("Gagal membaca dari sensor DHT!");
  }
  else {
    Serial.print("Kelembaban : ");
    Serial.println(h);
  }
}
//----------------------------------------
//----------------------------------------Setup
void setup(void){
  Serial.begin(9600);  //--> Memulai komunikasi serial dengan baud rate 115200
  delay(500);
  dht.begin();  //--> Memulai pembacaan sensor DHT11
  delay(500);
  
  WiFi.begin(ssid, password); //--> Mulai koneksi ke router WiFi
  Serial.println("");
    
  pinMode(LEDonBoard,OUTPUT); //--> Mengatur arah port LED bawaan ke output
  digitalWrite(LEDonBoard, HIGH); //--> Mematikan LED bawaan
  
  //----------------------------------------Menunggu hingga koneksi berhasil
  Serial.print("Menghubungkan");
  while (WiFi.status() != WL_CONNECTED) { //--> Selama belum terhubung ke WiFi
    Serial.print(".");
    //----------------------------------------Mengedipkan LED bawaan saat proses koneksi ke WiFi
    digitalWrite(LEDonBoard, LOW);
    delay(250);
    digitalWrite(LEDonBoard, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(LEDonBoard, HIGH); //--> Mematikan LED bawaan setelah berhasil terhubung
  //----------------------------------------Jika koneksi berhasil, tampilkan alamat IP di Serial Monitor
  Serial.println("");
  Serial.print("Berhasil terhubung ke : ");
  Serial.println(ssid);
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP()); //--> Menampilkan alamat IP
  //----------------------------------------
  server.on("/", handleRoot); //--> Menangani permintaan ke root ("/"). Ini untuk menampilkan halaman web.
  server.on("/readTemperature", handleDHT11Temperature);  //--> Menangani permintaan untuk membaca suhu
  server.on("/readHumidity", handleDHT11Humidity);  //--> Menangani permintaan untuk membaca kelembaban

  server.begin(); //--> Memulai server
  Serial.println("Server HTTP dimulai");
}
//----------------------------------------
//----------------------------------------Loop
void loop(){
  server.handleClient();  //--> Menangani permintaan klien
}
//----------------------------------------