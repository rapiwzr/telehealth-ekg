# Konfigurasi MQTT Broker

## 1. MQTT Broker
Broker yang digunakan adalah Mosquitto MQTT Broker yang berjalan pada server kampus.

## 2. Konfigurasi Protokol
Komunikasi antara ESP32 dan broker menggunakan MQTT over WebSocket Secure (WSS).

- Protocol: MQTT
- Transport: WebSocket Secure (WSS)
- Port publik: 443
- WebSocket path: `/mqtt`
- MQTT Topic: `telehealth/test`

## 3. Integrasi Cloudflare Tunnel
Cloudflare Quick Tunnel digunakan untuk meneruskan koneksi WSS dari ESP32 melalui internet menuju server kampus.

## 4. Routing Koneksi
Nginx meneruskan koneksi pada path `/mqtt` menuju layanan WebSocket Mosquitto pada port 9001.

## 5. Pengujian
Pengujian dilakukan dengan mengirimkan data dummy dari ESP32 dan memantau pesan menggunakan MQTT subscriber pada server.

**Hasil:** ESP32 berhasil mengirim data melalui WSS dan pesan berhasil diterima oleh subscriber Mosquitto.
