/*
 * Tiny GPS Sensor () - Sigfoxサンプル
 * Copyright (c) 2018 Kyocera Communication Systems, Co,. Ltd.
 * Callback - Custom Payload Config : lat::float:32 lng::float:32
 */
#include "SIGFOX.h"
#include "TinyGPS++.h"
#include <SoftwareSerial.h>

//必要に応じ書き換えてください
//************************************
static const bool isDebug = true;
static const double DISTANCE = 5; //DISTANCE(m)移動時にSigfox送信
//************************************

TinyGPSPlus gps;
SoftwareSerial SoftSerial(A3,A2);
float lastLat = 0;
float lastLng = 0;

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
  SoftSerial.begin(9600);   // Arduinoソフトウェアシリアルを起動 (Arduino <-> GPS)
  Serial.begin(9600);         // Arduinoハードウェアシリアルを起動(Arduino <-> PC)
  Serial.println("===========================");
  Serial.println("Sigfox UnaShield Grove GPS Sample");
  Serial.print("Send a message when GPS moves "); Serial.print(DISTANCE); Serial.println(" m.");
  Serial.println("===========================");

  //Sigfoxモジュールを起動
  if (!transceiver.begin()) stop(F("Unable to init SIGFOX module, may be missing"));  //  Will never return.

}

void loop()
{ 
  bool newData = false; //GPSデータのencodeに成功したかどうか？

  SoftSerial.listen();  //SoftSerialをlisten (UnaShieldのモジュールシリアルとの関係上、必須）

  //1秒間隔でGPSデータを取得する
  for (unsigned long start = millis(); millis() - start < 1000;) 
  {
    while (SoftSerial.available()) 
    {
      char c = SoftSerial.read();
      if (isDebug) Serial.write(c);
      if (gps.encode(c)) 
      {
        newData = true;
      }
    }
  }
  
  if (newData)
  {
    if (gps.location.isUpdated()) 
    {
      monitorGpsInfo(); //GPSデコード情報を表示
      //GPS緯度経度取得
      float lat = (float)gps.location.lat();
      float lng = (float)gps.location.lng();
      //以前の緯度経度との距離を取得
      double dist = getDistanceBetween(lat, lng, lastLat, lastLng);
      if (dist >= DISTANCE)   //GPS緯度経度が移動していたら
      {
        //Sigfoxで緯度経度を送信
        sendSigfoxMessage(lat, lng);
        lastLat = lat;
        lastLng = lng;
        delay(2000);
      }
    }
  }
}

//GPS緯度経度をSigfoxメッセージで送信する
void sendSigfoxMessage(float lat, float lng) 
{
  Serial.println("\n*****SEND SIGFOX MESSAGE*****");
  String sLat = convertFloatToHex(lat);
  String sLng = convertFloatToHex(lng);
  Serial.print("Latitude: "); Serial.print(lat, 5); Serial.print(" -> 0x"); Serial.println(sLat);
  Serial.print("Longitude: "); Serial.print(lng, 5); Serial.print(" -> 0x"); Serial.println(sLng);
  transceiver.sendMessage(sLat + sLng);
  Serial.println("*****************************");
}

//GPS情報を表示する
void monitorGpsInfo() 
{
  Serial.println("\n*****GPS INFORMATION*****");
  Serial.print("Location: ");  Serial.print(gps.location.lat(), 5); Serial.print(", "); Serial.println(gps.location.lng(), 5);
  Serial.print("Time: "); Serial.print(gps.time.hour()); Serial.print(":"); Serial.print(gps.time.minute()); Serial.print(":"); Serial.println(gps.time.second());
  Serial.println("*************************");   
}

//2点間の距離(m)を取得する
double getDistanceBetween(float currLat, float currLng, float lastLat, float lastLng)
{
  double distance = gps.distanceBetween(lastLat, lastLng, currLat, currLng);
  Serial.println("\n*****DISTANCE BETWEEN*****");
  Serial.print("Location(last): "); Serial.print(lastLat, 5); Serial.print(", "); Serial.println(lastLng, 5);
  Serial.print("Location(curr): "); Serial.print(currLat, 5); Serial.print(", "); Serial.println(currLng, 5);
  Serial.print("Distance: "); Serial.print(distance); Serial.println("(m)");
  Serial.println("*************************");   
  return distance;
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
