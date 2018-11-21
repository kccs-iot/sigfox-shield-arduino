/*
 * Sigfoxサンプル
 * UnaShieldの温度、バッテリー電圧をSigfoxで送信します
 * ※本サンプルでは温度、バッテリー電圧をFloat型で送りますが、
 * 本来はデータ圧縮のため実値を10倍などして整数型で送ることをお薦めします。
 * Copyright (c) 2018 Kyocera Communication Systems, Co,. Ltd.
 * Callback -Custom Payload Config : count::uint:16:little-endian temperature::float:32 voltage::float:32
 */
#include "SIGFOX.h"

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
  Serial.println("Sigfox UnaShield Basic Sample");
  Serial.print("Send a message in "); Serial.print(MAX_MESSAGE_CNT); Serial.print(" times for each "); Serial.print(MESSAGE_INTERVAL); Serial.println(" msec.");
  Serial.println("===========================");
  
  //Sigfoxモジュールを起動
  if (!transceiver.begin()) stop(F("Unable to init SIGFOX module, may be missing"));  //  Will never return.

  Serial.println("Waiting 3 seconds...");
  delay(3000);
}

void loop()
{ 
  //温度、バッテリー電圧を取得する
  float temperature = 0;
  float voltage = 0;
  transceiver.getTemperature(temperature);
  transceiver.getVoltage(voltage);
  
  if (sendSigfoxMessage(message_cnt, temperature, voltage)) 
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

//送信回数と温度、バッテリー電圧をSigfoxメッセージで送信する
bool sendSigfoxMessage(unsigned int cnt, float temperature, float voltage) 
{
  Serial.println("\n*****SEND SIGFOX MESSAGE*****");
  String sCnt = transceiver.toHex(cnt);
  String sTemp = convertFloatToHex(temperature);
  String sVoltage = convertFloatToHex(voltage);
  Serial.print("Count: "); Serial.print(cnt);
  Serial.print(" / Temperature: "); Serial.print(temperature); 
  Serial.print(" / Voltage: "); Serial.println(voltage);
  Serial.print("Payload: "); Serial.println(sCnt + sTemp + sVoltage);
  bool success = transceiver.sendMessage(sCnt + sTemp + sVoltage);
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
