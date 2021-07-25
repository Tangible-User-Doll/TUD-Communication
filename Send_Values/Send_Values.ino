#include <ArduinoJson.h>
#include "ESP8266HTTPClient.h"
#include "ESP8266WiFi.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// change your wifi credentials here
const char* ssid     = "xxxxxx";
const char* password = "xxxxxx";

// AD0 to ground
Adafruit_MPU6050 mpu_right;

// AD0 to vcc
Adafruit_MPU6050 mpu_left;


HTTPClient http;
WiFiClient client;

void setup() {
  Wire.begin();
  
  Serial.begin(115200);
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  // only necesarry for thingspeak  
  // client.setInsecure();
  
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
  // Change the p3 to your user intpu e. g. if you are user 1 then change to p1
  sprintf(urlWrite, "xxxxxx/changeColor?p3=%d", getEmotion());
  writeRGBValues(urlWrite);

  //delay(1000);
}

// Change the xxx to the url from the docker container
void writeRGBValues(String url){

  if(WiFi.status()== WL_CONNECTED){
    client.connect("xxxxxx", 80);
    http.begin(client,url);    
    http.GET();
    Serial.println(url);
    http.end();
 
   }else{
      Serial.print("Error on sending PUT Request: ");
   }
}


int getEmotion() {
  String rightPos = getMPUAccelerationRight();
  String leftPos = getMPUAccelerationLeft();
  int armPosition = 0;
  
  if(rightPos == "oben" || leftPos == "oben") {
    Serial.print("Freude");
    armPosition = 1;
  }
  if(rightPos == "gesicht" || leftPos == "gesicht") {
    Serial.print("Stress");
    armPosition = 2;
  }
  if((rightPos == "kopf" || leftPos == "kopf")) {
    Serial.print("Konzentration");
    armPosition = 3;
  }
  if(rightPos == "unten" || leftPos == "unten") {
    Serial.print("Trauer");
    armPosition = 4;
  }
  if(rightPos == "vorne" || leftPos == "vorne") {
    Serial.print("Wut");
    armPosition = 5;
  }
  
  return armPosition;
  Serial.println("");
  //delay(1000);
}

String getMPUAccelerationRight() {
  sensors_event_t a, g, temp;
  mpu_right.getEvent(&a, &g, &temp);
  int x = a.acceleration.x;
  int y = a.acceleration.y;
  int z = a.acceleration.z;
  String armRight;

  if(x < 11 && x > 7) {
    if(y < -1 && y > -7) {
      if(z < 6 && z > 0) {
        armRight = "oben";
      }
    }
  }

  if(x < 8 && x > 4) {
    if(y < -6 && y > -10) {
      if(z < 5 && z > 1) {
        armRight = "gesicht";
      }
    }
  }

  if(x < 10 && x > 7) {
    if(y < 3 && y > -2) {
      if(z < 5 && z > 1) {
        armRight = "kopf";
      }
    }
  }

  if(x < -7 && x > -11) {
    if(y < 1 && y > -2) {
      if(z < -2 && z > -6) {
        armRight = "unten";
      }
    }
  }

  if(x < 2 && x > -2) {
    if(y < -9 && y > -11) {
      if(z < 0 && z > -3) {
        armRight = "vorne";
      }
    }
  }
  return armRight;
}

String getMPUAccelerationLeft() {
  sensors_event_t a, g, temp;
  mpu_left.getEvent(&a, &g, &temp);
  int x = a.acceleration.x;
  int y = a.acceleration.y;
  int z = a.acceleration.z;
  String armLeft;
  
  if(x < 11 && x > 7) {
    if(y < 5 && y > 0) {
      if(z < 5 && z > -2) {
        armLeft = "oben";
      }
    }
  }

  if(x < 10 && x > 6) {
    if(y < 8 && y > 4) {
      if(z < 5 && z > -1) {
        armLeft = "gesicht";
      }
    }
  }

  if(x < 10 && x > 6) {
    if(y < -1 && y > -5) {
      if(z < 8 && z > 3) {
        armLeft = "kopf";
      }
    }
  }

  if(x < -7 && x > -11) {
    if(y < 4 && y > 1) {
      if(z < 0 && z > -4) {
        armLeft = "unten";
      }
    }
  }

  if(x < 2 && x > -2) {
    if(y < 11 && y > 9) {
      if(z < 2 && z > 0) {
        armLeft = "vorne";
      }
    }
  }
  return armLeft;
}
