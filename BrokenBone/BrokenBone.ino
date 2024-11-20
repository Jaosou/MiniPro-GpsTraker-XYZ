#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
/* #define Buzzer D5
#define Button D0 */
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int valx, valy, valz;
int stat = 0;
//------------------------------lineNotify--------------------------

#include <ESP8266WiFi.h>
#define SSID "Jaosou"
#define PASSWORD "30082022"

//------------------------------------------------------------------

void setup() {
  Wire.begin(D6,D5);
  Serial.begin(38400);
  Serial.println("Initialize MPU");
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");
  //--------------------------------------------------------
  Serial.println();

  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }

  //--------------------------------------------------------
}
void loop() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  valx = map(ax, -17000, 17000, 0, 179);
  valy = map(ay, -17000, 17000, 0, 179);
  valz = map(az, -17000, 17000, 0, 179);
  if (valx >= 0 && valx <= 179 && valy >= 0 && valy <= 179) {
    if (stat == 0) {
      Serial.print("axis x = ");
      Serial.print(valx);
      Serial.print(" axis y = ");
      Serial.print(valy);
      Serial.print(" axis z = ");
      Serial.println(valz);
      if (valx < 20) {  //ใชปรับระยะในการลมไปดานหนา

        Serial.println("ลมไปดานหนา");

        stat = 1;
      } else if (valx > 160) {  //ใชปรับระยะในการลมไปดานหลัง

        Serial.println("ลมไปดานหลัง");

        stat = 1;
      }
      if (valy < 20) {  //ใชปรับระยะในการลมไปดานซาย

        Serial.println("ลมไปดานซาย");

        stat = 1;
      } else if (valy > 160) {  //ใชปรับระยะในการลมไปดานขวา

        Serial.println("ลมไปดาขวา");

        stat = 1;
      }
    }else{
      delay(1000);
      stat = 0;
    }
  }
  delay(100);
}