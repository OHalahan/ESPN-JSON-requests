/*------------------------------------------------------------------------------
  JSON request via curl:
  > curl -H "Content-Type: application/json" -X POST -d '{state: true}' http://IP/led
  > curl -H "Content-Type: application/json" -X GET http://IP/info
  ------------------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server;
uint8_t pin_led = 16;
char* ssid = "Xiaomi_E916";
char* password = "sveta.sasha";

void setup()
{
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
}

void getInfo() {
  String json = "{";
  json += "led: true";
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
