/*
 * Digital Light Sensor (TSL2561) - Sigfoxサンプル
 * UnaShieldのGrove(I2C)ポートにセンサを接続してください
 * http://wiki.seeedstudio.com/Grove-Digital_Light_Sensor/
 * Copyright (c) 2018 Kyocera Communication Systems, Co,. Ltd.
 * Callback -Custom Payload Config : lux::uint:16:little-endian isBright::bool:7
 */
#include "SIGFOX.h"
#include "Digital_Light_TSL2561.h"
#include <Wire.h>

//必要に応じ書き換えてください
//************************************
static const bool isDebug = true;
static const int LIGHT_THRESHOLD = 40; //明暗判断の閾値(Lux)
//************************************

const int ledPin = 9;
bool lastLightState = false;

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
  Serial.println("Sigfox UnaShield Grove Light Sample");
  Serial.print("Send a message when the brightness is changed: "); Serial.print(LIGHT_THRESHOLD); Serial.println(" lux.");
  Serial.println("===========================");
  
  //Sigfoxモジュールを起動
  if (!transceiver.begin()) stop(F("Unable to init SIGFOX module, may be missing"));  //  Will never return.

  //Lightセンサ初期化
  Wire.begin();
  TSL2561.init();

  //明暗LED
  pinMode(ledPin, OUTPUT);
}

void loop()
{ 
  //Lightセンサから照度を取得
  int lux = TSL2561.readVisibleLux();
  if (isDebug) {Serial.print("Light(lux): "); Serial.println(lux);}
  
  //明暗判断
  bool lightState = (lux >= LIGHT_THRESHOLD) ? true : false;
  
  //LEDを明暗にあわせてON/OFFする
  digitalWrite(ledPin, lightState);
  //明暗の変化時にSigfoxで照度を送信
  if (lightState != lastLightState)
  {
    sendSigfoxMessage(lux, lightState);
    lastLightState = lightState;
    delay(2000);
  }
  delay(500);  
}

//照度と明暗をSigfoxメッセージで送信する
void sendSigfoxMessage(int lux, bool isBright) 
{
  Serial.println("\n*****SEND SIGFOX MESSAGE*****");
  String sLux = transceiver.toHex(lux);
  String sIsBright = (isBright) ? "80" : "00"; //0x80(1000000) / 0x00(00000000)
  Serial.print("IsBright: "); Serial.println(isBright);
  Serial.print("Payload: "); Serial.println(sLux + sIsBright);
  transceiver.sendMessage(sLux + sIsBright);
  Serial.println("*****************************");
}
