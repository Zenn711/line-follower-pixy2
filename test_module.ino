// This program is used to test components such as motors, servos, and OLEDs separately.
#include <ESP32Servo.h>
#include <Pixy2.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Motor Driver Pins
#define IN1 32  // Maju
#define IN2 33  // Mundur
#define IN5 26  // Maju
#define IN6 25  // Mundur

// Servo Objects
Servo servoRoda;
Servo servoLeher;

// Pin Definitions
const int pinServoRoda = 12;
const int pinServoLeher = 27;

void setup() {
  // Initialize Motor Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN5, 0);
  analogWrite(IN6, 0);

  // Initialize Serial
  Serial.begin(115200);
  Serial.println("Starting Component Tests...");

  // Initialize Servos
  servoRoda.attach(pinServoRoda, 500, 2500);
  servoLeher.attach(pinServoLeher, 500, 2500);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Testing Components..."));
  display.display();
}

void loop() {
  testMotors();
  delay(3000);
  testServo();
  delay(3000);
  testOLED();
  delay(3000);
}

void testMotors() {
  Serial.println("Testing motors...");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Testing Motors..."));
  display.display();

  controlMotors(100);
  delay(2000);
  stopMotors();
  Serial.println("Motor test complete.");
  display.setCursor(0, 10);
  display.println(F("Motors Done!"));
  display.display();
}

void testServo() {
  Serial.println("Testing servos...");
  display.setCursor(0, 20);
  display.println(F("Testing Servos..."));
  display.display();

  servoRoda.write(60);
  delay(1000);
  servoRoda.write(120);
  delay(1000);
  servoRoda.write(90);
  delay(1000);

  servoLeher.write(60);
  delay(1000);
  servoLeher.write(120);
  delay(1000);
  servoLeher.write(90);
  Serial.println("Servo test complete.");
  display.setCursor(0, 30);
  display.println(F("Servos Done!"));
  display.display();
}

void testOLED() {
  Serial.println("Testing OLED...");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Testing OLED"));
  display.println(F("Display OK!"));
  display.display();
  delay(2000);
  Serial.println("OLED test complete.");
}

void controlMotors(int speed) {
  analogWrite(IN1, speed);
  digitalWrite(IN2, LOW);
  analogWrite(IN5, speed);
  digitalWrite(IN6, LOW);
}

void stopMotors() {
  analogWrite(IN1, 0);
  digitalWrite(IN2, LOW);
  analogWrite(IN5, 0);
  digitalWrite(IN6, LOW);
}