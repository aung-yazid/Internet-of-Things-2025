const int ledMerah = 22; //Variabel Inisialisasi Pin
const int ledKuning = 4; //Variabel Inisialisasi Pin
const int ledHijau = 2; //Variabel Inisialisasi Pin

void setup() {
  pinMode(ledMerah, OUTPUT); //Iniasliasi Pin Sebagai Output
  pinMode(ledKuning, OUTPUT); //Iniasliasi Pin Sebagai Output
  pinMode(ledHijau, OUTPUT); //Iniasliasi Pin Sebagai Output
  Serial.begin(9600); //Output didalam software Serial Monitor
}

void loop() {
  //Kondisi Lampu Merah Nyala
  digitalWrite(ledMerah, HIGH); //Logika LED Merah Hidup
  digitalWrite(ledKuning, LOW); //Logika LED Kuning Mati
  digitalWrite(ledHijau, LOW); //Logika LED Hijau Mati
  Serial.println("LED MERAH NYALA"); //Memunculkan Data String dalam Serial Monitor
  delay(4000); //Delay sebelum menjalankan fungsi selanjutnya

  //Kondisi Lampu Kuning Nyala
  digitalWrite(ledMerah, LOW); //Logika LED Merah Mati
  digitalWrite(ledKuning, HIGH); //Logika LED Kuning Hidup
  digitalWrite(ledHijau, LOW); //Logika LED Hijau Mati
  Serial.println("LED KUNING NYALA"); //Memunculkan Data String dalam Serial Monitor
  delay(1000); //Delay sebelum menjalankan fungsi selanjutnya

  //Kondisi Lampu Hijau Nyala
  digitalWrite(ledMerah, LOW); //Logika LED Merah Mati
  digitalWrite(ledKuning, LOW); //Logika LED Kuning Mati
  digitalWrite(ledHijau, HIGH); //Logika LED Hijau Hidup
  Serial.println("LED HIJAU NYALA"); //Memunculkan Data String dalam Serial Monitor
  delay(1000); //Delay sebelum menjalankan fungsi selanjutnya

  //Kondisi Lampu Kuning Nyala yang kedua
  digitalWrite(ledMerah, LOW); //Logika LED Merah Mati
  digitalWrite(ledKuning, HIGH); //Logika LED Kuning Hidup
  digitalWrite(ledHijau, LOW); //Logika LED Hijau Mati
  Serial.println("LED KUNING NYALA"); //Memunculkan Data String dalam Serial Monitor
  delay(1000); //Delay sebelum menjalankan fungsi selanjutnya
}