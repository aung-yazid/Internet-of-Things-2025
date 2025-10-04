int button = D1;
int nilaibutton;
int count = 0;

int led = D0;

void setup() {
  pinMode(button, INPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  nilaibutton = digitalRead(button);

  if (nilaibutton == HIGH) {
    count++;
    delay(300); // debounce
    if (count == 1) {
      digitalWrite(led, HIGH);
    } else if (count == 2) {
      digitalWrite(led, LOW);
      count = 0;
    }
  }
}