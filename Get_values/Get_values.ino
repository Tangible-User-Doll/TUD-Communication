#include <ArduinoJson.h>
#include "ESP8266HTTPClient.h"
#include "ESP8266WiFi.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     16 
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

unsigned long timePressLimit;

const char* ssid     = "xxxxxxx";
const char* password = "xxxxxxx";

// the ThinkSpeak key and channel to read your own chanel
const char* apiKeyReadOwn = "xxxxxxx";
const char* apiChannelFeedOwn = "xxxxxxx";

// the ThinkSpeak key and channel to read from the second member
const char* apiKeyReadSecond = "xxxxxxx";
const char* apiChannelFeedSecond = "xxxxxxx";

// the ThinkSpeak key and channel to read from the third member
const char* apiKeyReadThird = "xxxxxxx";
const char* apiChannelFeedThird = "xxxxxxx";

// the key to send data to ThingSpeak
const char* apiKeyWrite = "xxxxxxx";

// just for one LED, the others depend on the hardware connection
int red_light_pin_1 = 1 ;
int green_light_pin_1 = 3;
int blue_light_pin_1 = 10;

int red_light_pin_2 = 13;
int green_light_pin_2 = 12;
int blue_light_pin_2 = 15;

int red_light_pin_3 = 2;
int green_light_pin_3 = 0;
int blue_light_pin_3 = 14;

HTTPClient http;
WiFiClientSecure client;

const char* pos_user_1; 
const char* pos_user_2; 
const char* pos_user_3; 

const int sensor_pin = 9; 

int touch_lastState = LOW;  
int touch_currentState;
 
void setup() {
 
  Serial.begin(9600);
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  client.setInsecure();

  Serial.println("WiFi is connected...");

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
      
  pinMode(red_light_pin_1, OUTPUT);
  pinMode(green_light_pin_1, OUTPUT);
  pinMode(blue_light_pin_1, OUTPUT);

  pinMode(red_light_pin_2, OUTPUT);
  pinMode(green_light_pin_2, OUTPUT);
  pinMode(blue_light_pin_2, OUTPUT);

  pinMode(red_light_pin_3, OUTPUT);
  pinMode(green_light_pin_3, OUTPUT);
  pinMode(blue_light_pin_3, OUTPUT);

  // activate touch sensor
  pinMode(sensor_pin, INPUT);

  Serial.println("Pin Mode is done...");

  setColorOwn(0,0,0);
  setColorUserTwo(0,0,0);
  setColorUserThree(0,0,0);
  display.display();
}
 
void loop() {
  Serial.println("Loop is starting...");
  
  // create an url string which is sent to thinkspeak
  char urlRead[100];
  sprintf(urlRead, "https://api.thingspeak.com/channels/%s/feeds.json?api_key=%s&results=1", apiChannelFeedOwn, apiKeyReadOwn);
  readRGBValues(urlRead);
  touchSensor();
}

void decodeJson(String response){
  
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, response);

  Serial.print("Read from JSON Doc...\n");
  if(doc["feeds"][0]["field1"]){
    pos_user_1 = doc["feeds"][0]["field1"]; 
    Serial.println(pos_user_1);
  }
  else{
    Serial.println(pos_user_1);
  }

  if(doc["feeds"][0]["field2"]){
    pos_user_2 = doc["feeds"][0]["field2"]; 
    Serial.println(pos_user_2);
  }
  else{
    Serial.println(pos_user_2);
  }

  if(doc["feeds"][0]["field3"]){
    pos_user_3 = doc["feeds"][0]["field3"]; 
    Serial.println(pos_user_3);
  }
  else{
    Serial.println(pos_user_3);
  }

  int posInt_1 = atoi(pos_user_1);
  int posInt_2 = atoi(pos_user_2);
  int posInt_3 = atoi(pos_user_3);
  
  Serial.println("pos 1: ");
  Serial.println(posInt_1);
  Serial.println("\n");

  Serial.println("pos 2: ");
  Serial.println(posInt_2);
  Serial.println("\n");

  Serial.println("pos 3: ");
  Serial.println(posInt_3);
  Serial.println("\n");

  delay(500);
  setColorOwn(0, 0, 0);
  switchUserColorsByPosition(posInt_1, 1);
  delay(500);
  setColorUserTwo(0,0,0);
  switchUserColorsByPosition(posInt_2, 2);
  delay(500);
  setColorUserThree(0,0,0);
  switchUserColorsByPosition(posInt_3, 3);
  
}

