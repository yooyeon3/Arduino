
// ===== 초음파 센서 핀 설정 =====
const int TRIG_PIN = 23;   // D1
const int ECHO_PIN = 22;   // D2

// ===== 타이밍 설정 =====
unsigned long previousMillis = 0;
const long interval = 2000;   // 2초마다 측정

// ===== 초음파 거리 측정 함수 =====
float measureDistance() {

  long duration;
  float distance;

  // 1. TRIG LOW 2μs
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // 2. TRIG HIGH 10μs (초음파 발사)
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // 3. ECHO 시간 측정 (30ms timeout)
  duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // 4. 타임아웃이면 실패
  if (duration == 0) {
    return -1.0;
  }

  // 5. 거리 계산 (cm)
  distance = duration * 0.034 / 2;

  return distance;
}
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("1단계: 초음파 센서 테스트");
  Serial.println("========================");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float distance = measureDistance();

    if (distance > 0) {
      Serial.print("거리: ");
      Serial.print(distance, 1);   // 소수점 1자리
      Serial.println(" cm");
    } else {
      Serial.println("측정 실패 (범위 초과)");
    }
  }
}
