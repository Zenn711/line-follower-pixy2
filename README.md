## Implementation of Ackermann Steering on Prototype Self-driving Car Robot Using Pixy2 Camera

Proyek robot line follower cerdas berbasis **ESP32** dan **kamera Pixy2** yang mampu mengikuti jalur **tikungan tajam** dan **garis putus-putus**, serta beradaptasi ketika garis menghilang sementara.

## Fitur Utama
- Mendeteksi dan mengikuti garis lurus maupun tidak kontinu.
- Manuver belokan tajam dengan kontrol sudut.
- Responsif dan stabil di lintasan kompleks.
- Tampilan informasi real-time di layar OLED.
- Algoritma seleksi vektor terbaik berdasarkan skor.

## Komponen
- ESP32
- Kamera Pixy2
- Driver Motor (H-Bridge)
- Motor DC + Servo
- OLED SSD1306
- Baterai Li-Ion
- Chasis dan roda


---
![Cuplikan layar 2025-06-01 155415](https://github.com/user-attachments/assets/d043776a-9fcc-4475-9511-64a43aa0d607)

### 🔹 `servo_test.ino`
```cpp
#include <ESP32Servo.h>

Servo servoRoda;
Servo servoLeher;

void setup() {
  servoRoda.attach(12);
  servoLeher.attach(27);
}

void loop() {
  for (int i = 60; i <= 120; i += 5) {
    servoRoda.write(i);
    servoLeher.write(180 - i);
    delay(300);
  }
}
