#define BLYNK_TEMPLATE_ID "TMPL6hEwvepSL"
#define BLYNK_TEMPLATE_NAME "Gas Monitor 13"
#define BLYNK_AUTH_TOKEN "SaIT2Wp_Y0cRP0IfFIBH39D6P2rnYV2_"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Dùng Serial chính để nhận từ Arduino Uno
#define unoSerial Serial

char ssid[] = "Bean Coffee";
char pass[] = "camonquykhach";

void setup() {
  unoSerial.begin(9600);
  delay(1000);

  unoSerial.println("ESP8266 Starting...");

  WiFi.begin(ssid, pass);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    unoSerial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    unoSerial.println("\nWiFi connected.");
  } else {
    unoSerial.println("\nWiFi failed!");
  }

  // Dùng config để không bị kẹt
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();  // chỉ gọi khi Wi-Fi đã sẵn sàng
}

void checkConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    unoSerial.println("WiFi disconnected! Reconnecting...");
    WiFi.begin(ssid, pass);
  }

  if (!Blynk.connected()) {
    unoSerial.println("Blynk disconnected! Reconnecting...");
    Blynk.connect();
  }
}

void loop() {
  Blynk.run();
  checkConnection();

  static char buffer[64];
  static byte index = 0;

  while (unoSerial.available()) {
    char c = unoSerial.read();

    if (c == '\n') {
      buffer[index] = '\0';
      index = 0;

      unoSerial.print("Received: ");
      unoSerial.println(buffer);

      int mq6_1 = -1, mq6_2 = -1, mq6_3 = -1;
      char *token = strtok(buffer, "|");
      while (token != NULL) {
        if (strncmp(token, "MQ6_1:", 6) == 0) {
          mq6_1 = atoi(token + 6);
        } else if (strncmp(token, "MQ6_2:", 6) == 0) {
          mq6_2 = atoi(token + 6);
        } else if (strncmp(token, "MQ6_3:", 6) == 0) {
          mq6_3 = atoi(token + 6);
        }
        token = strtok(NULL, "|");
      }

      if (mq6_1 >= 0 && mq6_2 >= 0 && mq6_3 >= 0) {
        Blynk.virtualWrite(V0, mq6_1);
        Blynk.virtualWrite(V1, mq6_2);
        Blynk.virtualWrite(V2, mq6_3);
        Blynk.virtualWrite(V3, mq6_1 > 600 ? 255 : 0);
        Blynk.virtualWrite(V4, mq6_2 > 600 ? 255 : 0);
        Blynk.virtualWrite(V5, mq6_3 > 600 ? 255 : 0);
      } else {
        unoSerial.println("Invalid or incomplete data received.");
      }

    } else {
      if (index < sizeof(buffer) - 1) {
        buffer[index++] = c;
      } else {
        unoSerial.println("Buffer overflow, resetting.");
        index = 0;
        memset(buffer, 0, sizeof(buffer));
      }
    }
  }

  yield();  // tránh watchdog reset
}
