/*
* ----------------------------------------------
* Project/Program Name : HW507-DHT11 Sensor Node
* File Name : main.cpp
* Author : Diego King
* Date : 10/22/2024
* Version : v2.0
*
* Purpose:
* ESP8266 connects to hard-coded SSID to:
*   Acquire timezone from webserver
*   Acquire time from timeAPI
*   Send Data with time to database
*
* Inputs:
*   Hardcoded parameters: WiFi credentials, webserver URL for posting data to database
*
* Outputs:
*   None
*
* Example Application:
*   Used as a remote node which monitors temperature and humidity and is able to send it to a webserver based database.
*
* Dependencies:
*   include\ArduinoJson-v6.21.5.h
* ---------------------------------------------------------------------------
*/

#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include <ArduinoJson-v6.21.5.h>
#include <iostream>
#include <string>
#include "DHT.h"
#include "Adafruit_Sensor.h"
 
#define BUTTON_PIN D0
#define DHT11_PIN 5
DHT dht11(DHT11_PIN, DHT11);

const char* ssid = "iPhone 190"; //Enter SSID
const char* password = "myhotspot"; //Enter Password

void setupWiFi();
bool readButton();
String sendHTTP(String URL);
String getZone();
String getTime();
String parseTime(String payload);
void transmit(int temperature,int humidity,String time_acquired);

void setup() {
  Serial.begin(115200);
  setupWiFi();
  pinMode(BUTTON_PIN, INPUT);
  dht11.begin();
}

void loop() {
  if(readButton()) {
    float humidity = dht11.readHumidity();
    float temperature = dht11.readTemperature(true);
    String time = getTime();
    transmit(temperature, humidity, time);
    delay(1000);
  }
}

void setupWiFi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi connection Successful");
  return;
}

bool readButton(){
  return digitalRead(BUTTON_PIN);
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
};

String getZone(){
  String payload = sendHTTP("http://67.169.107.53/timezone.php");
  return payload;
}

String getTime() {
  String zone = getZone();
  String URL = "https://timeapi.io/api/time/current/zone?timeZone=";
  URL = URL + zone;
  String payload = sendHTTP(URL);
  String time = parseTime(payload);
  return time;
}

String parseTime(String payload){
  DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 512);
  deserializeJson(doc, payload);
  String yyyy = doc["year"];
  String mm = doc["month"];
  String dd = doc["day"];
  String H = doc["hour"];
  String i = doc["minute"];
  String s = doc["seconds"];
  if (s.length()==1){
    s = "0" + s;
  }
  String mytime;
  mytime = yyyy + "-" + mm + "-" + dd + "%20" + H + "%3A" + i + "%3A" + s;
  return mytime;
}

void transmit(int temperature, int humidity, String time_acquired){
  Serial.println(temperature);
  Serial.println(humidity);
  Serial.println(time_acquired);
  
  String URL = "http://67.169.107.53/DHT11submit.php?node_name=node_1&temperature=";
  URL = URL + temperature;
  URL = URL + "&humidity=";
  URL = URL + humidity;
  URL = URL + "&time_acquired=";
  URL = URL + time_acquired;
  sendHTTP(URL);
  return;
}