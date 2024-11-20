#include <Wire.h>
#include <TinyGPS++.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

// กำหนดการตั้งค่า GPS
TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5);     // ใช้ Serial1 สำหรับการรับข้อมูลจาก GPS
const char* ssid = "WIFINAME";        //ssid of your wifi
const char* password = "PASSWORD";  //password of your wifi

// กำหนดการตั้งค่า MPU6050
MPU6050 mpu;

int func = 1;
float latitude, longitude;
String date_str, time_str, lat_str, lng_str;
WiFiServer server(80);

void setup() {
  // เริ่มต้น Serial Monitor
  Serial.begin(115200);

  // ตั้งค่า GPS Serial
  SerialGPS.begin(9600);  // RX, TX (D2, D1 บน NodeMCU)

  // เริ่มต้นการเชื่อมต่อ I2C กับ MPU6050
  Wire.begin(D6, D5);  // SDA, SCL
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connected!");
  } else {
    Serial.println("Failed to connect to MPU6050");
  }
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
}

void readGPS() {
  while (SerialGPS.available() > 0)    //while data is available
    if (gps.encode(SerialGPS.read()))  //read gps data
    {
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

void readDimen() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // แสดงค่า accelerometer และ gyroscope
  Serial.print("Accel: ");
  Serial.print("X = ");
  Serial.print(ax);
  Serial.print(" | Y = ");
  Serial.print(ay);
  Serial.print(" | Z = ");
  Serial.println(az);

  Serial.print("Gyro: ");
  Serial.print("X = ");
  Serial.print(gx);
  Serial.print(" | Y = ");
  Serial.print(gy);
  Serial.print(" | Z = ");
  Serial.println(gz);
}

void loop() {
  /* if (func == 1) {
    Serial.print("Work");
    readGPS();
    func = 0;
    delay(700); */
  /* else (func == 0);
  {
    readDimen();
    func = 1;
    delay(700);
  } */
  // อ่านข้อมูลจาก GPS
  readGPS();
  /* while (SerialGPS.available() > 0)    //while data is available
    if (gps.encode(SerialGPS.read()))  //read gps data
    {
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
    } */


      // แสดงข้อมูล GPS ถ้าพร้อม
      /* if (gps.location.isUpdated()) {
    Serial.print("Work");
    Serial.print("Latitude= ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(" Longitude= ");
    Serial.println(gps.location.lng(), 6);
  }
  delay(600); */

      // อ่านค่าจาก MPU6050
      /* int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // แสดงค่า accelerometer และ gyroscope
  Serial.print("Accel: ");
  Serial.print("X = ");
  Serial.print(ax);
  Serial.print(" | Y = ");
  Serial.print(ay);
  Serial.print(" | Z = ");
  Serial.println(az);

  Serial.print("Gyro: ");
  Serial.print("X = ");
  Serial.print(gx);
  Serial.print(" | Y = ");
  Serial.print(gy);
  Serial.print(" | Z = ");
  Serial.println(gz); */

  delay(200);  // หน่วงเวลาเล็กน้อย
  readDimen();
  delay(200);
}
