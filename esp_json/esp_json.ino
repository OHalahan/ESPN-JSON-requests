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

int dimming = 128;

char* ssid = "****";
char* password = "****";

int counter = 0;

void setup()
{
  analogWriteFreq(100);
  pinMode(firstLine, OUTPUT);
  digitalWrite(firstLine, LOW);
  pinMode(secondLine, OUTPUT);
  digitalWrite(secondLine, LOW);

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

  server.on("/led", setLed);
  server.on("/info", getInfo);
  server.on("/mode", nextMode);
  server.begin();
}

void loop()
{
  server.handleClient();
  for (int i = 5; i <= 128; i++) {
    dimming = i;
    delay(10);
  }
}

void detectChange() {
  int dimtime = (65 * dimming);  // For 60Hz =>65
  delayMicroseconds(dimtime);    // Off cycle
  digitalWrite(firstLine, HIGH);   // triac firing
  digitalWrite(secondLine, HIGH);   // triac firing
  delayMicroseconds(8.33);         // triac On propagation delay (for 60Hz use 8.33)
  digitalWrite(firstLine, LOW);   // triac firing
  digitalWrite(secondLine, LOW);   // triac firing
}

void getInfo() {
  String json = "{";
  json += "counter: " + String(counter);
  json += "\n";
  json += "}";
  server.send(200, "text/json", json);
}

void setLed()
{
  String data = server.arg("plain");
  StaticJsonBuffer<200> jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
  boolean state = jObject["state"];
  Serial.println(state);
  String json = "{";
  json += "\"led\":" + String(state);
  json += "}";

  server.send(200, "text/json", json);
}

void nextMode() {
  Serial.println("Start pressing");
  delay(500);
  Serial.println("Stop pressing");
  String json = "{";
  json += "success: true";
  json += "}";
  server.send(200, "text/json", json);
}
