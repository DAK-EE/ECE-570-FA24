/*
* ----------------------------------------------
* Project/Program Name : ButtonPUT-JSON
* File Name : main.cpp
* Author : Diego King
* Date : 10/30/2024
* Version : v2.0
*
* Purpose:
* ESP8266 Connected to two RGBs whose color is controlled by a webpage.
*
*
* Example Application:
*   Node network with remote controllable parameters.
*
* Dependencies:
*   include\ArduinoJson-v6.21.5.h
*
* ---------------------------------------------------------------------------
*/

#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include <ArduinoJson-v6.21.5.h>

void whichLED(DynamicJsonDocument& jsonPayload);
String sendHTTP(String URL);
DynamicJsonDocument parseJSON(String json);
void setLED(int pinR, int pinG, int pinB, int rcolor, int gcolor, int bcolor);
bool readButton();
const char* ssid = "iPhone 190"; //Enter SSID
const char* password = "myhotspot"; //Enter Password
String myURL = "http://67.169.107.53/RGBresults.txt";
String myURL2 = "http://67.169.107.53/results.txt";
const int PIN_BLUE1 = D5;
const int PIN_GREEN1 = D6;
const int PIN_RED1 = D7;
const int PIN_BLUE2 = D2;
const int PIN_GREEN2 = D3;
const int PIN_RED2 = D1;
const int PIN_BUTTON = D8;
const int PIN_BOARD_LED = D4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi connection Successful");
  pinMode(D8, INPUT);
  pinMode(D4, OUTPUT);
  pinMode(PIN_RED1, OUTPUT);
  pinMode(PIN_GREEN1, OUTPUT);
  pinMode(PIN_BLUE1, OUTPUT);
  pinMode(PIN_RED2, OUTPUT);
  pinMode(PIN_GREEN2, OUTPUT);
  pinMode(PIN_BLUE2, OUTPUT);
}

void loop() {
  if(readButton()){
    Serial.println("Button is pushed");
    String payload = sendHTTP(myURL);
    DynamicJsonDocument jsonPayload = parseJSON(payload);
    whichLED(jsonPayload);
    String payload2;
    delay(250);
  }
}

bool readButton(){
  return digitalRead(PIN_BUTTON);
}

String sendHTTP(String URL){
  String payload = "";
  if(WiFi.status()==WL_CONNECTED) {
      /*BearSSL client "clientS" is used for HTTPS requests, as opposed to HTTP requests*/
      WiFiClientSecure clientS;
      WiFiClient client;
      HTTPClient http;
      if(URL.charAt(4)=='s'){
        Serial.println("**Establishing BearSSL WiFi Client**");
        clientS.setInsecure();
        http.begin(clientS, URL);
      } else {
        Serial.println("**Establishing Arduino WiFi Client**");
        http.begin(client, URL);
      }
      int httpResponseCode = http.GET();
      if(httpResponseCode > 0) {
        payload = http.getString();
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        Serial.println("Payload: " + payload);
      } else {
        Serial.print("Error on HTTP request: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("WiFi connection could not be established.");
    }
  return payload;
}

DynamicJsonDocument parseJSON(String json){
  DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 512);
  deserializeJson(doc, json);
  Serial.println("Document deserialized.");
  return doc;
}

void whichLED(DynamicJsonDocument& jsonPayload){
  if(String(jsonPayload["led"])=="one"){
    Serial.println("LED one selected.");
    setLED(PIN_RED1, PIN_GREEN1, PIN_BLUE1, int(jsonPayload["red"]), int(jsonPayload["green"]), int(jsonPayload["blue"]));
  } else if(String(jsonPayload["led"])=="two"){
    Serial.println("LED two selected.");
    setLED(PIN_RED2, PIN_GREEN2, PIN_BLUE2, int(jsonPayload["red"]), int(jsonPayload["green"]), int(jsonPayload["blue"]));
  }else{
    Serial.println("Invalid LED selection:"+String(jsonPayload["led"]));
  }
  return;
}

void setLED(int pinR, int pinG, int pinB, int rcolor, int gcolor, int bcolor){
  Serial.println("Writing to pins:");
  Serial.println(String(pinR)+"(red)->"+String(rcolor));
  Serial.println(String(pinG)+"(green)->"+String(gcolor));
  Serial.println(String(pinB)+"(blue)->"+String(bcolor));

  digitalWrite(pinR, rcolor);
  digitalWrite(pinG, gcolor);
  digitalWrite(pinB, bcolor);
  return;
}
