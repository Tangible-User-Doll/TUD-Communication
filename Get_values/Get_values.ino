#include <ArduinoJson.h>
#include "ESP8266HTTPClient.h"
#include "ESP8266WiFi.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const char* ssid     = "XXXXXXXXXXXXXXXXXXXXX";
const char* password = "XXXXXXXXXXXXXXXXXXXXX";

// the ThinkSpeak key and channel to read your own chanel
const char* apiKeyReadOwn = "XXXXXXXXXXXXXXXXXXXXX";
const char* apiChannelFeedOwn = "XXXXXXXXXXXXXXXXXXXXX";

// the ThinkSpeak key and channel to read from the second member
const char* apiKeyReadSecond = "XXXXXXXXXXXXXXXXXXXXX";
const char* apiChannelFeedSecond = "XXXXXXXXXXXXXXXXXXXXX";

// the ThinkSpeak key and channel to read from the third member
const char* apiKeyReadThird = "XXXXXXXXXXXXXXXXXXXXX";
const char* apiChannelFeedThird = "XXXXXXXXXXXXXXXXXXXXX";

// just for one LED, the others depend on the hardware connection
int red_light_pin= 15;
int green_light_pin = 14;
int blue_light_pin = 13;

HTTPClient http;
WiFiClientSecure client;

const char* pos;
 
void setup() {
 
  Serial.begin(9600);
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  client.setInsecure();

  Serial.println("");
  delay(100);
      
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
}
 
void loop() {
  char urlRead[100];
  
  // create an url string which is sent to thinkspeak
  sprintf(urlRead, "https://api.thingspeak.com/channels/%s/feeds.json?api_key=%s&results=1", apiChannelFeedOwn, apiKeyReadOwn);
  readRGBValues(urlRead);

  delay(1000);
}

void decodeJson(String response){ 
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, response);
    
  pos = doc["feeds"][0]["field4"];
  Serial.println("pos");
  Serial.println(pos);
  int posInt = atoi(pos);

  // for example: 0 === arms are above
  if(posInt == 0){
    setColor(255, 255, 255);
  }
  if(posInt == 1){
    setColor(255, 0, 0);
  }
  if(posInt == 2){
    setColor(0, 255, 0);
  }
  Serial.println("\n");
  Serial.println("-------------------");  
}

void readRGBValues(String url){

 if(WiFi.status()== WL_CONNECTED){
  
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

void setColor(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}
