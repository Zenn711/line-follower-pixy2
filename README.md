# Implementation of Ackermann Steering on Prototype Self-driving Car Robot Using Pixy2 Camera

## Pendahuluan

Proyek ini merupakan implementasi dari sistem kemudi **Ackermann Steering** pada sebuah prototipe **robot mobil self-driving** yang dirancang untuk mampu mengikuti jalur secara mandiri. Sistem ini memanfaatkan **kamera Pixy2** sebagai sensor utama untuk mendeteksi garis lintasan, dengan bantuan mikrokontroler **ESP32** sebagai otak pemroses logika serta kontrol aktuator.

Tujuan utama proyek ini adalah mengembangkan robot mobil yang mampu:
- Mendeteksi dan mengikuti jalur garis, baik lurus, melengkung, maupun putus-putus.
- Melakukan manuver tikungan tajam dengan perhitungan sudut belok yang sesuai.
- Menyesuaikan kecepatan dan arah secara real-time berdasarkan kondisi jalur.
- Tetap bergerak stabil bahkan ketika kehilangan garis untuk sementara waktu.

Dengan kombinasi algoritma seleksi vektor dari Pixy2 dan sistem kemudi Ackermann, robot ini mampu melakukan navigasi otomatis dengan respons yang lebih halus dan realistis seperti kendaraan sungguhan.

---

## Cara Kerja Program

Program utama yang berjalan pada ESP32 bertugas untuk:

1. **Inisialisasi Komponen:** Termasuk Pixy2, OLED, motor driver, dan dua buah servo (untuk kemudi dan kepala/leher).
2. **Deteksi Jalur:** Mengambil data garis dari Pixy2 dalam bentuk vektor, lalu memilih vektor terbaik berdasarkan panjang, sudut, dan posisi relatif.
3. **Perhitungan Sudut Belok:** Menghitung penyimpangan posisi garis dari pusat kamera dan mengonversinya menjadi sudut belok servo roda.
4. **Penyesuaian Kecepatan:** Menyesuaikan kecepatan motor berdasarkan tajamnya belokan untuk menghindari tergelincir atau keluar jalur.
5. **Koreksi Garis Hilang:** Jika garis tidak terdeteksi, robot akan tetap berjalan dengan data terakhir sambil melakukan pencarian arah secara bertahap.
6. **Monitoring OLED & Debug Serial:** Menampilkan informasi real-time seperti sudut belok, posisi garis, dan kecepatan di OLED dan Serial Monitor.

---

## Blok Diagram Sistem

Berikut ini adalah blok diagram dari sistem robot mobil self-driving yang dikembangkan:


![Cuplikan layar 2025-06-03 005802](https://github.com/user-attachments/assets/6c33540e-ca96-4ee8-931c-44f45a9d441e)

Blok diagram terdiri dari:
- Kamera Pixy2 sebagai input visual utama.
- ESP32 sebagai unit pemrosesan dan kontrol.
- Motor DC + Driver Motor untuk penggerak utama.
- Servo Motor (Kemudi & Leher) untuk belokan.
- OLED Display sebagai output visual real-time.

---

## 🔌 Skematik Rangkaian

![Cuplikan layar 2025-06-03 005721](https://github.com/user-attachments/assets/f7162979-6ffd-4abb-a47c-144213232b36)

![Cuplikan layar 2025-06-03 005735](https://github.com/user-attachments/assets/6cbf6565-6d77-4889-bf8d-c6e0ac61dd65)

![Gambar WhatsApp 2025-06-02 pukul 20 46 51_1623cf28](https://github.com/user-attachments/assets/d2bdac33-764d-4d93-834e-b690a36252ea)

Gambar ini menunjukkan hubungan antara ESP32, Pixy2, driver motor, OLED, dan servo. Pastikan mencantumkan pin yang digunakan dan arah koneksi daya serta sinyal.

---

## Desain 3D dan Fisik Robot

![Gambar WhatsApp 2025-06-01 pukul 12 49 02_99ff7686](https://github.com/user-attachments/assets/01fddf2d-e415-4ccf-a285-d2515429c572)

![Gambar WhatsApp 2025-06-01 pukul 12 50 25_42d03837](https://github.com/user-attachments/assets/f15f24d9-25a2-495c-9c68-b5e1c1060902)


Tampilan ini memperlihatkan wujud fisik dari robot self-driving yang dilengkapi kamera di bagian depan dan mekanisme roda depan berkemudi (mirip seperti mobil sungguhan).

---

## 🎥 Demo Video


🎬 [Berikut adalah video demonstrasi robot](https://github.com/username/repo-link-to-video)

Video ini menunjukkan kemampuan robot dalam mengikuti jalur yang berkelok, putus-putus, hingga mampu kembali ke lintasan saat kehilangan garis secara sementara.

