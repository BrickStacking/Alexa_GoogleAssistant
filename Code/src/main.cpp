#include <Arduino.h>
/*
  Version 0.3 - March 06 2018
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "WebSocketsClient.h" //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include "ArduinoJson.h"      // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <StreamString.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

#define MyApiKey "72aa04e1-84ff-4629-abba-6e5a5a28943d" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "StackingBrick"                          // TODO: Change to your Wifi network SSID
#define MyWifiPassword "1234554320"                     // TODO: Change to your Wifi network password
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883 // use 8883 for SSL
#define AIO_USERNAME "thienlc123"
#define AIO_KEY "5325ef2a33814de2bf6836afbec64a52"

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Subscribe relay1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay1");
Adafruit_MQTT_Subscribe relay2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay2");
Adafruit_MQTT_Subscribe relay3 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay3");
Adafruit_MQTT_Subscribe relay4 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay4");
Adafruit_MQTT_Publish relay_1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/relay1");
Adafruit_MQTT_Publish relay_2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/relay2");
Adafruit_MQTT_Publish relay_3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/relay3");
Adafruit_MQTT_Publish relay_4 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/relay4");

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;
int a, b, c, d;
const int button1 = 5;
const int button2 = 4;
const int button3 = 0;
const int button4 = 2;

const int relayPin1 = 16; //Change from  pin 14 to test led built-in
const int relayPin2 = 15;
const int relayPin3 = 3;
const int relayPin4 = 13;

void button_processing();
void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);
void MQTT_connect();
// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here

void turnOn(String deviceId)
{
  if (deviceId == "5bb4288477057d2175972428") // Device ID of first device
  {
    Serial.println("Turn on relay 1 ");
    Serial.println("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin1, HIGH);
    setPowerStateOnServer("5bb4288477057d2175972428", "ON");
    if (!relay_1.publish("on"))
    {
      Serial.println(F("Failed to sync from alexa to google home relay 1 ON"));
    }
    else
    {
      Serial.println(F("Sync from alexa to google home relay 1 ON"));
    }
  }
  else if (deviceId == "5bb42d6c77057d217597242e") // Device ID of second device
  {
    Serial.println("Turn on relay 2 ");
    Serial.println("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin2, HIGH);
    setPowerStateOnServer("5bb42d6c77057d217597242e", "ON");
    if (!relay_2.publish("on"))
    {
      Serial.println(F("Failed to sync from alexa to google home relay 2 ON"));
    }
    else
    {
      Serial.println(F("Sync from alexa to google home relay 2 ON"));
    }
  }
  else if (deviceId == "5bb42d7e77057d217597242f") // Device ID of third device
  {
    Serial.println("Turn on relay 3 ");
    Serial.println("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin3, HIGH);
    setPowerStateOnServer("5bb42d7e77057d217597242f", "ON");
    if (!relay_3.publish("on"))
    {
      Serial.println(F("Failed to sync from alexa to google home relay 3 ON "));
    }
    else
    {
      Serial.println(F("Sync from alexa to google home relay 3 ON"));
    }
  }
  else if (deviceId == "5bb42d8977057d2175972430") // Device ID of fourth device
  {
    Serial.println("Turn on relay 4 ");
    Serial.println("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin4, HIGH);
    setPowerStateOnServer("5bb42d8977057d2175972430", "ON");
    if (!relay_4.publish("on"))
    {
      Serial.println(F("Failed to sync from alexa to google home relay 4 ON"));
    }
    else
    {
      Serial.println(F("Sync from alexa to google home relay 4 ON"));
    }
  }
  else if (deviceId == "5bb42eed77057d2175972432") // Turn on all device
  {
    Serial.println("Turn on all relays ");
    Serial.println("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, HIGH);
    digitalWrite(relayPin3, HIGH);
    digitalWrite(relayPin4, HIGH);
    setPowerStateOnServer("5bb42d6c77057d217597242e", "ON");
    setPowerStateOnServer("5bb42d7e77057d217597242f", "ON");
    setPowerStateOnServer("5bb42d8977057d2175972430", "ON");
    setPowerStateOnServer("5bb4288477057d2175972428", "ON");
  }
  else
  {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);
  }
}

void turnOff(String deviceId)
{
  if (deviceId == "5bb4288477057d2175972428") // Device ID of first device
  {
    Serial.println("Turn off relay 1 ");
    Serial.println("Turn off Device ID: ");
    Serial.println(deviceId);
    digitalWrite(relayPin1, LOW);
    setPowerStateOnServer("5bb4288477057d2175972428", "OFF");
    if (!relay_1.publish("off"))
    {
      Serial.println(F("Failed to sync from alexa to google home relay 1 OFF"));
    }
    else
    {
      Serial.println(F("Sync from alexa to google home relay 1 OFF"));
    }
  }
  else if (deviceId == "5bb42d6c77057d217597242e") // Device ID of second device
  {
    Serial.println("Turn off relay 2 ");
    Serial.println("Turn off Device ID: ");
    Serial.println(deviceId);
    digitalWrite(relayPin2, LOW);
    setPowerStateOnServer("5bb42d6c77057d217597242e", "OFF");
    if (!relay_2.publish("off"))
    {
      Serial.println(F("Failed to sync from alexa to google home relay 2 OFF"));
    }
    else
    {
      Serial.println(F("Sync from alexa to google home relay 2 OFF"));
    }
  }
  else if (deviceId == "5bb42d7e77057d217597242f") // Device ID of third device
  {
    Serial.println("Turn off relay 3 ");
    Serial.println("Turn off Device ID: ");
    Serial.println(deviceId);
    digitalWrite(relayPin3, LOW);
    setPowerStateOnServer("5bb42d7e77057d217597242f", "OFF");
    if (!relay_3.publish("off"))
    {
      Serial.println(F("Failed to sync from alexa to google home relay 3 OFF"));
    }
    else
    {
      Serial.println(F("Sync from alexa to google home relay 3 OFF"));
    }
  }
  else if (deviceId == "5bb42d8977057d2175972430") // Device ID of fourth device
  {
    Serial.println("Turn off relay 4 ");
    Serial.println("Turn off Device ID: ");
    Serial.println(deviceId);
    digitalWrite(relayPin4, LOW);
    setPowerStateOnServer("5bb42d8977057d2175972430", "OFF");
    if (!relay_4.publish("off"))
    {
      Serial.println(F("Failed to sync from alexa to google home relay 4 OFF"));
    }
    else
    {
      Serial.println(F("Sync from alexa to google home relay 4 OFF"));
    }
  }
  else if (deviceId == "5bb42eed77057d2175972432") // Turn off all device
  {
    Serial.println("Turn off all relays ");
    Serial.println("Turn off device id: ");
    Serial.println(deviceId);
    digitalWrite(relayPin1, LOW);
    digitalWrite(relayPin2, LOW);
    digitalWrite(relayPin3, LOW);
    digitalWrite(relayPin4, LOW);
    setPowerStateOnServer("5bb42d6c77057d217597242e", "OFF");
    setPowerStateOnServer("5bb42d7e77057d217597242f", "OFF");
    setPowerStateOnServer("5bb42d8977057d2175972430", "OFF");
    setPowerStateOnServer("5bb4288477057d2175972428", "OFF");
  }
  else
  {
    Serial.print("Turn off for unknown device id: ");
    Serial.println(deviceId);
  }
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    isConnected = false;
    Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
    break;
  case WStype_CONNECTED:
  {
    isConnected = true;
    Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
    Serial.printf("Waiting for commands from sinric.com ...\n");
  }
  break;
  case WStype_TEXT:
  {
    Serial.printf("[WSc] get text: %s\n", payload);
    // Example payloads

    // For Switch or Light device types
    // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html

    // For Light device type
    // Look at the light example in github

    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.parseObject((char *)payload);
    String deviceId = json["deviceId"];
    String action = json["action"];

    if (action == "setPowerState")
    { // Switch or Light
      String value = json["value"];
      if (value == "ON")
      {
        turnOn(deviceId);
      }
      else
      {
        turnOff(deviceId);
      }
    }
    else if (action == "SetTargetTemperature")
    {
      String deviceId = json["deviceId"];
      String action = json["action"];
      String value = json["value"];
    }
    else if (action == "test")
    {
      Serial.println("[WSc] received test command from sinric.com");
    }
  }
  break;
  case WStype_BIN:
    Serial.printf("[WSc] get binary length: %u\n", length);
    break;
  }
}

void setup()
{
  Serial.begin(115200);

  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);
  // Relay Pin-Out
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);

  digitalWrite(relayPin1, LOW);
  digitalWrite(relayPin2, LOW);
  digitalWrite(relayPin3, LOW);
  digitalWrite(relayPin4, LOW);

  //Button Pins
  pinMode(button1, INPUT_PULLUP); //Button1
  pinMode(button2, INPUT_PULLUP); //Button2
  pinMode(button3, INPUT_PULLUP); //Button3
  pinMode(button4, INPUT_PULLUP); //Button4

  // Waiting for Wifi connect
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  if (WiFiMulti.run() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);

  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(10000); // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
  mqtt.subscribe(&relay1);
  mqtt.subscribe(&relay2);
  mqtt.subscribe(&relay3);
  mqtt.subscribe(&relay4);
}

void loop()
{
  webSocket.loop();
  MQTT_connect();
  if (isConnected)
  {
    // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
    if ((millis() - heartbeatTimestamp) > HEARTBEAT_INTERVAL)
    {
      heartbeatTimestamp = millis();
      webSocket.sendTXT("H");
    }
  }

  button_processing();

  //MQTT Io.adafruit

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(2000)))
  {
    // Check if its the onoff button feed
    if (subscription == &relay1)
    { //onoff_relay1
      Serial.print(F("On-Off button: "));
      Serial.println((char *)relay1.lastread);

      if (strcmp((char *)relay1.lastread, "on") == 0)
      {
        //digitalWrite(Relay1, LOW);
        Serial.println("Turn on relay1 from IO.adafruit");
      }
      if (strcmp((char *)relay1.lastread, "off") == 0)
      {
        //digitalWrite(Relay1, HIGH);
        Serial.println("Turn off relay1 from IO.adafruit");
      }
    }

    // check if its the slider feed
    if (subscription == &relay2)
    { //onoff_relay1
      Serial.print(F("On-Off button: "));
      Serial.println((char *)relay2.lastread);

      if (strcmp((char *)relay2.lastread, "on") == 0)
      {
        Serial.println("Turn on relay2 from IO.adafruit");
        //digitalWrite(Relay2, LOW);
      }
      if (strcmp((char *)relay2.lastread, "off") == 0)
      {
        Serial.println("Turn off relay2 from IO.adafruit");
        //digitalWrite(Relay2, HIGH);
      }
    }

    if (subscription == &relay3)
    { //onoff_relay1
      Serial.print(F("On-Off button: "));
      Serial.println((char *)relay3.lastread);

      if (strcmp((char *)relay3.lastread, "on") == 0)
      {
        Serial.println("Turn on relay3 from IO.adafruit");
        //digitalWrite(Relay2, LOW);
      }
      if (strcmp((char *)relay3.lastread, "off") == 0)
      {
        Serial.println("Turn off relay3 from IO.adafruit");
        //digitalWrite(Relay2, HIGH);
      }
    }

    if (subscription == &relay4)
    { //onoff_relay1
      Serial.print(F("On-Off button: "));
      Serial.println((char *)relay4.lastread);

      if (strcmp((char *)relay4.lastread, "on") == 0)
      {
        Serial.println("Turn on relay4 from IO.adafruit");
        //digitalWrite(Relay2, LOW);
      }
      if (strcmp((char *)relay4.lastread, "off") == 0)
      {
        Serial.println("Turn off relay4 from IO.adafruit");
        //digitalWrite(Relay2, HIGH);
      }
    }
  }
}

// If you are going to use a push button to on/off the switch manually, use this function to update the status on the server
// so it will reflect on Alexa app.
// eg: setPowerStateOnServer("deviceid", "ON")
void setPowerStateOnServer(String deviceId, String value)
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["deviceId"] = deviceId;
  root["action"] = "setPowerState";
  root["value"] = value;
  StreamString databuf;
  root.printTo(databuf);

  webSocket.sendTXT(databuf);
}

//eg: setPowerStateOnServer("deviceid", "CELSIUS", "25.0")
void setTargetTemperatureOnServer(String deviceId, String value, String scale)
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["action"] = "SetTargetTemperature";
  root["deviceId"] = deviceId;

  JsonObject &valueObj = root.createNestedObject("value");
  JsonObject &targetSetpoint = valueObj.createNestedObject("targetSetpoint");
  targetSetpoint["value"] = value;
  targetSetpoint["scale"] = scale;

  StreamString databuf;
  root.printTo(databuf);

  webSocket.sendTXT(databuf);
}

void button_processing()
{
  /*-----------------*/ // Relay and button 1
  if (digitalRead(button1) == 1)
    a = 0;
  if (digitalRead(button1) == 0)
    a++;
  if (a == 15)
    digitalWrite(relayPin1, !digitalRead(relayPin1));
  //Sau khi toggle phai update trang thai nut nhan len server

  //////////

  /*-----------------*/ // Relay and button 2
  if (digitalRead(button2) == 1)
    b = 0;
  if (digitalRead(button2) == 0)
    b++;
  if (b == 15)
    digitalWrite(relayPin2, !digitalRead(relayPin2));

  //////////

  /*-----------------*/ // Relay and button 3
  if (digitalRead(button3) == 1)
    c = 0;
  if (digitalRead(button3) == 0)
    c++;
  if (c == 15)
    digitalWrite(relayPin3, !digitalRead(relayPin3));

  //////////

  /*-----------------*/ // Relay and button 4
  if (digitalRead(button4) == 1)
    d = 0;
  if (digitalRead(button4) == 0)
    d++;
  if (d == 15)
    digitalWrite(relayPin4, !digitalRead(relayPin4));

  //////////
}

void MQTT_connect()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}