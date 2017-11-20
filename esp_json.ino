/*------------------------------------------------------------------------------
 JSON request via curl:
 > curl -H "Content-Type: application/json" -X POST -d '{state: true}' http://IP/led
------------------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server;
uint8_t pin_led = 16;
char* ssid = "*****";
char* password = "*****";

void setup()
{
  WiFi.begin(ssid,password);
  Serial.begin(115200);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/led",setLed);
  server.begin();
}

void loop()
{
  server.handleClient();
}

void setLed()
{
  String data = server.arg("plain");
  StaticJsonBuffer<200> jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
  String state = jObject["state"];
  Serial.println(state);
  server.send(204,"");
}
