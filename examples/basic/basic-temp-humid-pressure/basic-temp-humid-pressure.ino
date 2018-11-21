/*
 * Sigfoxサンプル
 * UnaShieldのBME280から温度、湿度、気圧をSigfoxで送信します
 * ライブラリマネージャ（スケッチ_ライブラリのインクルード_ライブラリを管理）で
 * Adafruit BME280 Library
 * をインストールしておく必要があります。
 * Copyright (c) 2018 Kyocera Communication Systems, Co,. Ltd.
 * Callback -Custom Payload Config : temperature::float:32 humidity::float:32 pressure::uint:16 altitude::int:16
 */
#include "SIGFOX.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

//必要に応じ書き換えてください
//************************************
static const unsigned int MESSAGE_INTERVAL = 30000;   //Sigfoxメッセージ送信間隔(30秒)
static const unsigned int MAX_MESSAGE_CNT = 10;         //Sigfoxメッセージ最大送信数(10回)
//************************************

unsigned int message_cnt = 0;

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
  Serial.println("Sigfox UnaShield Temp/Humid/Pressure Sample");
  Serial.print("Send a message in "); Serial.print(MAX_MESSAGE_CNT); Serial.print(" times for each "); Serial.print(MESSAGE_INTERVAL/1000); Serial.println(" sec.");
  Serial.println("===========================");
  
  //Sigfoxモジュールを起動
  if (!transceiver.begin()) stop(F("Unable to init SIGFOX module, may be missing"));  //  Will never return.

  //BME280センサ起動
  if (!bme.begin(0x76)) stop("Bosch BME280 sensor missing");  //  Will never return.

  Serial.println("Waiting 3 seconds...");
  delay(3000);
}

void loop()
{ 
  //温度、湿度、気圧を取得する
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  unsigned int pressure = (unsigned int)(bme.readPressure() / 100.0F); //hPa
  int altitude = (int)(bme.readAltitude(SEALEVELPRESSURE_HPA));
    
  if (sendSigfoxMessage(temperature, humidity, pressure, altitude)) 
  {
    message_cnt++;
  }

  if (message_cnt >= MAX_MESSAGE_CNT) 
  {
    stop(String("finish...(Message sent successfully)"));
  }

  Serial.print("Waiting "); Serial.print(MESSAGE_INTERVAL/1000); Serial.println(" seconds...");
  delay(MESSAGE_INTERVAL);  
}

//温度、湿度、気圧、標高をSigfoxメッセージで送信する
bool sendSigfoxMessage(float temperature, float humidity, unsigned int pressure, int altitude) 
{
  Serial.println("\n*****SEND SIGFOX MESSAGE*****");
  String sTemp = convertFloatToHex(temperature);
  String sHumid = convertFloatToHex(humidity);
  String sPressure = transceiver.toHex(pressure);
  String sAltitude = transceiver.toHex(altitude);
  Serial.print("Temp: "); Serial.print(temperature); Serial.print(" *C -> 0x"); Serial.println(sTemp);
  Serial.print("Humid: "); Serial.print(humidity);  Serial.print(" % -> 0x"); Serial.println(sHumid);
  Serial.print("Pressure: "); Serial.print(pressure); Serial.print(" hPa -> 0x"); Serial.println(sPressure);
  Serial.print("Altitude: "); Serial.print(altitude); Serial.print(" m -> 0x"); Serial.println(sAltitude);
  Serial.print("Payload: "); Serial.println(sTemp + sHumid + sPressure + sAltitude);
  bool success = transceiver.sendMessage(sTemp + sHumid + sPressure + sAltitude);
  Serial.println("*****************************");
  return success;
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
