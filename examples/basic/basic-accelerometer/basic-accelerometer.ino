/*
 * Sigfox Accelerometerサンプル
 * UnaShieldの加速度センサが一定の動作を検知したらメッセージをSigfoxで送信します
 * ライブラリマネージャ（スケッチ_ライブラリのインクルード_ライブラリを管理）で
 * Adafruit Unified Sensor と Adafruit MMA8451 Library
 * をインストールしておく必要があります。
 * Copyright (c) 2018 Kyocera Communication Systems, Co,. Ltd.
 * Callback - Custom Payload Config : acc::float:32 orientation::char:3
 */
#include "SIGFOX.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MMA8451.h>

//必要に応じ書き換えてください
//************************************
static const bool isDebug = true;
static const float ACC_THRESHOLD = 1; //移動判定閾値
//************************************

Adafruit_MMA8451 mma = Adafruit_MMA8451();

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
  Serial.println("Sigfox UnaShield Accelerometer Sample");
  Serial.println("Send a message when the UnaShield is moved.");
  Serial.println("===========================");
  
  //Sigfoxモジュールを起動
  if (!transceiver.begin()) stop(F("Unable to init SIGFOX module, may be missing"));  //  Will never return.

  //加速度センサ(Adafruit MMA8451)を起動
  if (! mma.begin(0x1c)) { ////  //  NOTE: Must use 0x1c for UnaShield V2S
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!");
  
  mma.setRange(MMA8451_RANGE_2_G);
  
  Serial.print("Range = "); Serial.print(2 << mma.getRange());  
  Serial.println("G");
}

void loop()
{ 
  //加速度センサのRawデータ(14-bit)を取得する
  mma.read();
  if (isDebug) displayMM8451RawData();

  //加速度センサイベントを取得する
  sensors_event_t event; 
  mma.getEvent(&event);
  if (isDebug) displayMM8451AccData(event);

  //加速度センサから方向を取得する
  uint8_t o = mma.getOrientation();
  if (isDebug) displayMM8451Orientation(o);

  //加速度を計算
  float acc = calcAcc(event.acceleration.x, event.acceleration.y, event.acceleration.z);

  //所定の加速度以上であれば、Sigfoxメッセージ送信
  if (abs(acc - 9.8) >= ACC_THRESHOLD)
  {
    sendSigfoxMessage(acc, o);
    delay(2000);
  }

  delay(500);
}

//ボタン押下をSigfoxメッセージで送信する
void sendSigfoxMessage(float acc, uint8_t orientation) 
{
  Serial.println("\n*****SEND SIGFOX MESSAGE*****");
  String sAcc = convertFloatToHex(acc);
  String sOrientation = convertOrientationToHex(orientation);
  Serial.print("ACC: "); Serial.print(acc); Serial.print(" -> 0x"); Serial.println(sAcc);
  Serial.print("Orientation: "); Serial.print(orientation); Serial.print(" -> 0x"); Serial.println(sOrientation);
  Serial.print("Payload: "); Serial.println(sAcc + sOrientation);  
  transceiver.sendMessage(sAcc + sOrientation);
  Serial.println("*****************************");
}

//XYZ値から加速度を計算
float calcAcc(float x, float y, float z)
{
  float acc =  sqrt(x * x + y * y + z * z);
  Serial.print("ACC: "); Serial.print(acc); 
  Serial.print("\tX: "); Serial.print(x);
  Serial.print("\tY: "); Serial.print(y);
  Serial.print("\tZ: "); Serial.println(z);
  return acc;
}

//加速度センサのRawデータ(14-bit)を表示する
void displayMM8451RawData()
{
  Serial.print("X:\t"); Serial.print(mma.x); 
  Serial.print("\tY:\t"); Serial.print(mma.y); 
  Serial.print("\tZ:\t"); Serial.print(mma.z); 
  Serial.println();
}

//加速度センサのXYZ値を表示する
void displayMM8451AccData(sensors_event_t event)
{
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: \t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print("Y: \t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print("Z: \t"); Serial.print(event.acceleration.z); Serial.print("\t");
  Serial.println("m/s^2 ");
}

//加速度センサの方向を表示する
void displayMM8451Orientation(uint8_t o)
{
  switch (o) {
    case MMA8451_PL_PUF: 
      Serial.println("Portrait Up Front");
      break;
    case MMA8451_PL_PUB: 
      Serial.println("Portrait Up Back");
      break;    
    case MMA8451_PL_PDF: 
      Serial.println("Portrait Down Front");
      break;
    case MMA8451_PL_PDB: 
      Serial.println("Portrait Down Back");
      break;
    case MMA8451_PL_LRF: 
      Serial.println("Landscape Right Front");
      break;
    case MMA8451_PL_LRB: 
      Serial.println("Landscape Right Back");
      break;
    case MMA8451_PL_LLF: 
      Serial.println("Landscape Left Front");
      break;
    case MMA8451_PL_LLB: 
      Serial.println("Landscape Left Back");
      break;
    }
  Serial.println();
}

String convertOrientationToHex(uint8_t o)
{
  String sOrientation = "";
  switch (o) {
    case MMA8451_PL_PUF: 
      sOrientation = "505546";
      break;
    case MMA8451_PL_PUB: 
      sOrientation = "505542";
      break;    
    case MMA8451_PL_PDF: 
      sOrientation = "504446";
      break;
    case MMA8451_PL_PDB: 
      sOrientation = "504442";
      break;
    case MMA8451_PL_LRF: 
      sOrientation = "4C5246";
      break;
    case MMA8451_PL_LRB: 
      sOrientation = "4C5242";
      break;
    case MMA8451_PL_LLF: 
      sOrientation = "4C4C46";
      break;
    case MMA8451_PL_LLB: 
      sOrientation = "4C4C42";
      break;
    }
  return sOrientation;
}

//Float型変数をIEEE754に準拠した16進文字列へ変換する
String convertFloatToHex(float val) 
{
  union {
    uint32_t B32;
    float Float;
  } floatb32;

  floatb32.Float = val;
  return String(floatb32.B32, HEX);
}
