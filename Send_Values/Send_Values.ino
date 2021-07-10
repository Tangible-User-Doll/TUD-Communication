#include <ArduinoJson.h>
#include "ESP8266HTTPClient.h"
#include "ESP8266WiFi.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const char* ssid     = "XXXXXXXXXXXXXXXXXXXXX";
const char* password = "XXXXXXXXXXXXXXXXXXXXX";

// the ThinkSpeak Key and Channel to read
const char* apiKeyRead = "XXXXXXXXXXXXXXXXXXXXX";
const char* apiChannelFeed = "XXXXXXXXXXXXXXXXXXXXX";

// the ThinksSpeak Key and Channel to write
const char* apiKeyWrite = "XXXXXXXXXXXXXXXXXXXXX";

Adafruit_MPU6050 mpu;

HTTPClient http;
WiFiClientSecure client;

void setup() {
 
  Serial.begin(115200);
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  client.setInsecure();

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");  
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  Serial.println("");
  delay(100);
}
 
void loop() {
  char urlWrite[100];
  char urlRead[100];

  sprintf(urlWrite, "https://api.thingspeak.com/update.json?api_key=%s&field4=%d", apiKeyWrite, getArmPosition());
  writeRGBValues(urlWrite);

  delay(1000);
}

void writeRGBValues(String url){

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

int getArmPosition(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  int armPosition = 0;

  Serial.print("Doll arm position X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);

  if(a.acceleration.x > 9){
     armPosition = 1;
     Serial.print("\nArm is Oben!\n");
  }

  if(a.acceleration.y > 9){
     armPosition = 2;
     Serial.print("\nArm irgendwas....!\n");
  }

  if(a.acceleration.z > 9){
     armPosition = 3;
     Serial.print("\nArm is zur Seite!\n");
  }

  Serial.println("");
  return armPosition;
}
