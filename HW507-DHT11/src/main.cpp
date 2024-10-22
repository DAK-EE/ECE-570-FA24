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
*
* 
* 
*
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
 
#define DHT11_PIN 5
DHT dht11(DHT11_PIN, DHT11);

const char* ssid = "iPhone 190"; //Enter SSID
const char* password = "myhotspot"; //Enter Password

bool readButton();
String getTime();
int transmit(int temperature,int humidity,String time_acquired);
String getZone();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi connection Successful");
  pinMode(D0, INPUT);
  dht11.begin();
}

void loop() {
  if(readButton()){
    Serial.println("Button is pushed");
    float humidity = dht11.readHumidity();
    float temperature = dht11.readTemperature(true);
    String time = getTime();
    transmit(temperature, humidity, time);
    delay(1000);
  }
  delay(10);
}

bool readButton(){
  return digitalRead(D0);
}

String getTime() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;
  String zone = getZone();
  String URL = "https://timeapi.io/api/time/current/zone?timeZone=America%2FNew_York";
  // America%2FLos_Angeles
  if(https.begin(client, URL)){
    int HTTPCode = https.GET();
    if(HTTPCode > 0){
      String time = https.getString();
      DynamicJsonDocument doc(ESP.getMaxFreeBlockSize() - 512);
      deserializeJson(doc, time);
      String yyyy = doc["year"];
      String mm = doc["month"];
      String dd = doc["day"];
      String H = doc["hour"];
      String i = doc["minute"];
      String s = doc["seconds"];
      if (s.length()==1){
        s = "0" + s;
      }
      String mytime = yyyy + "-" + mm + "-" + dd + "%20" + H + "%3A" + i + "%3A" + s;
      https.end();
      Serial.println(mytime);
      return mytime;
      }
    https.end(); 
  } else {
      Serial.println("Unable to connect using HTTPS.");
  }
  return "Unable to connect using HTTPS.";
}

int transmit(int temperature, int humidity, String time_acquired){
  Serial.println(temperature);
  Serial.println(humidity);
  Serial.println(time_acquired);
  String URL = "http://67.169.107.53/DHT11submit.php?node_name=node_1&temperature=";
  URL = URL + temperature;
  URL = URL + "&humidity=";
  URL = URL + humidity;
  URL = URL + "&time_acquired=";
  URL = URL + time_acquired;
  if(WiFi.status()==WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, URL);
    int httpResponseCode = http.GET();
    if(httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Payload: " + payload);
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    delay(1000);
  }
  return 1;
}

String getZone(){
    String payload;
    if(WiFi.status()==WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://67.169.107.53/timezone.php");
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
    delay(1000);
  }
  return payload;
}