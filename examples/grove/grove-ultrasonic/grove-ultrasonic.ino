/*
 * Ultrasonic Ranger - Sigfoxサンプル
 * UnaShieldのGrove(Analog)ポートにセンサを接続してください
 * http://wiki.seeedstudio.com/Grove-Ultrasonic_Ranger/
 * Copyright (c) 2018 Kyocera Communication Systems, Co,. Ltd.
 * 
 * The device(unashield v2s) uses an ultrasonic ranger (sensor) to detect the presence of an object within an area (100cm from the sensor).
 * If an object is detected by the ultrasonic ranger within this area, a message will be sent to the user.
 * The device will as well send a message to the user when the object leaves the detection area.  
 * 
 * Callback - Custom Payload Config : distance::uint:16:little-endian isDetected::bool:7
 */
#include "SIGFOX.h"
#include "Ultrasonic.h"
#include <Wire.h>

//必要に応じ書き換えてください
//************************************
static const bool isDebug = true;
static const int DISTANCE_DETECTED = 100; //検知距離(cm)
//************************************

Ultrasonic ultrasonic(A3);
const int ledPin = 9;
bool lastDetectState = false;

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
  Serial.println("Sigfox UnaShield Grove Ultrasonic Sample");
  Serial.print("Send a message when someone is deteced within "); Serial.print(DISTANCE_DETECTED); Serial.println(" (cm).");
  Serial.println("===========================");
  
  //Sigfoxモジュールを起動
  if (!transceiver.begin()) stop(F("Unable to init SIGFOX module, may be missing"));  //  Will never return.

  //物体検知時LED
  pinMode(ledPin, OUTPUT);
}

void loop()
{ 
  //Ultrasonicセンサから距離(cm)を取得
  long rangeCm = ultrasonic.MeasureInCentimeters();
  if (isDebug) {Serial.print("Range(cm): "); Serial.println(rangeCm);}
  
  //物体検知判断
  bool detectState = (rangeCm < DISTANCE_DETECTED) ? true : false;
  
  //LEDを検知有無にあわせてON/OFFする
  digitalWrite(ledPin, detectState);
  //検知有無の変化時にSigfoxで距離を送信
  if (detectState != lastDetectState)
  {
    sendSigfoxMessage((unsigned int)rangeCm, detectState);
    lastDetectState = detectState;
    delay(2000);
  }
  delay(500);  
}

//距離(cm)と検知有無をSigfoxメッセージで送信する
void sendSigfoxMessage(unsigned int distCm, bool isDetected) 
{
  Serial.println("\n*****SEND SIGFOX MESSAGE*****");
  String sDistCm = transceiver.toHex(distCm);
  String sIsDetected = (isDetected) ? "80" : "00"; //0x80(1000000) / 0x00(00000000)
  Serial.print("IsDetected: "); Serial.println(isDetected);
  Serial.print("Payload: "); Serial.println(sDistCm + sIsDetected);
  transceiver.sendMessage(sDistCm + sIsDetected);
  Serial.println("*****************************");
}
