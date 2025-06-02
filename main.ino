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

Pixy2 pixy;
Servo servoRoda;
Servo servoLeher;

// Pin and Constants
const int pinServoRoda = 12;
const int pinServoLeher = 27;
const int x_center = 39;
const int max_turn_angle = 50;  // Increased for more aggressive response
const int max_leher_angle = 30;
const int dead_zone = 5;
const int straight_speed = 150;
const int moving_avg_size = 5;
const int search_speed = 100;    // Speed during search
const int search_step = 5;      // Step size for searching (degrees)
const int search_delay = 100;    // Delay between search steps (ms)
const float angle_tolerance = 15.0; // Tolerance for vector angle (degrees)

// Moving Average Variables
int delta_x_buffer[moving_avg_size];
int buffer_index = 0;
int delta_x_sum = 0;

// Variables for Last Known Values
int last_roda_angle = 90;
int last_leher_angle = 90;
int last_speed = 0;
int last_x_r = x_center;
float last_vector_angle = 0.0;  // To store the last calculated vector angle
int no_line_count = 0;
const int no_line_threshold = 30;

// Function to calculate vector angle
float calculateVectorAngle(int x0, int y0, int x1, int y1) {
  float angle_rad = atan2((float)(y1 - y0), (float)(x1 - x0));
  float angle_deg = (angle_rad * 180.0) / PI;
  if (angle_deg < 0) angle_deg += 360.0; // Convert to 0-360 degrees
  return angle_deg;
}

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

  // Initialize Serial and Pixy2
  Serial.begin(115200);
  Serial.println("Initializing Pixy2, Servos, and OLED...");
  pixy.init();
  pixy.line.setMode(LINE_MODE_MANUAL_SELECT_VECTOR); // Manual vector selection

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
  display.println(F("Initializing..."));
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

    // Select the best vector based on length, angle, and position
    int selected_vector = 0;
    float max_length = 0;
    float best_score = -1;
    for (int i = 0; i < pixy.line.numVectors; i++) {
      // Calculate vector length
      float length = sqrt(pow(pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0, 2) +
                          pow(pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0, 2));
      
      // Calculate x_r for position scoring
      int y_r = (pixy.line.vectors[i].m_y0 + pixy.line.vectors[i].m_y1) / 2;
      int x_r = pixy.line.vectors[i].m_x0 + 
                ((pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0) * 
                 (y_r - pixy.line.vectors[i].m_y0)) / 
                (pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0);
      
      // Calculate angle of the vector
      float current_angle = calculateVectorAngle(pixy.line.vectors[i].m_x0, pixy.line.vectors[i].m_y0,
                                               pixy.line.vectors[i].m_x1, pixy.line.vectors[i].m_y1);
      float angle_diff = abs(current_angle - last_vector_angle);
      if (angle_diff > 180) angle_diff = 360 - angle_diff;
      
      // Score based on length, angle consistency, and position
      float score = length * (1.0 - (angle_diff / 180.0)) * 
                    (1.0 - (abs(x_r - last_x_r) / 78.0));
      if (score > best_score) {
        best_score = score;
        selected_vector = i;
        max_length = length;
      }
    }

    // Calculate x_r based on the selected vector
    int x_s = pixy.line.vectors[selected_vector].m_x0;
    int y_s = pixy.line.vectors[selected_vector].m_y0;
    int x_e = pixy.line.vectors[selected_vector].m_x1;
    int y_e = pixy.line.vectors[selected_vector].m_y1;
    int y_r = (y_s + y_e) / 2;
    int x_r = x_s + ((x_e - x_s) * (y_r - y_s)) / (y_e - y_s);
    last_x_r = x_r;
    last_vector_angle = calculateVectorAngle(x_s, y_s, x_e, y_e);

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

    // Adjust speed based on steering angle
    int speed;
    if (abs(smoothed_delta_x) < 10) {
      speed = straight_speed;
    } else if (abs(roda_angle - 90) > 30) {
      speed = 120; // Reduce speed for sharp turns
    } else {
      speed = map(abs(smoothed_delta_x), 0, 40, 150, 100);
      speed = constrain(speed, 100, 255);
    }
    controlMotors(speed);

    // Store last known values
    last_roda_angle = roda_angle;
    last_leher_angle = leher_angle;
    last_speed = speed;

    // Display on OLED
    display.print(F("x_r: ")); display.println(x_r);
    display.print(F("delta_x: ")); display.println(smoothed_delta_x);
    display.print(F("Servo Roda: ")); display.println(roda_angle);
    display.print(F("Servo Leher: ")); display.println(leher_angle);
    display.print(F("Speed: ")); display.println(speed);
    display.print(F("Vectors: ")); display.println(pixy.line.numVectors);
    display.display();

    // Serial Monitor for Debugging
    Serial.print("x_r: "); Serial.print(x_r);
    Serial.print(" | delta_x: "); Serial.print(smoothed_delta_x);
    Serial.print(" | Servo Roda: "); Serial.print(roda_angle);
    Serial.print(" | Servo Leher: "); Serial.print(leher_angle);
    Serial.print(" | Speed: "); Serial.print(speed);
    Serial.print(" | Vectors: "); Serial.println(pixy.line.numVectors);
  } else {
    no_line_count++;
    if (no_line_count < no_line_threshold) {
      // Continue with last known values
      servoRoda.write(last_roda_angle);
      servoLeher.write(last_leher_angle);
      controlMotors(last_speed);
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
      controlMotors(search_speed);
      display.print(F("Searching... Angle: "));
      display.println(search_angle);
      display.display();
      Serial.print("Searching for line at angle: ");
      Serial.println(search_angle);
      delay(search_delay);
    }
  }
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