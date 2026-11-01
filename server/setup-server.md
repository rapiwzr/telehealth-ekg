# Setup Server MQTT Broker

Dokumentasi instalasi dan konfigurasi Mosquitto MQTT Broker, Nginx Reverse Proxy, dan Cloudflare Quick Tunnel pada server Ubuntu.

## 1. Instalasi Mosquitto

Perbarui package:

```bash
sudo apt update
```

Instal Mosquitto dan client:

```bash
sudo apt install mosquitto mosquitto-clients -y
```

Aktifkan service:

```bash
sudo systemctl enable mosquitto
sudo systemctl start mosquitto
```

Periksa status:

```bash
sudo systemctl status mosquitto
```

## 2. Konfigurasi Mosquitto

Buat file konfigurasi:

```bash
sudo nano /etc/mosquitto/conf.d/telehealth.conf
```

Isi konfigurasi:

```conf
listener 1883 0.0.0.0
protocol mqtt

listener 9001 127.0.0.1
protocol websockets

allow_anonymous true
```

Simpan file, kemudian restart Mosquitto:

```bash
sudo systemctl restart mosquitto
```

Periksa kembali:

```bash
sudo systemctl status mosquitto
```

## 3. Instalasi dan Konfigurasi Nginx

Instal Nginx:

```bash
sudo apt install nginx -y
```

Konfigurasi Nginx digunakan untuk mengarahkan:
- Akses `/` ke aplikasi Laravel.
- Akses `/mqtt` ke Mosquitto WebSocket pada port 9001.

Contoh konfigurasi reverse proxy WebSocket:

```nginx
location /mqtt {
    proxy_pass http://127.0.0.1:9001/;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "Upgrade";
    proxy_set_header Host $host;
    proxy_read_timeout 3600s;
    proxy_send_timeout 3600s;
}
```

Periksa konfigurasi:

```bash
sudo nginx -t
```

Restart Nginx:

```bash
sudo systemctl restart nginx
```

## 4. Cloudflare Quick Tunnel

Cloudflare Tunnel digunakan untuk menyediakan akses publik menuju server tanpa membuka port secara langsung.

Tunnel diarahkan ke layanan Nginx:

```bash
cloudflared tunnel --url http://127.0.0.1:8080 --protocol http2
```

Alamat HTTPS yang dihasilkan digunakan sebagai hostname koneksi ESP32.

## 5. Parameter Koneksi ESP32

| Parameter | Nilai |
|---|---|
| Protocol | MQTT over WSS |
| Port | 443 |
| WebSocket Path | `/mqtt` |
| MQTT Topic | `telehealth/test` |
| Broker | Mosquitto |
| Reverse Proxy | Nginx |
| Tunnel | Cloudflare Quick Tunnel |

## 6. Pengujian MQTT

Jalankan subscriber pada server:

```bash
mosquitto_sub -h 127.0.0.1 -t "telehealth/test" -v
```

Jika ESP32 berhasil mengirim data, pesan akan tampil pada terminal subscriber.

Contoh:

```text
telehealth/test {"device":"ESP32-DevKit","value":100}
```

## 7. Pemeriksaan Service

Periksa Mosquitto:

```bash
sudo systemctl status mosquitto --no-pager
```

Periksa Nginx:

```bash
sudo systemctl status nginx --no-pager
```

---

**Catatan keamanan:** Konfigurasi `allow_anonymous true` digunakan pada lingkungan pengujian ini. Untuk penerapan produksi, gunakan autentikasi MQTT, pembatasan akses, dan validasi sertifikat TLS.
