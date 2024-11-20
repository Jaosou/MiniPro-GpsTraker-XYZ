//GPS
#include <TinyGPS++.h>  // library for GPS module
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <TaskScheduler.h>

//Broken
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Wire.h>

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int valx, valy, valz;
int stat = 0;

Scheduler scheduler;
Task taskReadGPS;
Task taskReadMPU6050;

TinyGPSPlus gps;  // The TinyGPS++ object
#define rxGPS 4
#define txGPS 5

//SoftwareSerial ss(rxGPS, txGPS); // The serial connection to the GPS device

const char* ssid = "WIFINAME";        //ssid of your wifi
const char* password = "PASSWORD";  //password of your wifi
float latitude, longitude;
int year, month, date, hour, minute, second;
String date_str, time_str, lat_str, lng_str;
int pm;

WiFiServer server(80);

SoftwareSerial ss(rxGPS, txGPS);

void setup() {
  Wire.begin(D6, D5);
  taskReadGPS.setInterval(1000);    // อ่าน GPS ทุก 1 วินาที
  taskReadMPU6050.setInterval(50);  // อ่าน MPU6050 ทุก 50 มิลลิวินาที
  scheduler.addTask(taskReadGPS);
  scheduler.addTask(taskReadMPU6050);
  /*   //********** CHANGE PIN FUNCTION  TO TX/RX **********
  //GPIO 1 (TX) swap the pin to a TX.
  pinMode(1, FUNCTION_0); 
  //GPIO 3 (RX) swap the pin to a RX.
  pinMode(3, FUNCTION_0); 
  //*************************************************** */
  Serial.begin(115200);
  Serial.println("Initialize MPU");
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");
  Serial.println();
  ss.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  //connecting to wifi

  while (WiFi.status() != WL_CONNECTED)  // while wifi not connected
  {
    delay(500);
    Serial.print(".");  //print "...."
  }
  Serial.println("");
  Serial.println("WiFi connected");
  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());  // Print the IP address
  delay(3000);
}

void seachDimention() {
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
    } else {
      delay(1000);
      stat = 0;
    }
  }
  delay(100);
}


void searchLocation() {
  while (ss.available() > 0)
    Serial.print("Work");     //while data is available
  if (gps.encode(ss.read()))  //read gps data
  {
    Serial.print("Wooork");
    if (gps.location.isUpdated())  //check whether gps location is valid
    {
      latitude = gps.location.lat();

      lat_str = String(latitude, 6);  // latitude location is stored in a string
      Serial.print("Lat : ");
      Serial.println(lat_str);
      longitude = gps.location.lng();
      lng_str = String(longitude, 6);  //longitude location is stored in a string
      Serial.print("Long : ");
      Serial.println(lng_str);
    }
  }
}


void loop() {
  scheduler.execute();
  seachDimention();
  delay(300);
  searchLocation();
  delay(580);
}