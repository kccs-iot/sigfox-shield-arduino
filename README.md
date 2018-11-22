# sigfox-shield-arduino
Sigfox Shield for Arduino (UnaShield) - Japan samples
ZIPダウンロード後、Arduino IDEでダウンロードしたZIPファイルを指定して、ライブラリをインクルードしてください。

## Sigfox Shield for Arduino (UnaShield)の入手方法
下記サイトからご購入ください。
https://www.marutsu.co.jp/pc/i/952284/
http://eleshop.jp/shop/g/gHAR311/
https://www.switch-science.com/catalog/3354/
https://soracom.jp/products/sigfox/

# サンプルスケッチ

## basic-demo
Sigfoxモジュールの温度と入力電圧を一定間隔で送信します。
Custom Payload Configの設定は、"count::uint:16:little-endian temperature::float:32 voltage::float:32"がお薦めです。
※このサンプルでは、温度と入力電圧をFloat型として送信していますが、データサイズを圧縮するためにも、整数型で送信し、受信アプリケーションサーバ側でFloat型に変換することが重要です。

## basic-button
シールド上のボタンを押したときに、Sigfoxメッセージを送信します。
Custom Payload Configの設定は、"button::bool:7"がお薦めです。

## basic-temp-humid-pressure
シールド上のBME280から温度、湿度、気圧を送信します。
ライブラリマネージャ（スケッチ_ライブラリのインクルード_ライブラリを管理）で"Adafruit BME280 Library"をインストールしておく必要があります。
Custom Payload Configの設定は、"temperature::float:32 humidity::float:32 pressure::uint:16 altitude::int:16"がお薦めです。

## grove-gps
Grove GPSセンサの緯度経度を送信します。
シールド上のGrove(Analog)ポートに接続してください。下記GPSセンサで動作確認しています。
http://wiki.seeedstudio.com/Grove-GPS/
Custom Payload Configの設定は、"lat::float:32 lng::float:32"がお薦めです。

## grove-light
Digital Light Sensor (TSL2561)から、明暗変化時に、取得した照度を送信します。
Grove(I2C)ポートにセンサを接続してください。
http://wiki.seeedstudio.com/Grove-Digital_Light_Sensor/
Custom Payload Configの設定は、"lux::uint:16:little-endian isBright::bool:7"がお薦めです。

## grove-ultrasonic
Grove Ultrasonic Rangerから物体検出時に想定距離と共にSigfoxメッセージを送信します。
Grove(Analog)ポートにセンサを接続してください。
http://wiki.seeedstudio.com/Grove-Ultrasonic_Ranger/
Custom Payload Configの設定は、"distance::uint:16:little-endian isDetected::bool:7"がお薦めです。

