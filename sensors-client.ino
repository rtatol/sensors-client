#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// BME280 default configuration
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

const char* deviceId = "deviceId";
const char* wifiSsid = "ssid";
const char* wifiPassword = "password";
const char* serverAddress = "http://address:port/";

HTTPClient http;
Adafruit_BME280 bme; // I2C connection, SLC -> D1, SDA -> D2

void setup() {

  Serial.begin(115200);

  WiFi.begin(wifiSsid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.println("Waiting for connection");
  }

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    char* request = prepareRequest();
    sendRequest(request);
  } else {
    Serial.println("WiFi connection error");
  }

  delay(5000);
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
  gauges["humidity"] = bme.readHumidity();
  gauges["temperature"] = bme.readTemperature();
  gauges["pressure"] = bme.readPressure() / 100.0F;
  gauges["lux"] = analogRead(A0);

  static char requestBuffer[256];
  request.printTo(requestBuffer, sizeof(requestBuffer));

  return requestBuffer;
}
