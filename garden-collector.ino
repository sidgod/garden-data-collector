#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define DHTPIN 5 // GPIO 5 Pin (D1 on ESP8266 Board)

#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

ESP8266WebServer server(80);

const char* ssid = "<You Wifi SSID>";
const char* password = "<Your Wifi PassKey>";

int humidityValue = 0;
int temperatureValue = 0;
int soilMoistureValue = 0;

unsigned long previousMillis = 0;
const long interval = 2000;

String dataString = "";

WiFiClient espClient;

void handle_root() {
  server.send(200, "text/plain", "Hello from the garden esp8266, you can get temperature, humidity and soil moisture from me!");
  delay(100);
}

void setup() {
  Serial.begin (115200);
  setup_wifi();
  dht.begin();

  server.on("/", handle_root);

  server.on("/data", []() {
    readData();
    dataString = "{\"temperature\": " + String(temperatureValue) + ", \"humidity\": " + humidityValue + ", \"soilMoisture\": " + soilMoistureValue + "}";
    server.send(200, "application/json", dataString);
  });

  server.begin();
}

void readData()
{

  soilMoistureValue = analogRead(A0);
  
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor
    previousMillis = currentMillis;

    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C\t");
    Serial.print("Moisture: ");
    Serial.print(soilMoistureValue);
    Serial.println("");

    humidityValue = (int)(h);
    temperatureValue = (int)(t);
  }
}

void loop()
{
  server.handleClient();
}

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
