/*
* ----------------------------------------------
* Project/Program Name : ButtonPUT
* File Name : main.cpp
* Author : Diego King
* Date : 10/30/2024
* Version : v2.0
*
* Purpose:
* Sends HTTP request to get LED status (on or off) at the push of a button.
*
*
* Example Application:
*   Used as a remote control for any larger node.
*
*
* ---------------------------------------------------------------------------
*/

#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"

String sendHTTP(String URL);
bool readButton();
const char* ssid = "iPhone 190"; //Enter SSID
const char* password = "myhotspot"; //Enter Password
String myURL = "http://67.169.107.53/results.txt";

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
}

void loop() {
  if(readButton()){
    Serial.println("Button is pushed");
    String payload = sendHTTP(myURL);
    if(payload=="on"){
      digitalWrite(D4, 0);
    } else {
      digitalWrite(D4, 1);
    }
    delay(1000);
  }
}

bool readButton(){
  return digitalRead(D8);
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