void switchUserColorsByPosition(int pos, int user){

  if(user == 1){
    switch (pos) {
    case 0:
      setColorOwn(100, 100, 75);        // white (neutral)
      break;
    case 1:
      setColorOwn(0, 255, 0);           // green (joy, happiness)
      break;
    case 2:
      setColorOwn(255, 75, 0);          // yellow (stress)
      break;
    case 3:
      setColorOwn(0, 200, 200);         // cyan (concentration)
      break;
    case 4:
      setColorOwn(0, 0, 255);           // blue (sadness)
      break;
    case 5:
      setColorOwn(255, 0, 0);           // red (anger)
      break;
    }
  }

  if(user == 2){
    switch (pos) {
    case 0:
      setColorUserTwo(100, 100, 75);    // white (neutral)
      break;
    case 1:
      setColorUserTwo(0, 255, 0);       // green (joy, happiness)
      break;
    case 2:
      setColorUserTwo(255, 75, 0);      // yellow (stress)
      break;
    case 3:
      setColorUserTwo(0, 200, 200);     // cyan (concentration)
      break;
    case 4:
      setColorUserTwo(0, 0, 255);       // blue (sadness)
      break;
    case 5:
      setColorUserTwo(255, 0, 0);       // red (anger)
      break;
    }
  }

  if(user == 3){
    switch (pos) {
    case 0:
      setColorUserThree(100, 100, 75);  // white (neutral)
      break;
    case 1:
      setColorUserThree(0, 255, 0);     // green (joy, happiness)
      break;
    case 2:
      setColorUserThree(255, 75, 0);    // yellow (stress)
      break;
    case 3:
      setColorUserThree(0, 200, 200);   // cyan (concentration)
      break;
    case 4:
      setColorUserThree(0, 0, 255);     // blue (sadness)
      break;
    case 5:
      setColorUserThree(255, 0, 0);     // red (anger)
      break;
    }
  }
}

void readRGBValues(String url){

 if(WiFi.status()== WL_CONNECTED){

   Serial.println("Try to get data from ThinkSpeak...");
   http.begin(client, url);
   http.addHeader("Content-Type", "application/json");            
 
   int httpResponseCode = http.GET();   

   if(httpResponseCode>0){
     Serial.println("Read data from ThinkSpeak...");
     String response = http.getString();   

     char responseText[50];
     sprintf(responseText, "Response Code: %d\n", httpResponseCode);
     Serial.print(responseText);

     
     decodeJson(response);
 
   }else{
      Serial.print("Error on sending PUT Request: ");
      Serial.println(httpResponseCode);
   }
   http.end();
 
}else{
    Serial.println("Error in WiFi connection");
 }
}

void setColorOwn(int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(red_light_pin_1, red_light_value);
  analogWrite(green_light_pin_1, green_light_value);
  analogWrite(blue_light_pin_1, blue_light_value);
}

void setColorUserTwo(int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(red_light_pin_2, red_light_value);
  analogWrite(green_light_pin_2, green_light_value);
  analogWrite(blue_light_pin_2, blue_light_value);
}

void setColorUserThree(int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(red_light_pin_3, red_light_value);
  analogWrite(green_light_pin_3, green_light_value);
  analogWrite(blue_light_pin_3, blue_light_value);
}

void touchSensor(){
  touch_currentState = digitalRead(sensor_pin);

  if(touch_lastState == LOW && touch_currentState == HIGH){
    char urlWrite[100];
    sprintf(urlWrite, "https://api.thingspeak.com/update.json?api_key=%s&field4=%d", apiKeyWrite, 1);
    sendTextValue(urlWrite);

    timePressLimit = millis() + 30000;    
  }

  if(timePressLimit > 0 && millis() < timePressLimit){
    char urlWrite[100];
    sprintf(urlWrite, "https://api.thingspeak.com/update.json?api_key=%s&field4=%d", apiKeyWrite, 0);
    sendTextValue(urlWrite);
  }

  touch_lastState = touch_currentState;
}

void sendTextValue(String url){

 if(WiFi.status()== WL_CONNECTED){
  
   http.begin(client, url);
   http.addHeader("Content-Type", "application/json");            
 
   int httpResponseCode = http.GET();   

   if(httpResponseCode > 0){
     Serial.println("Write data to ThinkSpeak...");
     String response = http.getString();   

     char responseText[50];
     sprintf(responseText, "Response Code: %d\n", httpResponseCode);
     Serial.print(responseText);
 
   }else{
      Serial.print("Error on sending PUT Request: ");
      Serial.println(httpResponseCode);
   }
   http.end();
 
 }else{
    Serial.println("Error in WiFi connection");
 }
}
