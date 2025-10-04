const int led = D0;

void setup() {
  Serial.begin(9600);
  pinMode(D0, OUTPUT);
}

void loop() {
  digitalWrite(led, HIGH)
  Serial.println("LED AKTIF");
  delay(1000);

  digitalWrite(led, LOW)
  Serial.println("LED MATI");
  delay(1000);
}