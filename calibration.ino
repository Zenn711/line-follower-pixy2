// This program is used to calibrate the Pixy2 and servo sensors to be more accurate.
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

// Servo and Pixy2 Objects
Pixy2 pixy;
Servo servoRoda;
Servo servoLeher;

// Pin Definitions
const int pinServoRoda = 12;
const int pinServoLeher = 27;

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  Serial.println("Starting Calibration...");

  // Initialize Pixy2
  pixy.init();
  pixy.line.setMode(LINE_MODE_MANUAL_SELECT_VECTOR);

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
  display.println(F("Calibrating..."));
  display.display();
}

void loop() {
  calibratePixy();
  calibrateServo();
  delay(5000); // Wait before restarting calibration
}

void calibratePixy() {
  Serial.println("Calibrating Pixy2...");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Calibrating Pixy2..."));
  display.display();

  pixy.line.setBrightness(50); // Adjust brightness for better detection
  for (int i = 0; i < 10; i++) {
    pixy.line.getMainFeatures();
    Serial.print("Calibration step "); Serial.println(i);
    display.setCursor(0, 10);
    display.print(F("Step: ")); display.println(i);
    display.display();
    delay(500);
  }
  Serial.println("Pixy2 Calibration complete!");
  display.setCursor(0, 20);
  display.println(F("Pixy2 Done!"));
  display.display();
}

void calibrateServo() {
  Serial.println("Calibrating Servos...");
  display.setCursor(0, 30);
  display.println(F("Calibrating Servos..."));
  display.display();

  servoRoda.write(90); // Set to neutral position
  servoLeher.write(90);
  delay(1000);
  Serial.println("Servo calibration set to neutral.");
  display.setCursor(0, 40);
  display.println(F("Servos Done!"));
  display.display();
}