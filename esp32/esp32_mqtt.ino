
#include <WiFi.h>
#include <WebSocketsClient.h>

// ==================================================
// 1. KONFIGURASI WIFI
// ==================================================

const char* WIFI_SSID = "";
const char* WIFI_PASS = "";

// ==================================================
// 2. KONFIGURASI MQTT OVER WEBSOCKET SECURE
// ==================================================

const char* MQTT_HOST = "rise-finder-buffer-variable.trycloudflare.com";
const uint16_t MQTT_PORT = 443;
const char* MQTT_PATH = "/mqtt";

const char* MQTT_TOPIC = "telehealth/test";
const char* MQTT_CLIENT_ID = "ESP32-DevKit-Test";

// ==================================================
// 3. OBJEK WEBSOCKET
// ==================================================

WebSocketsClient ws;

// ==================================================
// 4. VARIABEL
// ==================================================

bool wsConnected = false;
bool mqttConnected = false;

unsigned long lastSend = 0;
unsigned long lastPing = 0;

const unsigned long SEND_INTERVAL = 5000;
const unsigned long PING_INTERVAL = 15000;

int value = 100;

// ==================================================
// 5. FUNGSI ENCODE MQTT REMAINING LENGTH
// ==================================================

size_t encodeRemainingLength(uint8_t* buffer, size_t length) {
  size_t count = 0;

  do {
    uint8_t digit = length % 128;
    length /= 128;

    if (length > 0) {
      digit |= 0x80;
    }

    buffer[count++] = digit;

  } while (length > 0);

  return count;
}

// ==================================================
// 6. FUNGSI KIRIM MQTT CONNECT
// ==================================================

void sendMqttConnect() {

  uint8_t packet[128];
  size_t pos = 0;

  // Protocol Name: MQTT
  packet[pos++] = 0x00;
  packet[pos++] = 0x04;
  packet[pos++] = 'M';
  packet[pos++] = 'Q';
  packet[pos++] = 'T';
  packet[pos++] = 'T';

  // Protocol Level: MQTT 3.1.1
  packet[pos++] = 0x04;

  // Connect Flags: Clean Session
  packet[pos++] = 0x02;

  // Keep Alive: 30 detik
  packet[pos++] = 0x00;
  packet[pos++] = 0x1E;

  // Client ID
  size_t clientIdLength = strlen(MQTT_CLIENT_ID);

  packet[pos++] = (clientIdLength >> 8) & 0xFF;
  packet[pos++] = clientIdLength & 0xFF;

  memcpy(packet + pos, MQTT_CLIENT_ID, clientIdLength);
  pos += clientIdLength;

  // Fixed Header
  uint8_t output[160];

  output[0] = 0x10;

  size_t remainingBytes =
    encodeRemainingLength(output + 1, pos);

  memcpy(output + 1 + remainingBytes, packet, pos);

  ws.sendBIN(output, 1 + remainingBytes + pos);

  Serial.println("Mengirim MQTT CONNECT...");
}

// ==================================================
// 7. FUNGSI MQTT PUBLISH
// ==================================================

bool publishMqtt(const char* topic, const String& payload) {

  if (!mqttConnected || !wsConnected) {
    return false;
  }

  size_t topicLength = strlen(topic);
  size_t payloadLength = payload.length();

  size_t remainingLength =
    2 + topicLength + payloadLength;

  uint8_t packet[512];

  // Fixed Header: PUBLISH QoS 0
  packet[0] = 0x30;

  size_t remainingBytes =
    encodeRemainingLength(packet + 1, remainingLength);

  size_t pos = 1 + remainingBytes;

  // Topic Length
  packet[pos++] = (topicLength >> 8) & 0xFF;
  packet[pos++] = topicLength & 0xFF;

  // Topic
  memcpy(packet + pos, topic, topicLength);
  pos += topicLength;

  // Payload
  memcpy(packet + pos, payload.c_str(), payloadLength);
  pos += payloadLength;

  ws.sendBIN(packet, pos);

  return true;
}

// ==================================================
// 8. FUNGSI WEBSOCKET EVENT
// ==================================================

void webSocketEvent(
  WStype_t type,
  uint8_t* payload,
  size_t length
) {

  switch (type) {

    case WStype_CONNECTED:

      Serial.println();
      Serial.println("WebSocket WSS terhubung");

      wsConnected = true;
      mqttConnected = false;

      sendMqttConnect();

      break;

    case WStype_DISCONNECTED:

      Serial.println("WebSocket terputus");

      wsConnected = false;
      mqttConnected = false;

      break;

    case WStype_BIN:

      if (length >= 2) {

        // MQTT CONNACK
        if ((payload[0] & 0xF0) == 0x20 && length >= 4) {

          if (payload[3] == 0x00) {

            mqttConnected = true;

            lastPing = millis();

            Serial.println("MQTT berhasil terhubung");

          } else {

            mqttConnected = false;

            Serial.print("MQTT gagal, CONNACK code: ");
            Serial.println(payload[3]);
          }
        }

        // MQTT PINGRESP
        if ((payload[0] & 0xF0) == 0xD0) {
          Serial.println("MQTT PINGRESP diterima");
        }
      }

      break;

    case WStype_ERROR:

      Serial.println("WebSocket error");

      break;

    default:

      break;
  }
}

// ==================================================
// 9. SETUP
// ==================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println(" ESP32 MQTT OVER WSS");
  Serial.println("================================");

  // --------------------------------
  // Koneksi WiFi
  // --------------------------------

  Serial.print("Menghubungkan WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi terhubung");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // --------------------------------
  // Koneksi WebSocket Secure
  // --------------------------------

  ws.beginSSL(
    MQTT_HOST,
    MQTT_PORT,
    MQTT_PATH,
    nullptr,
    "mqtt"
  );

  ws.onEvent(webSocketEvent);

  ws.setReconnectInterval(5000);

  Serial.println("Memulai koneksi WSS...");
}

// ==================================================
// 10. LOOP
// ==================================================

void loop() {

  // Proses WebSocket
  ws.loop();

  // Tunggu sampai MQTT terhubung
  if (!wsConnected || !mqttConnected) {
    return;
  }

  // --------------------------------
  // MQTT PINGREQ
  // --------------------------------

  if (millis() - lastPing >= PING_INTERVAL) {

    lastPing = millis();

    uint8_t pingPacket[] = {
      0xC0,
      0x00
    };

    ws.sendBIN(pingPacket, sizeof(pingPacket));

  }

  // --------------------------------
  // MQTT PUBLISH setiap 5 detik
  // --------------------------------

  if (millis() - lastSend >= SEND_INTERVAL) {

    lastSend = millis();

    String payload =
      "{\"device\":\"ESP32-DevKit\",\"value\":" +
      String(value) + "}";

    bool success = publishMqtt(
      MQTT_TOPIC,
      payload
    );

    Serial.print("Publish: ");

    if (success) {
      Serial.println(payload);
    } else {
      Serial.println("GAGAL");
    }

    value++;
  }
}
