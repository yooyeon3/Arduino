/******************************************************************************
 * ESP32 + Ultrasonic + MQTT Publish
 ******************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>

// ===== Wi-Fi 설정 =====
const char *ssid     = "5층";
const char *password = "48864886";

// ===== MQTT 설정 =====
const char *MQTT_ID = "student_01";
const char *TOPIC   = "device/01/ultrasonic";

IPAddress broker(192, 168, 0, 87);   // 라즈베리파이 IP
const int MQTT_PORT = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// ===== 초음파 핀 설정 (ESP32 GPIO) =====
const int TRIG_PIN = 23;    // 원하는 GPIO로 변경 가능
const int ECHO_PIN = 22;   // 원하는 GPIO로 변경 가능

// ===== 타이밍 =====
unsigned long previousMillis = 0;
const long interval = 2000;


// ===== Wi-Fi 연결 =====
void setup_wifi() {
  Serial.print("Connecting to ");
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


// ===== 초음파 거리 측정 =====
float measureDistance() {
  long duration;
  float distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout

  if (duration == 0) {
    return -1.0;
  }

  distance = duration * 0.017;

  return distance;
}


void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  delay(100);

  setup_wifi();
  client.setServer(broker, MQTT_PORT);

  Serial.println("ESP32 초음파 MQTT 시작");
  Serial.println("=======================");
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float distance = measureDistance();

    if (distance > 0) {

      char distStr[10];
      dtostrf(distance, 4, 1, distStr);

      client.publish(TOPIC, distStr);

      Serial.print(TOPIC);
      Serial.print(" => ");
      Serial.print(distStr);
      Serial.println(" cm");

    } else {
      Serial.println("측정 실패 (범위 초과)");
    }
  }
}
