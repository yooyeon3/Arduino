#include <WiFi.h>
#include <PubSubClient.h>

#define LED 18      // ESP32 LED 핀
bool ledState = false;

// ===== WiFi 설정 =====
const char *ssid = "5층";          // WiFi 이름
const char *password = "48864886"; // WiFi 비밀번호

// ===== MQTT Broker 설정 =====
const char *mqtt_broker = "broker.emqx.io";  
const char *mqtt_topic = "emqx/esp32";     
const char *mqtt_username = "emqx";        
const char *mqtt_password = "public";      
const int mqtt_port = 1883;                

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

// ===== 함수 선언 =====
void connectToWiFi();
void connectToMQTTBroker();
void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);  // LED 초기 상태 OFF

  connectToWiFi();

  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(mqttCallback);

  connectToMQTTBroker();
}

void loop() {
  if (!mqtt_client.connected()) {
    connectToMQTTBroker();
  }
  mqtt_client.loop();
}

// ===== WiFi 연결 =====
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to the WiFi network");
}

// ===== MQTT 연결 =====
void connectToMQTTBroker() {
  while (!mqtt_client.connected()) {
    String client_id = "esp32-client-" + String(WiFi.macAddress());
    Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
    if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
      mqtt_client.subscribe(mqtt_topic);
      mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP32 ^^"); // 연결 확인용 메시지
    } else {
      Serial.print("Failed to connect, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// ===== MQTT 메시지 수신 콜백 =====
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Message: " + message);

  if (message == "on" && !ledState) {
    digitalWrite(LED, HIGH);
    ledState = true;
    Serial.println("LED is turned ON");
  }
  if (message == "off" && ledState) {
    digitalWrite(LED, LOW);
    ledState = false;
    Serial.println("LED is turned OFF");
  }

  Serial.println("-----------------------");
}