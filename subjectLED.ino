

#include <Servo.h>

Servo myservo;

int potpin = A0;
int val;

int redLED = 6;     // 위험
int greenLED = 5;   // 정상

void setup() {
  myservo.attach(9);

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  Serial.begin(9600);   // 확인용
}

void loop() {

  val = analogRead(potpin);         // 0~1023 읽기
  val = map(val, 0, 1023, 0, 180);  // 0~180 변환

  myservo.write(val);

  Serial.println(val);  // 각도 확인

  //  안전 로직
  if (val >= 30 && val <= 150) {
    digitalWrite(greenLED, HIGH);   // 정상 ( 초록 )
    digitalWrite(redLED, LOW);
  }
  else {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);     // 위험  ( 빨간 )
  }

  delay(15);
}
