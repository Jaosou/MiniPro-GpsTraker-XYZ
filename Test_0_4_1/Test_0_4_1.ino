#include <Wire.h>
#include <TinyGPS++.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

// กำหนดการตั้งค่า GPS
TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5);     // ใช้ Serial1 สำหรับการรับข้อมูลจาก GPS
const char* ssid = "Jaosou";        //ssid of your wifi
const char* password = "30082022";  //password of your wifi

//Connect Telegram
const char* token = "7829882370:AAGpqWhn9G8h7upkLmjm-b6VJ9zoEpoM2H4";
String id = "1777879694";
WiFiClientSecure client;

UniversalTelegramBot bot(token, client);

// กำหนดการตั้งค่า MPU6050
MPU6050 mpu;

//Define
int func = 1, stat;
float latitude, longitude;
String message = "";
String date_str, time_str, lat_str, lng_str;
WiFiServer server(80);
int valx, valy, valz;

void setup() {
  // เริ่มต้น Serial Monitor
  Serial.begin(115200);

  // ตั้งค่า GPS Serial
  SerialGPS.begin(9600);

  // เริ่มต้นการเชื่อมต่อ I2C กับ MPU6050
  Wire.begin(D6, D5);  // SDA, SCL // RX, TX (D2, D1 บน NodeMCU)
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connected!");
  } else {
    Serial.println("Failed to connect to MPU6050");
  }

  //Wifi Connection
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

  client.setInsecure();
}

void readGPS() {
  unsigned long start = millis();
  bool gpsDataAvailable = false;

  // Wait up to 10 seconds for GPS data
  while (millis() - start < 10000 && !gpsDataAvailable)
    while (SerialGPS.available() > 0)    //while data is available
      if (gps.encode(SerialGPS.read()))  //read gps data
      {
        if (gps.location.isUpdated())  //check whether gps location is valid
        {
          latitude = gps.location.lat();
          longitude = gps.location.lng();
          lat_str = String(latitude, 6);
          lng_str = String(longitude, 6);

          // Prepare the message for Telegram
          message = "https://www.google.com/maps/place/" + lat_str + "," + lng_str;

          Serial.println("Updated GPS Location:");
          Serial.print("Latitude: ");
          Serial.println(lat_str);
          Serial.print("Longitude: ");
          Serial.println(lng_str);
          Serial.print("Message: ");
          Serial.println(message);
        }
      }

  if (!gpsDataAvailable) {
    Serial.println("No GPS data available.");
  }
}

//Alert Send Data
void alert(){

}

//Search Dimention
void readDimention() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  valx = map(ax, -17000, 17000, 0, 179);
  valy = map(ay, -17000, 17000, 0, 179);
  valz = map(az, -17000, 17000, 0, 179);
  if (valx >= 0 && valx <= 179 && valy >= 0 && valy <= 179) {
    Serial.print("axis x = ");
    Serial.print(valx);
    Serial.print(" axis y = ");
    Serial.print(valy);
    Serial.print(" axis z = ");
    Serial.println(valz);
    if (valx < 27) {
      Serial.println("Fall forward");
      readGPS();
      bot.sendMessage(id,"\n--------------------------------------------------\nPerson falling.");
      if(message == ""){
        bot.sendMessage(id,"No GPS data available.\n--------------------------------------------------\n");
      }else{
        bot.sendMessage(id,message+"\n--------------------------------------------------\n");
      }
      delay(50);
    } else if (valx > 160) {
      Serial.println("Fall Backward");
      readGPS();
      bot.sendMessage(id,"\n--------------------------------------------------\nPerson falling.");
      if(message == ""){
        bot.sendMessage(id,"No GPS data available.\n--------------------------------------------------\n");
      }else{
        bot.sendMessage(id,message+"\n--------------------------------------------------\n");
      }
      delay(50);
    }
    if (valy < 20) {
      Serial.println("Fall Left");
      readGPS();
      bot.sendMessage(id,"\n--------------------------------------------------\nPerson falling.");
      if(message == ""){
        bot.sendMessage(id,"No GPS data available.\n--------------------------------------------------\n");
      }else{
        bot.sendMessage(id,message+"\n--------------------------------------------------\n");
      }
      delay(50);
    } else if (valy > 160) {
      Serial.println("Fall Rigth");
      readGPS();
      bot.sendMessage(id,"\n--------------------------------------------------\nPerson falling.");
      if(message == ""){
        bot.sendMessage(id,"No GPS data available.\n--------------------------------------------------\n");
      }else{
        bot.sendMessage(id,message+"\n--------------------------------------------------\n");
      }
      delay(50);
    }
  }
  delay(200);


  /* // แสดงค่า accelerometer และ gyroscope
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
}

//Get Requir on Telegram
void handleNewMessage(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";
    if (text == "/sendlocation") {
      readGPS();
      if (lat_str != "" && lng_str != "") {  // Only send if location is updated
        bot.sendChatAction(chat_id, "typing");
        bot.sendMessage(chat_id, "\n--------------------------------------------------\nLocation: " + message+"\n--------------------------------------------------\n");  // Send updated GPS location
      } else {
        bot.sendMessage(chat_id, "\n--------------------------------------------------\nGPS location not available yet.\n--------------------------------------------------\n");
      }
    }
  }
}


void loop() {
  readDimention();
  delay(50);

  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numNewMessages) {
    Serial.println("got response");
    handleNewMessage(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }


  delay(300);
}
