/*
 * Sigfox Buttonサンプル
 * UnaShieldのボタンを押したときにメッセージをSigfoxで送信します
 * Copyright (c) 2018 Kyocera Communication Systems, Co,. Ltd.
 * Callback - Custom Payload Config : button::bool:7
 */
#include "SIGFOX.h"

const int buttonPin = 6;    //ボタンPIN
const int ledPin = 9;         //LED PIN

// IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.
static const String device = "NOTUSED";  //  Set this to your device name if you're using UnaBiz Emulator.
static const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
static const bool echo = true;  //  Set to true if the SIGFOX library should display the executed commands.
static const Country country = COUNTRY_JP;  //  Set this to your country to configure the SIGFOX transmission frequencies.
static UnaShieldV2S transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaShield V2S Dev Kit
static String response;  //  Will store the downlink response from SIGFOX.
// IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.

void setup()
{
  Serial.begin(9600);         // Arduinoハードウェアシリアルを起動(Arduino <-> PC)
  Serial.println("===========================");
  Serial.println("Sigfox UnaShield Button Sample");
  Serial.println("Send a message when a button on UnaShiled is pushed.");
  Serial.println("===========================");
  
  //Sigfoxモジュールを起動
  if (!transceiver.begin()) stop(F("Unable to init SIGFOX module, may be missing"));  //  Will never return.

  pinMode(ledPin, OUTPUT);
}

void loop()
{ 
  //ボタン押下状態を取得する
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW)
  {
    digitalWrite(ledPin, HIGH);
    Serial.println(F("Pushed"));
    sendSigfoxMessage();
  }
  else 
  {
    digitalWrite(ledPin, LOW);
    Serial.println(F("Not Pushed"));    
  }
}

//ボタン押下をSigfoxメッセージで送信する
void sendSigfoxMessage() 
{
  Serial.println("\n*****SEND SIGFOX MESSAGE*****");
  Serial.print("Payload: "); Serial.println("80");  //0x80 : 10000000
  transceiver.sendMessage("80");
  Serial.println("*****************************");
}
