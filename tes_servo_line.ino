// Program to test the moving servo when detecting a line using Pixy2
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

// Pixy2 and Servo Objects
Pixy2 pixy;
Servo servoRoda;
Servo servoLeher;

// Pin and Constants
const int pinServoRoda = 12;
const int pinServoLeher = 27;
const int x_center = 39; // Center of Pixy2 frame
const int max_turn_angle = 50; // Max angle for servoRoda
const int max_leher_angle = 30; // Max angle for servoLeher
const int dead_zone = 5; // Dead zone to prevent small movements
const int moving_avg_size = 5; // Size of moving average filter

// Moving Average Variables
int delta_x_buffer[moving_avg_size];
int buffer_index = 0;
int delta_x_sum = 0;

// Variables for Last Known Values
int last_roda_angle = 90;
int last_leher_angle = 90;
int last_x_r = x_center;
int no_line_count = 0;
const int no_line_threshold = 30;
const int search_step = 5; // Step size for searching (degrees)
const int search_delay = 100; // Delay between search steps (ms)

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  Serial.println("Starting Servo Line Detection Test...");

  // Initialize Pixy2
  pixy.init();
  pixy.line.setMode(LINE_MODE_MANUAL_SELECT_VECTOR);

  // Initialize Servos
  servoRoda.attach(pinServoRoda, 500, 2500);
  servoLeher.attach(pinServoLeher, 500, 2500);
  servoRoda.write(90); // Set to neutral
  servoLeher.write(90);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Servo Line Test"));
  display.display();

  // Initialize Moving Average Buffer
  for (int i = 0; i < moving_avg_size; i++) {
    delta_x_buffer[i] = 0;
  }
}

void loop() {
  pixy.line.getMainFeatures();
  display.clearDisplay();
  display.setCursor(0, 0);

  if (pixy.line.numVectors > 0) {
    no_line_count = 0;

    // Select the best vector (simplified for testing)
    int x_s = pixy.line.vectors[0].m_x0;
    int y_s = pixy.line.vectors[0].m_y0;
    int x_e = pixy.line.vectors[0].m_x1;
    int y_e = pixy.line.vectors[0].m_y1;
    int y_r = (y_s + y_e) / 2;
    int x_r = x_s + ((x_e - x_s) * (y_r - y_s)) / (y_e - y_s);
    last_x_r = x_r;

    // Calculate delta_x and apply moving average
    int delta_x = x_r - x_center;
    delta_x_sum -= delta_x_buffer[buffer_index];
    delta_x_buffer[buffer_index] = delta_x;
    delta_x_sum += delta_x;
    buffer_index = (buffer_index + 1) % moving_avg_size;
    int smoothed_delta_x = delta_x_sum / moving_avg_size;

    // Apply dead zone to prevent small corrections
    int roda_angle;
    if (abs(smoothed_delta_x) < dead_zone) {
      roda_angle = 90; // Straight
    } else {
      roda_angle = constrain(90 - (smoothed_delta_x * max_turn_angle / 40), 0, 180);
    }
    servoRoda.write(roda_angle);

    int leher_angle = constrain(90 + (smoothed_delta_x * max_leher_angle / 40), 60, 120);
    servoLeher.write(leher_angle);

    // Store last known values
    last_roda_angle = roda_angle;
    last_leher_angle = leher_angle;

    // Display on OLED
    display.print(F("x_r: ")); display.println(x_r);
    display.print(F("delta_x: ")); display.println(smoothed_delta_x);
    display.print(F("Servo Roda: ")); display.println(roda_angle);
    display.print(F("Servo Leher: ")); display.println(leher_angle);
    display.display();

    // Serial Monitor for Debugging
    Serial.print("x_r: "); Serial.print(x_r);
    Serial.print(" | delta_x: "); Serial.print(smoothed_delta_x);
    Serial.print(" | Servo Roda: "); Serial.print(roda_angle);
    Serial.print(" | Servo Leher: "); Serial.println(leher_angle);
  } else {
    no_line_count++;
    if (no_line_count < no_line_threshold) {
      // Continue with last known values
      servoRoda.write(last_roda_angle);
      servoLeher.write(last_leher_angle);
      display.println(F("Line lost, continuing..."));
      display.display();
      Serial.println("Line lost, continuing with last values...");
    } else {
      // Active search for the line
      static int search_angle = 90;
      static bool search_direction = true; // true = increasing, false = decreasing
      search_angle += search_direction ? search_step : -search_step;
      if (search_angle >= 120) {
        search_angle = 120;
        search_direction = false;
      } else if (search_angle <= 60) {
        search_angle = 60;
        search_direction = true;
      }
      servoRoda.write(search_angle);
      servoLeher.write(last_leher_angle);
      display.print(F("Searching... Angle: "));
      display.println(search_angle);
      display.display();
      Serial.print("Searching for line at angle: ");
      Serial.println(search_angle);
      delay(search_delay);
    }
  }
}