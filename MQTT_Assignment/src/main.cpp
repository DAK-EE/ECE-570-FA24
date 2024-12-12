#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

// put function declarations here:
void setLED(int pinR, int pinG, int pinB, int rcolor, int gcolor, int bcolor);
bool pollButton();
int pollADC();
void setupWifi();
void callback(char* topic, byte* payload, int length);
void reconnect();
String getStatusJSON();
// char* intToStr(int num);

const int PIN_BLUE = D2;
const int PIN_GREEN = D5;
const int PIN_RED = D1;
const int PIN_BUTTON = D3;
const int PIN_BOARD_LED1 = D4;
const int PIN_BOARD_LED2 = D0;
const int PIN_POT = A0;

// WiFi variables
const char* ssid = "NETGEAR53";
const char* password = "jaggedink965";

// MQTT variables
// const char* port = "1883";
const char* mqtt_server = "broker.hivemq.com";
const char* publishTopic = "myesp/testing/room"; //output topic where the esp publishes
const char* subscribeTopic = "myesp/testing/room";
#define publishTimeInterval 10000

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
#define BUILTIN_LED 2
char msg[MSG_BUFFER_SIZE];
int value = 0;
int ledStatus = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_BOARD_LED1, OUTPUT);
  pinMode(PIN_BOARD_LED2, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_POT, INPUT);
  digitalWrite(PIN_BOARD_LED1, 1);
  digitalWrite(PIN_BOARD_LED2, 1);
  setupWifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  unsigned long now = millis();
  if(now - lastMsg > publishTimeInterval) {
    String myword = getStatusJSON();
    lastMsg = now;
    value = pollADC();
    snprintf(msg, MSG_BUFFER_SIZE, "%s", myword.c_str());
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(publishTopic, msg);
  }
}

void setLED(int pinR, int pinG, int pinB, int rcolor, int gcolor, int bcolor){
  // Serial.println("Writing to pins:");
  // Serial.println(String(pinR)+"(red)->"+String(rcolor));
  // Serial.println(String(pinG)+"(green)->"+String(gcolor));
  // Serial.println(String(pinB)+"(blue)->"+String(bcolor));

  digitalWrite(pinR, rcolor);
  digitalWrite(pinG, gcolor);
  digitalWrite(pinB, bcolor);
  return;
}

bool pollButton(){
  return !digitalRead(PIN_BUTTON);
}

int pollADC(){
  return analogRead(PIN_POT);
}

void setupWifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

String getStatusJSON(){
  int ADC = pollADC();
  int LED;
  String pre= "{\"status\": [{\"LED\": \"";
  String mid= "\",\"ADC\": \"";
  String post = "\"}]}";
  if(digitalRead(PIN_RED)){
    LED = 1;
  } else {
    LED = 0; 
  }
  return pre + String(LED) + mid + String(ADC) + post;
}

void callback(char* topic, byte* payload, int length){
  char* targetON = "LED ON";
  char* targetOFF = "LED OFF";
  bool matchON = true;
  bool matchOFF = true;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
    if((char)payload[i] != targetON[i]){
      matchON = false;
    }
    if((char)payload[i] != targetOFF[i]){
      matchOFF = false;
    }
  }
  Serial.println();
  if(matchON){
    // setLED(PIN_RED, PIN_BLUE, PIN_GREEN, 1, 0, 0);
    digitalWrite(PIN_RED, 1);
  } else if(matchOFF){
    // setLED(PIN_RED, PIN_BLUE, PIN_GREEN, 0, 0, 0);
    digitalWrite(PIN_RED, 0);
  }
  Serial.println();
}

void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())){
      Serial.println("connected");
      client.subscribe(subscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds.");
      delay(5000);
    }
  }
}