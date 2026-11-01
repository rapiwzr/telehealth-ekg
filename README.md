# Konfigurasi Protokol Komunikasi dan MQTT Broker

## Deskripsi Proyek

Repository ini berisi dokumentasi progres pekerjaan pada bagian konfigurasi protokol komunikasi dan MQTT Broker untuk sistem Telehealth EKG.

Fokus pekerjaan meliputi implementasi komunikasi MQTT over WebSocket Secure (WSS), integrasi Cloudflare Tunnel, dan konfigurasi Mosquitto MQTT Broker pada server kampus.

## Arsitektur Komunikasi

ESP32 → Cloudflare Tunnel → Nginx → Mosquitto MQTT Broker → MQTT Subscriber

## Teknologi yang Digunakan

- ESP32 DevKit
- MQTT Protocol
- WebSocket Secure (WSS)
- Mosquitto MQTT Broker
- Cloudflare Quick Tunnel
- Nginx
- Python MQTT Client untuk pengujian

## Progres Pengerjaan

- [x] Konfigurasi Mosquitto MQTT Broker.
- [x] Konfigurasi WebSocket pada Mosquitto.
- [x] Integrasi Cloudflare Quick Tunnel.
- [x] Konfigurasi routing WebSocket melalui Nginx.
- [x] Implementasi koneksi MQTT over WSS pada ESP32.
- [x] Pengiriman data dummy dari ESP32.
- [x] Pengujian penerimaan data menggunakan MQTT Subscriber.

## Hasil Pengujian

ESP32 berhasil terhubung ke MQTT Broker melalui Cloudflare Tunnel menggunakan protokol MQTT over WSS.

Data dummy berhasil dikirim melalui topic:

`telehealth/test`

Pesan berhasil diterima oleh MQTT Subscriber yang berjalan pada server kampus.

## Dokumentasi

- `esp32/esp32_mqtt.ino` — Program ESP32 untuk komunikasi MQTT over WSS.
- `konfigurasi/konfigurasi-broker.md` — Dokumentasi konfigurasi broker dan protokol.

## Status

**Progres:** Implementasi komunikasi ESP32 dengan MQTT Broker berhasil diuji.
