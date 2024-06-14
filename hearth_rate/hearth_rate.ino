#include <WiFi.h>

const char* ssid = "UDINUS H.3";
const char* password = "";
const char* serverName = "http://192.168.34.242:5000";

#define LP 12
#define LM 13
#define PIN_AN 33

WiFiClient client;

void setup() {
  Serial.begin(9600);
  pinMode(LP, INPUT); // Setup for leads off detection LO +
  pinMode(LM, INPUT); // Setup for leads off detection LO -
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  if ((digitalRead(LP) == 1) || (digitalRead(LM) == 1)) {
    Serial.println('!');
  } else {
    int sensorValue = analogRead(PIN_AN);

    // Mengirimkan data melalui metode GET
    if (client.connect(serverName, 5000)) {
      String getData = "/get_data?data=" + String(sensorValue);
      client.print(String("GET ") + getData + " HTTP/1.1\r\n" +
                   "Host: " + serverName + "\r\n" +
                   "Connection: close\r\n\r\n");
      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          Serial.println("Data sent via GET");
          break;
        }
      }
      client.stop();
    }

    delay(5000); // Delay 5 detik sebelum mengirimkan data lagi

    // Mengirimkan data melalui metode POST
    if (client.connect(serverName, 5000)) {
      String postData = "data=" + String(sensorValue);
      client.print("POST /post_data HTTP/1.1\r\n");
      client.print("Host: " + String(serverName) + "\r\n");
      client.print("Content-Type: application/x-www-form-urlencoded\r\n");
      client.print("Content-Length: " + String(postData.length()) + "\r\n\r\n");
      client.print(postData);
      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          Serial.println("Data sent via POST");
          break;
        }
      }
      client.stop();
    }

    delay(5000); // Delay 5 detik sebelum mengirimkan data lagi
  }

  delay(500); // Wait for a bit to keep serial data from saturating
}