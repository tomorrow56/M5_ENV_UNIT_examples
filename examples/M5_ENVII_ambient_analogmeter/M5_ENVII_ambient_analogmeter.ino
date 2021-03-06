/*Please install the < Adafruit BMP280 Library > （https://github.com/adafruit/Adafruit_BMP280_Library）
 * from the library manager before use. 
 *This code will display the temperature, humidity and air pressure information on the screen*/

#include <M5Stack.h>
#include "M5StackUpdater.h"
//#include <Wire.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>
#include "SHT3X.h"

// for WiFi Manager
#include <WiFi.h>
#include <DNSServer.h>     // https://github.com/zhouhan0126/DNSServer---esp32
#include <WebServer.h>     // https://github.com/zhouhan0126/WebServer-esp32
#include "WiFiManager.h"   // https://github.com/zhouhan0126/WIFIMANAGER-ESP32

#include <Ambient.h>
Ambient ambient;

#include "analogmeter.h"
ANALOGMETER mtr;
uint8_t x_posi = 30;
uint8_t y_posi = 140;
uint8_t y_interval = 20;

unsigned int channelId = xxxxx; // AmbientのチャネルID
const char* writeKey = "xxxxxxxxxxxxxxxx"; // ライトキー

SHT3X sht30;
Adafruit_BMP280 bme;

float tmp = 0.0;
float hum = 0.0;
float pressure = 0.0;
float disconfort = 0.0;
float preVal = 0.0;

// WiFi instanse
WiFiManager wifiManager;
WiFiClient client;
IPAddress ipadr;

void setup() {
  // M5Stack::begin(LCDEnable, SDEnable, SerialEnable, I2CEnable);
  M5.begin(true, true, true, true);
  M5.Power.begin();
  //Wire.begin();

  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

  M5.Lcd.setBrightness(10);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(WHITE);
  Serial.println(F("ENV Unit(SHT30 and BMP280) test..."));

  while (!bme.begin(0x76)){  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    M5.Lcd.println("Could not find a valid BMP280 sensor, check wiring!");
  }

  // WiFiManager debug message enabled
  wifiManager.setDebugOutput(false);

  // WiFiManager auto connect setting check
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("AP Name: M5StackAP");
  M5.Lcd.println("IP adrs: 192.168.4.1");
  wifiManager.autoConnect("M5StackAP");

  //if you get here you have connected to the WiFi
  ipadr = WiFi.localIP();

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setCursor(0, 0);

  M5.Lcd.println("Wifi Connected!");
  M5.Lcd.println("");
  M5.Lcd.println(" SSID: " + WiFi.SSID());
  M5.Lcd.println(" IP adrs: " + (String)ipadr[0] + "." + (String)ipadr[1] + "." + (String)ipadr[2] + "." + (String)ipadr[3]);

  delay(500);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setCursor(0, 0);

  // チャネルIDとライトキーを指定してAmbientの初期化
  ambient.begin(channelId, writeKey, &client);
  M5.Lcd.println("ambient begin...");
  M5.Lcd.print(" channelID:");
  M5.Lcd.println(channelId);

  delay(500);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
//  M5.Lcd.setTextColor(GREEN);
//  M5.Lcd.setCursor(110, 225);
//  M5.Lcd.print("Ambient");

  mtr.Title = "disconfort";
  mtr.drawMeter(); // Draw analogue meter
  mtr.plotNeedle(0, 0);
}

void loop() {
  M5.update();  // Key scan

  pressure = bme.readPressure();
  if(sht30.get()==0){
    tmp = sht30.cTemp;
    hum = sht30.humidity;
    disconfort = (0.81 * tmp) + ((0.01 * hum) * ((0.99 * tmp) -14.3)) + 46.3;
  }
//  Serial.printf("Temperatura: %2.2f*C  Humidity: %0.2f%%  Pressure: %0.2fPa\r\n", tmp, hum, pressure);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(CYAN, BLACK);

//  M5.Lcd.printf("Temp: %2.1f  \r\nHumi: %2.1f%%  \r\nPressure:%4.1fPa\r\ndisconfort:%2.1f \r\n", tmp, hum, pressure / 100, discomfort);
  M5.Lcd.setCursor(x_posi, y_posi);
  M5.Lcd.printf("Temperature: %2.1f C", tmp);
  M5.Lcd.setCursor(x_posi, y_posi + y_interval);
  M5.Lcd.printf("Humidity   : %2.1f %%", hum);
  M5.Lcd.setCursor(x_posi, y_posi + y_interval * 2);
  M5.Lcd.printf("Pressure   : %4.1f hPa", pressure / 100);
  M5.Lcd.setCursor(x_posi, y_posi + y_interval * 3);
  M5.Lcd.printf("Disconfort : %2.1f", disconfort);

//  mtr.plotNeedle(disconfort, 0);

  if(round(disconfort) > round(preVal)){
    for(int i = round(preVal); i <= round(disconfort); i++){
      mtr.plotNeedle(i, 0);
      delay(20);
    }
  }else if(round(disconfort) < round(preVal)){
    for(int i = round(preVal); i >= round(disconfort); i--){
      mtr.plotNeedle(i, 0);
      delay(20);
    }
  }

  preVal = disconfort;

  // 温度、湿度、気圧の値をAmbientに送信する
  ambient.set(1, String(tmp).c_str());
  ambient.set(2, String(hum).c_str());
  ambient.set(3, String(pressure / 100).c_str());
  ambient.set(4, String(disconfort).c_str());
  ambient.send();

/*  
  if(discomfort <= 75){
    M5.Lcd.fillRect(279, 0, 40, 40, GREEN);
  }else if(disconfort <= 80){
    M5.Lcd.fillRect(279, 0, 40, 40, YELLOW);
  }else if(disconfort <= 85){
    M5.Lcd.fillRect(279, 0, 40, 40, ORANGE);
  }else{
    M5.Lcd.fillRect(279, 0, 40, 40, RED);
  }
 */
 
  M5.Lcd.setTextColor(YELLOW, BLACK);
  M5.Lcd.drawCentreString("DATA SENDING...", 160, 225, 1);
  delay(1000);
  M5.Lcd.setTextColor(BLACK, BLACK);
  M5.Lcd.drawCentreString("DATA SENDING...", 160, 225, 1);

  delay((1000 * 60 * 10) - 1000);
}
