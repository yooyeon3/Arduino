/******************************************************************************
 * step3_temperature_mqtt.ino
 * 3단계: 온도 조건부 MQTT 퍼블리시 (QoS 0)
 ******************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ===== Wi-Fi 설정 =====
const char *ssid     = "5층";
const char *password = "48864886";

// ===== MQTT 설정 =====
const char *MQTT_ID = "temperature_07";
const char *TOPIC   = "device/07/temperature";

IPAddress broker(192, 168, 0, 87);   // 라즈베리파이 IP
const int MQTT_PORT = 1883;

WiFiClient wclient;
PubSubClient client(wclient);

// ===== DHT 센서 설정 =====
#define DHT_PIN   4
#define DHT_TYPE  DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// ===== 타이밍 설정 =====
unsigned long previousMillis = 0;
const long interval = 2000;   // 2초

// ===== 조건부 전송 설정 =====
const float THRESHOLD_TEMP = 30.0;   // 30도 이상일 때만 전송

// ===== 전송 조건 판단 함수 =====
bool shouldPublish(float temperature) {
  if (temperature >= THRESHOLD_TEMP) {
    return true;
  }
  return false;
}

// ===== Wi-Fi 연결 =====
void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// ===== MQTT 재연결 =====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(MQTT_ID)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  delay(100);
  setup_wifi();
  client.setServer(broker, MQTT_PORT);

  Serial.println("3단계: 온도 조건부 MQTT 퍼블리시");
  Serial.println("=================================");
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    // ===== 온도 읽기 =====
    float temperature = dht.readTemperature();

    // ===== 센서 읽기 실패 처리 =====
    if (isnan(temperature)) {
      Serial.println("DHT 센서 읽기 실패!");
      return;
    }

    // ===== 조건 확인 후 전송 =====
    if (shouldPublish(temperature)) {

      char tempStr[10];
      dtostrf(temperature, 4, 1, tempStr);

      client.publish(TOPIC, tempStr);

      Serial.print("[전송] ");
      Serial.print(TOPIC);
      Serial.print(" => ");
      Serial.println(tempStr);

    } else {

      Serial.print("[스킵] 온도: ");
      Serial.print(temperature);
      Serial.println(" ℃ - 조건 미충족");

    }
  }
}