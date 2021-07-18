#include <ArduinoJson.h>
#include "ESP8266HTTPClient.h"
#include "ESP8266WiFi.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const char* ssid     = "xxxxxxx";
const char* password = "xxxxxxx";

// the ThinksSpeak key and channel to write to your own channel
const char* apiKeyWrite = "xxxxxxx";

// AD0 to ground
Adafruit_MPU6050 mpu_right;

// AD0 to vcc
Adafruit_MPU6050 mpu_left;


HTTPClient http;
WiFiClientSecure client;

void setup() {
  Wire.begin();
  
  Serial.begin(115200);
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  client.setInsecure();

  // Try to initialize first mpu!
  if (!mpu_right.begin(0x68)) {
    Serial.println("Failed to find MPU6050 chip at address 0x68");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 at address 0x68 found!");

  mpu_right.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu_right.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu_right.setFilterBandwidth(MPU6050_BAND_5_HZ);
  

  // Try to initialize first mpu!
  if (!mpu_left.begin(0x69)) {
    Serial.println("Failed to find MPU6050 chip at address 0x69");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 at address 0x69 found!");

  mpu_left.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu_left.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu_left.setFilterBandwidth(MPU6050_BAND_5_HZ);
}
 
void loop() {
  char urlWrite[100];
  char urlRead[100];

  // getArmPosition() returns the current arm position based on the gyro values
  sprintf(urlWrite, "https://api.thingspeak.com/update.json?api_key=%s&field1=%d", apiKeyWrite, getEmotion());
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

     // if code is used to get Data uncomment this
     //decodeJson(response);
 
   }else{
      Serial.print("Error on sending PUT Request: ");
      Serial.println(httpResponseCode);
   }
   http.end();
 
 }else{
    Serial.println("Error in WiFi connection");
 }
}

int getEmotion() {
  String rightPos = getMPUAccelerationRight();
  String leftPos = getMPUAccelerationLeft();
  int armPosition = 0;
  
  if(rightPos == "seitwärts" && leftPos == "seitwärts") {
    Serial.print("Standard-Position");
    armPosition = 0;
  }
  if(rightPos == "oben" || leftPos == "oben") {
    Serial.print("Freude");
    armPosition = 1;
  }
  if(rightPos == "gesicht" || leftPos == "gesicht") {
    Serial.print("Angst");
    armPosition = 2;
  }
  if((rightPos == "kopf" && leftPos == "seitwärts") || (rightPos == "seitwärts" && leftPos == "kopf")) {
    Serial.print("Konzentration");
    armPosition = 3;
  }
  
  return armPosition;
  Serial.println("");
  delay(1000);
}

String getMPUAccelerationRight() {
  sensors_event_t a, g, temp;
  mpu_right.getEvent(&a, &g, &temp);

  Serial.print("X-rechts:");
  Serial.print(a.acceleration.x);
  Serial.print("Y-rechts:");
  Serial.print(a.acceleration.y);
  Serial.print("Z-rechts:");
  Serial.print(a.acceleration.z);
  Serial.println("");

  if(a.acceleration.x < 3 && a.acceleration.x > -1) {
    if(a.acceleration.y < -8 && a.acceleration.y > -11) {
      if(a.acceleration.z < 1 && a.acceleration.z > -3) {
        Serial.print("rechts seitwärts");
        return "seitwärts";
      }
    }
  }

  if(a.acceleration.x < 11 && a.acceleration.x > 7) {
    if(a.acceleration.y < -1 && a.acceleration.y > -7) {
      if(a.acceleration.z < 6 && a.acceleration.z > 0) {
        Serial.print("rechts oben");
        return "oben";
      }
    }
  }

  if(a.acceleration.x < 8 && a.acceleration.x > 4) {
    if(a.acceleration.y < -6 && a.acceleration.y > -10) {
      if(a.acceleration.z < 5 && a.acceleration.z > 1) {
        Serial.print("rechts gesicht");
        return "gesicht";
      }
    }
  }

  if(a.acceleration.x < 10 && a.acceleration.x > 6) {
    if(a.acceleration.y < 1 && a.acceleration.y > -5) {
      if(a.acceleration.z < 7 && a.acceleration.z > 3) {
        Serial.print("rechts kopf");
        return "kopf";
      }
    }
  }

  delay(1000);
}

String getMPUAccelerationLeft() {
  sensors_event_t a, g, temp;
  mpu_left.getEvent(&a, &g, &temp);

  Serial.print("X-links:");
  Serial.print(a.acceleration.x);
  Serial.print("Y-links:");
  Serial.print(a.acceleration.y);
  Serial.print("Z-links:");
  Serial.print(a.acceleration.z);
  Serial.println("");

  if(a.acceleration.x < 2 && a.acceleration.x > -4) {
    if(a.acceleration.y < 11 && a.acceleration.y > 6) {
      if(a.acceleration.z < 2 && a.acceleration.z > -5) {
        Serial.print("links seitwärts");
        return "seitwärts";
      }
    }
  }

  if(a.acceleration.x < 11 && a.acceleration.x > 7) {
    if(a.acceleration.y < 5 && a.acceleration.y > 0) {
      if(a.acceleration.z < 5 && a.acceleration.z > -2) {
        Serial.print("links oben");
        return "oben";
      }
    }
  }

  if(a.acceleration.x < 10 && a.acceleration.x > 6) {
    if(a.acceleration.y < 8 && a.acceleration.y > 4) {
      if(a.acceleration.z < 5 && a.acceleration.z > -1) {
        Serial.print("links gesicht");
        return "gesicht";
      }
    }
  }

  /*if(a.acceleration.x < 11 && a.acceleration.x > 8) {
    if(a.acceleration.y < 3 && a.acceleration.y > -3) {
      if(a.acceleration.z < 5 && a.acceleration.z > 1) {
        Serial.print("links kopf");
        return "kopf";
      }
    }
  }*/

  delay(1000);
}
