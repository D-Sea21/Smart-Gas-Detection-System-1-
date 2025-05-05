#include <SoftwareSerial.h>

SoftwareSerial espSerial(7, 8); // RX, TX (Uno → ESP8266)

const int mq6_1 = A0;
const int mq6_2 = A1;
const int mq6_3 = A2;

const int buzzer = 2;
const int led1 = 9;
const int led2 = 10;
const int led3 = 11;

const int GAS_THRESHOLD = 600;

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);  // Giao tiếp với ESP

  pinMode(buzzer, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  delay(3000);  // Cho ESP ổn định
}

void loop() {
  int v1 = analogRead(mq6_1);
  int v2 = analogRead(mq6_2);
  int v3 = analogRead(mq6_3);

  String data = "MQ6_1:" + String(v1) + "|MQ6_2:" + String(v2) + "|MQ6_3:" + String(v3) + "\n";
  espSerial.print(data);    // gửi cho ESP
  Serial.print(data);       // hiển thị ra máy tính

  bool alert = false;

  if (v1 > GAS_THRESHOLD) {
    digitalWrite(led1, HIGH);
    alert = true;
  } else digitalWrite(led1, LOW);

  if (v2 > GAS_THRESHOLD) {
    digitalWrite(led2, HIGH);
    alert = true;
  } else digitalWrite(led2, LOW);

  if (v3 > GAS_THRESHOLD) {
    digitalWrite(led3, HIGH);
    alert = true;
  } else digitalWrite(led3, LOW);

  if (alert) {
    tone(buzzer, 100);
  } else {
    noTone(buzzer);
  }

  delay(500);
}
