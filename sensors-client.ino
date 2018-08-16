#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "DHT.h" // https://github.com/adafruit/DHT-sensor-library

#define DHTPIN 14
#define DHTTYPE DHT22

const char* deviceId = "deviceId";
const char* wifiSsid = "ssid";
const char* wifiPassword = "password";
const char* serverAddress = "http://address:port/";
const unsigned long delayTime = 60000; // in milliseconds;

HTTPClient http;
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(115200);

  WiFi.begin(wifiSsid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.println("Waiting for connection");
  }
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    char* request = prepareRequest();
    sendRequest(request);
  } else {
    Serial.println("WiFi connection error");
  }

  delay(delayTime);
}

void sendRequest(char* request) {

  http.begin(serverAddress);
  http.addHeader("Content-Type", "application/json");

  Serial.print("\nrequest:");
  Serial.println(request);

  int httpResponseCode = http.POST(request);

  Serial.print("response: ");
  Serial.println(httpResponseCode);

  http.end();
}

char* prepareRequest() {

  StaticJsonBuffer<256> jsonBuffer;

  JsonObject& request = jsonBuffer.createObject();
  request["deviceId"] = deviceId;

  JsonObject& gauges = request.createNestedObject("gauges");
  gauges["humidity"] = dht.readHumidity();
  gauges["temperature"] = dht.readTemperature();

  static char requestBuffer[256];
  request.printTo(requestBuffer, sizeof(requestBuffer));

  return requestBuffer;
}
