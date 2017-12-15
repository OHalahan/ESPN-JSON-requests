/*------------------------------------------------------------------------------
  JSON request via curl:
  > curl -H "Content-Type: application/json" -X POST -d '{state: true}' http://IP/led
  > curl -H "Content-Type: application/json" -X GET http://IP/info
  ------------------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server;

#define firstLine 5
#define secondLine 4

#define zeroPin 2

int firstDelay = 20;
int secondDelay = 40;

// arbitrary taken amount of dimming steps
// affects frequencyMultiplier
int dimming = 128;

// Each sinuswave takes 1000ms/50=20ms and each pike is 20/2ms = 10 after diode bridge
// 10ms/128 = 10000us/128=75us (in fact, its 78, but 75 is OK)
int frequencyMultiplier = 75;

int propagationDelay = 8.33; // theoretical propagation delay for thyristor, try to play

// How long to wait for increasing dimming in loop
int staticDelay = 10;

// WIFI settings
char* ssid = "****";
char* password = "****";

int counter = 0;

void setup()
{
  analogWriteFreq(100);

  // first LED line
  pinMode(firstLine, OUTPUT);
  pinMode(secondLine, OUTPUT);

  // second LED line
  digitalWrite(firstLine, LOW);
  digitalWrite(secondLine, LOW);

  // Here goes interruption
  pinMode(zeroPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(zeroPin), detectChange, RISING);

  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/info", returnInfo);
  server.on("/mode", setParams);
  server.begin();
}

void loop()
{
  server.handleClient();
  for (int i = 5; i <= 128; i++) {
    dimming = i;
    delay(staticDelay);
  }
}

// Main function which handles dimming when phase starts
void detectChange() {
  int dimtime = (frequencyMultiplier * dimming);

  delayMicroseconds(dimtime);

  digitalWrite(firstLine, HIGH);
  digitalWrite(secondLine, HIGH);

  delayMicroseconds(propagationDelay);

  digitalWrite(firstLine, LOW);
  digitalWrite(secondLine, LOW);
}

// https://github.com/me-no-dev/ESPAsyncWebServer#arduinojson-basic-response
void returnInfo() {
  const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["success"] = true;

  JsonObject& data = root.createNestedObject("data");
  data["dimming"] = dimming;
  data["static_delay"] = staticDelay;
  data["frequency_multiplier"] = frequencyMultiplier;
  data["propagation_delay"] = propagationDelay;

  String response = "";
  root.prettyPrintTo(response);

  server.send(200, "text/json", response);
}

void setParams()
{
  const size_t bufferSize = JSON_OBJECT_SIZE(4) + 70;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  String data = server.arg("plain");
  JsonObject& root = jsonBuffer.parseObject(data);

  int delay = root["delay"];
  int dimming = root["dimming"];
  int frequencyMultiplier = root["frequency_multiplier"];
  int propagationDelay = root["propagation_delay"];

  returnInfo();
}
