
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <MQTT.h>

const char* WIFI_SSID = "Samsung";
const char* WIFI_PASS = "rafidedenisa";

const char* MQTT_HOST = "rise-finder-buffer-variable.trycloudflare.com";
const int MQTT_PORT = 443;
const char* MQTT_TOPIC = "telehealth/test";

WiFiClientSecure net;
MQTTClient mqtt(256);
WebSocketsClient ws;

unsigned long lastSend = 0;
int value = 100;

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Menghubungkan WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi terhubung");
  Serial.println(WiFi.localIP());

  // Jangan gunakan kode ini untuk produksi.
  // Ini melewati verifikasi sertifikat TLS.
  net.setInsecure();

  mqtt.begin(MQTT_HOST, MQTT_PORT, net);

  Serial.println("Memulai MQTT WSS...");
}

void loop() {
  if (!mqtt.connected()) {
    Serial.print("Menghubungkan MQTT...");
    if (mqtt.connect("ESP32-DevKit-Test")) {
      Serial.println(" berhasil");
    } else {
      Serial.println(" gagal");
      delay(3000);
      return;
    }
  }

  mqtt.loop();

  if (millis() - lastSend >= 5000) {
    lastSend = millis();

    String payload = "{\"device\":\"ESP32-DevKit\",\"value\":" +
                     String(value) + "}";

    bool ok = mqtt.publish(MQTT_TOPIC, payload);
    Serial.print("Publish: ");
    Serial.println(ok ? payload : "GAGAL");

    value++;
  }
}
