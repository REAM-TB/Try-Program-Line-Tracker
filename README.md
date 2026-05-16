# PID Line Follower (ESP32)

Deskripsi singkat
- Proyek robot line-follower berbasis ESP32 menggunakan sensor array analog dan kontrol PID untuk menjaga robot pada garis.

Fitur
- Pembacaan 16 sensor (array) dan 8 sensor mid untuk deteksi posisi garis.
- Kontrol motor berbasis PWM untuk dua motor (kiri & kanan).
- Implementasi PID (proportional + derivative) sederhana.

Perangkat Keras (pin)
- Sensor selector: `SEL_A` = GPIO25, `SEL_B` = GPIO26
- ADC input (dibaca dari multiplexer pada ADC pins 32..35)
- Motor kiri:
  - `LEFT_EN` = GPIO13 (PWM)
  - `LEFT_FORWARD` = GPIO2
  - `LEFT_BACKWARD` = GPIO4
- Motor kanan:
  - `RIGHT_EN` = GPIO5 (PWM)
  - `RIGHT_FORWARD` = GPIO17
  - `RIGHT_BACKWARD` = GPIO16

Struktur kode (file penting)
- `platformio.ini` — konfigurasi build PlatformIO ([platformio.ini](platformio.ini)).
- `src/main.cpp` — entry point; set `base_speed`, `Kp`, `Kd` dan memanggil fungsi `PID()`.
- `include/bacaSensor.h` dan `src/bacaSensor.cpp` — inisialisasi dan pembacaan sensor; threshold dan bobot sensor didefinisikan di sini.
- `include/motorControl.h` dan `src/motorControl.cpp` — inisialisasi motor dan fungsi pengendalian (forward, stop, setMotorSpeed).
- `include/PIDController.h` dan `src/PIDController.cpp` — perhitungan posisi garis, error, dan koreksi motor menggunakan PID.

Konfigurasi & Parameter penting
- Kecepatan dasar: ubah `base_speed` di `src/main.cpp`.
- Konstantan PID: `Kp` dan `Kd` di `src/main.cpp`.
- Threshold sensor: variabel `treshold` di `src/bacaSensor.cpp` (default 500).
- Bobot sensor tengah: `WeightValue[]` di `src/bacaSensor.cpp` (default {10,20,30,40,50,60,70,80}).
- Posisi tengah yang diasumsikan: `center_position` di `src/PIDController.cpp` (default 45.0).

Build & Upload (PlatformIO)
- Pastikan board tersambung dan `upload_port` sesuai di `platformio.ini` (default COM12).
- Build dan upload dengan PlatformIO (VSCode): pilih environment `esp32doit-devkit-v1` dan tekan Upload.
- Dari terminal, contoh perintah:

```bash
# Build
pio run
# Upload
pio run -t upload
# Monitor serial
pio device monitor -p COM12 -b 115200
```

Penggunaan
1. Letakkan sensor di atas jalur hitam/putih sesuai konfigurasi.
2. Nyalakan robot; monitor serial untuk melihat status sensor, error, dan koreksi motor.
3. Lakukan tuning `Kp` terlebih dahulu (tingkat respons), lalu `Kd` (meredam osilasi).

Tips Tuning & Troubleshooting
- Jika robot lambat bereaksi: naikkan `Kp` sedikit.
- Jika robot berosilasi atau bergetar: tambahkan `Kd`.
- Kalau sensor sering kehilangan jalur: periksa `treshold` dan iluminasi permukaan.
- Untuk memeriksa pembacaan sensor, aktifkan debug print di `bacaSensor.cpp` (sudah ada output serial setiap pembacaan).

Catatan implementasi
- Saat garis hilang, algoritma memberikan koreksi arah berdasarkan `previous_error` (nilai ±40) untuk mencoba menemukan kembali garis.
- PWM menggunakan `ledc` ESP32 dengan resolusi 8-bit dan frekuensi yang didefinisikan di `include/motorControl.h`.

Atribusi & Lisensi
- Kode ini dibuat oleh pembuat proyek; tambahkan lisensi di file jika diperlukan.

---
Jika Anda ingin, saya bisa menambahkan diagram wiring sederhana, contoh nilai tuning (`Kp`, `Kd`) untuk pengujian, atau menambahkan instruksi kalibrasi sensor otomatis.