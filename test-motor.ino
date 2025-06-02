#define IN1 32
#define IN2 33
#define IN5 26
#define IN6 25

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);
}

void loop() {
  analogWrite(IN1, 150);
  digitalWrite(IN2, LOW);
  analogWrite(IN5, 150);
  digitalWrite(IN6, LOW);
  delay(2000);

  analogWrite(IN1, 0);
  digitalWrite(IN2, LOW);
  analogWrite(IN5, 0);
  digitalWrite(IN6, LOW);
  delay(2000);
}
