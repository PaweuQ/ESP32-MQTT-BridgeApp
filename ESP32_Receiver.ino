//BASIC INFORMATION
// Program: receiving data from server (phone app)
// Author:  Paweł Bartkiewicz 
// email:   15g.pawel.bart@gmail.com

//ADDITIONAL INFORMATION 
// PubSubClient need to be installed
// PubSubClient by Nick 0'Leary 

//DESCRIPTION
// this is program for esp32
// receives data from phone app, which is considered to be a server
// program controles 4 LED diodes basing on the data (received via MQTT)

#include <WiFi.h>
#include <PubSubClient.h>
#define LED_up 5
#define LED_down 19
#define LED_right 18 
#define LED_left 21


const char* ssid = "login";
const char* password = "password";

///publiczny serwer mqtt
///inne: 
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
bool flag5 = false;

void setup() 
{
  pinMode(LED_up, OUTPUT);
  pinMode(LED_down, OUTPUT);
  pinMode(LED_right, OUTPUT);
  pinMode(LED_left, OUTPUT);
  Serial.begin(115200);

  connectToWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    if (client.connect("lalalala2")) {
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

  if (rP == "UP" && !flagU) {  
    digitalWrite(LED_up, HIGH);
    flagU = true;
    flag5 = false; 
  } else if (rP == "nUP" && flagU) {
    digitalWrite(LED_up, LOW);
    flagU = false;
  }

  if (rP == "DOWN" && !flagD) {  
    digitalWrite(LED_down, HIGH);
    flagD = true;
    flag5 = false; 
  } else if (rP == "nDOWN" && flagD) {
    digitalWrite(LED_down, LOW);
    flagD = false;
  }

  if (rP == "RIGHT" && !flagR) {  
    digitalWrite(LED_right, HIGH);
    flagR = true;
    flag5 = false; 
  } else if (rP == "nRIGHT" && flagR) {
    digitalWrite(LED_right, LOW);
    flagR = false;
  }

  if (rP == "LEFT" && !flagL) {  
    digitalWrite(LED_left, HIGH);
    flagL = true;
    flag5 = false;  
  } else if (rP == "nLEFT" && flagL) {
    digitalWrite(LED_left, LOW);
    flagL = false;
  }
  if (rP == "nALL" && !flag5) {
    digitalWrite(LED_up, LOW);
    digitalWrite(LED_down, LOW);
    digitalWrite(LED_right, LOW);
    digitalWrite(LED_left, LOW);
    flag5 = true; 
    flagU = false;
    flagD = false;
    flagR = false;
    flagL = false;
  }
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
    if (client.connect("lalalala2")) 
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
