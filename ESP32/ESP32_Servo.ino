/******************************************************************************
 * step4_servo_mqtt.ino
 ******************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// ===== Wi-Fi 설정 =====
const char *ssid     = "5층";
const char *password = "48864886";

// ===== MQTT 설정 =====
const char *MQTT_ID = "servo_07";               
const char *TOPIC   = "device/07/servo/cmd";    

IPAddress broker(192, 168, 0, 87);   // 라즈베리파이 IP
const int MQTT_PORT = 1883;

WiFiClient wclient;
PubSubClient client(wclient);

// ===== 서보모터 설정 =====
const int SERVO_PIN = 5;   // D5 (GPIO14)
Servo myServo;


// ===== Wi-Fi 연결 =====
void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


// ===== MQTT 메시지 수신 콜백 =====
void callback(char* topic, byte* payload, unsigned int length) {

  // payload → 문자열 변환
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  Serial.print("수신: [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  String cmd = String(message);

  if (cmd == "OPEN") {
    myServo.write(90);
    Serial.println("→ 서보 90도 (OPEN)");
  }
  else if (cmd == "CLOSE") {
    myServo.write(0);
    Serial.println("→ 서보 0도 (CLOSE)");
  }
  else {
    int angle = cmd.toInt();
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("→ 서보 ");
      Serial.print(angle);
      Serial.println("도");
    } else {
      Serial.println("잘못된 명령 (0~180 또는 OPEN/CLOSE)");
    }
  }
}


// ===== MQTT 재연결 =====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(MQTT_ID)) {
      Serial.println("connected");

      //  토픽 구독 (QoS 1)
      client.subscribe(TOPIC, 1);

      Serial.print("Subscribed to: ");
      Serial.println(TOPIC);
      Serial.println();
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

  // ===== 서보 초기화 =====
  myServo.attach(SERVO_PIN);
  myServo.write(0);   // 초기 위치 0도

  delay(100);

  setup_wifi();
  client.setServer(broker, MQTT_PORT);

  // 콜백 등록
  client.setCallback(callback);

  Serial.println("4단계: 서보모터 MQTT 제어");
  Serial.println("=========================");
  Serial.println("명령 대기 중...");
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();   // 메시지 수신 처리
}