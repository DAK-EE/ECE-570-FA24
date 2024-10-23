/*
* ----------------------------------------------
* Project/Program Name : BlinkLED
* File Name : main.cpp
* Author : Diego King
* Date : 10/22/2024
* Version : v2.0
*
* Purpose:
* Blinks an LED connected to an ESP8266 at a specified rate using object oriented programming.
*
* Inputs:
*   blinkRate - the rate of LED blinking.
*
* Outputs:
*   none
*
* Example Application:
*   Diagnostics indicator light.
*
* Dependencies:
*
* 
* 
*
* ---------------------------------------------------------------------------
*/
#include <ledblink.h>
#include <Arduino.h>
Blink LED(5);
void setup() {
  LED.blinkRate(250);
  pinMode(LED._pin, OUTPUT);
}

void loop() {
  digitalWrite(LED._pin, LOW);
  delay(LED._rate);
  digitalWrite(LED._pin, HIGH);
  delay(LED._rate);
}