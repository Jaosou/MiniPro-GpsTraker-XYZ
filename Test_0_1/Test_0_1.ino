//GPS
#include <TinyGPS++.h>  // library for GPS module
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <TaskScheduler.h>

//Broken
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Wire.h>

int16_t ax, ay, az;
int16_t gx, gy, gz;
int valx, valy, valz;
int stat = 0;

// Create GPS and MPU6050 objects
TinyGPSPlus gps;
MPU6050 mpu;

// Create TaskScheduler object
Scheduler scheduler;



// WIFI NAME AND PASS
const char* ssid = "WIFINAME";        //ssid of your wifi
const char* password = "PASSWORD";  //password of your wifi
SoftwareSerial GPSSerial(4, 5);
WiFiServer server(80);


// Function to read GPS data
void readGPS() {
  Serial.print("GPS : Work");
  while (GPSSerial.available() > 0) {
    char c = GPSSerial.read();
    gps.encode(c);

    if (gps.location.isValid()) {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
    }else {
      Serial.println("Invalid GPS Data");
    }
  }
}

// Function to read MPU6050 data
void readMPU6050() {
  Serial.print("MPU : Work");
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  valx = map(ax, -17000, 17000, 0, 179);
  valy = map(ay, -17000, 17000, 0, 179);
  valz = map(az, -17000, 17000, 0, 179);
  if (valx >= 0 && valx <= 179 && valy >= 0 && valy <= 179 && stat == 0) {
    Serial.print("axis x = ");
    Serial.print(valx);
    Serial.print(" axis y = ");
    Serial.print(valy);
    Serial.print(" axis z = ");
    Serial.println(valz);
    if (valx < 20) {  //ใชปรับระยะในการลมไปดานหนา
      Serial.println("ล้มไปด้านหน้า");
      stat = 1;
    } else if (valx > 160) {  //ใชปรับระยะในการลมไปดานหลัง
      Serial.println("ล้มไปด้านหลัง");
      stat = 1;
    }
    if (valy < 20) {  //ใชปรับระยะในการลมไปดานซาย
      Serial.println("ล้มไปด้านซาย");
      stat = 1;
    } else if (valy > 160) {  //ใชปรับระยะในการลมไปดานขวา
      Serial.println("ล้มไปด้านขวา");
      stat = 1;
    }
  }else {
    stat = 0;
    delay(500);
  } 
  /* Vector norm = mpu.getAcceleration();
  Serial.print("aX = "); Serial.print(norm.X);
  Serial.print(" aY = "); Serial.print(norm.Y);
  Serial.print(" aZ = "); Serial.println(norm.Z); */
}

// Create tasks
Task taskReadGPS(0,1000,readGPS);
Task taskReadMPU6050(0,200,readMPU6050);

void setup() {
  // Initialize Serial, GPS, I2C, and MPU6050
  Serial.begin(115200);
  GPSSerial.begin(9600);
  Wire.begin(D6, D5);
  mpu.initialize();

  // Set up tasks
  taskReadGPS.setInterval(1000);    // Read GPS data every 1 second
  taskReadMPU6050.setInterval(200);  // Read MPU6050 data every 200 milliseconds
  scheduler.addTask(taskReadGPS);
  scheduler.addTask(taskReadMPU6050);

  //Connecting Wifi
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

void loop() {
  scheduler.execute();
}
