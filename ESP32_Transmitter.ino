//BASIC INFORMATION
// Program: sending data from devices through MQTT (joystick, keyboard)
// Author:  Paweł Bartkiewicz 
// email:   15g.pawel.bart@gmail.com

//ADDITIONAL INFORMATION 
// PubSubClient need to be installed
// PubSubClient by Nick 0'Leary 

// Keypad library need to be installed 
// Keypad by Mark Stanley, Alexander Brevig

//DESCRIPTION
// this is program for esp32
// it receives the data from pheripheral devices (joystick, membrane keyboard)
// and sends it throught the MQTT to the phone app created in MIT App Inventor
// the app works like proxy server - receives the data, displays it and passes it on
// this data is later received by another esp32 and based on the data turns on/turn off LEDs 

#include <WiFi.h>
#include <PubSubClient.h>
#include <Keypad.h>
#define VRX 32
#define VRY 35

int prevX = 0;
int prevY = 0;

const byte ROWS = 4;
const byte COLS = 3;

byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {25, 26, 33};

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'},
};

Keypad keyboard = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const char* ssid = "login";
const char* password = "password";

///public MQTT server 
///other: 
///  broker.emqx.io
///  test.mosquitto.org
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

String rP; 
bool flagU = false;
bool flagD = false;
bool flagR = false;
bool flagL = false;

void setup() 
{

  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  
  Serial.begin(115200);

  connectToWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    if (client.connect("lalalala")) {
      Serial.println("Connected to MQTT server");
      
      //here you must change subscribed topic
      //topic must be the same like in the function connectToMQTT 
      client.subscribe("abc123");
    } else {
      Serial.print("Couldnt connect, error code:");
      Serial.print(client.state());
      Serial.println("Retrying in 5s...");
      delay(5000);
    }
  }
}

void loop() 
{
  
  if (!client.connected()) 
  {
    connectToMQTT();
  }
  client.loop();
  checkWiFiConnection();

  char KKey = keyboard.getKey();
  int x = analogRead(VRX);
  int y = analogRead(VRY);
  
  // values from joystick are passed 
  // when value changes more than 100
  if (x - prevX >= 100 || x - prevX <= -100) {
    String xx = String(x);
    client.publish("abc123X", xx.c_str());
  }
  
  if (y - prevY >= 100 || y - prevY <= -100) {
    String yy = String(y);
    client.publish("abc123Y", yy.c_str());
  }


/////////////////////////////////////// Joystick DOWN
  if (y >= 3000 && !flagD) {  
    client.publish("abc123", "D");
    flagD = true; 
  } else if (y <= 3000 && y >=1900 && flagD) {
    client.publish("abc123", "nD");
    flagD = false;
  }
/////////////////////////////////////// Joystick UP
  if (y <= 1000 && !flagU) {  
    client.publish("abc123", "U");
    flagU = true; 
  } else if (y >= 1000 && y <= 2000 && flagU) {
    client.publish("abc123", "nU");
    flagU = false;
  }
  /////////////////////////////////////// Joystick RIGHT
  if (x >= 3000 && !flagR) {  
    client.publish("abc123", "R");
    flagR = true; 
  } else if (x <= 3000 && x >=1900 && flagR) {
    client.publish("abc123", "nR");
    flagR = false;
  }
  /////////////////////////////////////// Joystick LEFT
  if (x <= 1000 && !flagL) {  
    client.publish("abc123", "L");
    flagL = true; 
  } else if (x >= 1000 && x <= 2000 && flagL) {
    client.publish("abc123", "nL");
    flagL = false;
  }
/////////////////////////////////////// Keyboard
  switch(KKey){
    case '2':
      client.publish("abc123", "2");
      break;
    case '4':
      client.publish("abc123", "4");
      break;
    case '6':
      client.publish("abc123", "6");
      break;
    case '8':
      client.publish("abc123", "8");
      break;
    case '5':
      client.publish("abc123", "5");
      break;
  }
  prevX = x;
  prevY = y;
}


void callback(char* topic, byte* payload, unsigned int length) 
{
  rP = ""; 
  for (int i = 0; i < length; i++) 
  {
    rP += (char)payload[i];
  }
}

void connectToWiFi() 
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  Serial.println("Connected to WiFi!");
}


void checkWiFiConnection() 
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("WiFi connection has been dropped!");
    connectToWiFi(); 
  }
}

void connectToMQTT() 
{
  while (!client.connected()) 
  {
    Serial.println("Connecting to MQTT...");
    if (client.connect("lalalala")) 
    {
      Serial.println("Connected to MQTT server!");
      client.subscribe("abc123");
    } 
    else 
    {
      Serial.print("Couldnt connect, error code: ");
      Serial.print(client.state());
      Serial.println("Retrying in 5s...");
      delay(5000);
    }
  }
}
