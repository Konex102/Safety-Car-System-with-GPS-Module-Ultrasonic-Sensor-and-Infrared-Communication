#include <TinyGPS++.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "BluetoothSerial.h"
#include <math.h>

#define RXD2 16      // Pinout for GPS module NEO-6M
#define TXD2 17      // Pinout for GPS module NEO-6M
#define ECHOREAR 5   // Pinout for Ultrasonic Sensor US-025 (Rear)
#define TRIGREAR 18  // Pinout for Ultrasonic Sensor US-025 (Rear)
#define IRemit 4    // Pinout for Infrared Transmitter

// Bluetooth pairing
String device_name = "ESP32Receiver";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

// Unique Code
uint16_t UniqCode1 = 0xB18D, UniqCode2 = 0xB24D, UniqCode3 = 0xB42D, UniqCode4 = 0xB66D, UniqCode5 = 0xCA53, UniqCode6 = 0xCC33, UniqCode7 = 0x9E79;

const int max_data = 5000;
double dataSpeed[max_data];
int indexData = 0;

TinyGPSPlus gps;
HardwareSerial neogps(1);
IRsend irsend(IRemit);
BluetoothSerial SerialBT;

void setup() 
{
  Serial.begin(115200);
  SerialBT.begin(device_name);
  irsend.begin();
  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);
  pinMode(TRIGREAR, OUTPUT);
  pinMode(ECHOREAR, INPUT);

  for(int i =0;i<max_data;i++)
  {
   dataSpeed[i]=0.0; 
  }
}

void loop() 
{
  while(neogps.available()>0) 
  {
    if(gps.encode(neogps.read()))
    {
      print_speed();
      print_ultrasonic();
    }else if(millis() > 5000 && gps.charsProcessed() < 10)
    {
    Serial.println(F("No GPS data received: check wiring"));
    }
  }
}

void print_ultrasonic()
{
  // Define sound speed for Ultrasonic 
  long duration1;
  float distance_cm1;

  // Rear ultrasonic program
  digitalWrite(TRIGREAR, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGREAR, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGREAR, LOW);
  duration1 = pulseIn(ECHOREAR, HIGH);
  distance_cm1 = 0.034*duration1/2;

  // Condition distance
  if(distance_cm1<10)
  {
    SerialBT.print("Terjadi tabrakan!");
    irsend.sendNEC(UniqCode6);
    SerialBT.print("|||");
    SerialBT.println("Kode unik 6 terkirim!");

  }else if(distance_cm1<=200) 
  {
    SerialBT.print("Jarak Kendaraan Berdekatan");
    SerialBT.print("|||");
    SerialBT.print(distance_cm1);
    irsend.sendNEC(UniqCode5);
    SerialBT.print("|||");
    SerialBT.println("Kode unik 5 terkirim!");
  }
}


void print_speed()
{
  // Definition for speed variables
  double highSpeed = 30;
  double normalSpeed = 10;

  if(gps.speed.isValid())
  {
    double curSpeed = gps.speed.kmph();

    SerialBT.print(" |||");
    SerialBT.print(" Kecepatan saat ini : ");
    SerialBT.print(curSpeed,2);
    SerialBT.println("Km/h");
    SerialBT.print(" |||");

    double prevSpeed;
    if(indexData == 0)
    {
      prevSpeed = dataSpeed[max_data -1];
    }else
    {
      prevSpeed = dataSpeed[indexData - 1];
    }

    if(curSpeed>prevSpeed)
    {
      irsend.sendNEC(UniqCode3);
      SerialBT.print("|||");
      SerialBT.println("Kode unik 3 terkirim!");
      delay(500);
    }else if(curSpeed<prevSpeed)
    {
      irsend.sendNEC(UniqCode4);
      SerialBT.print("|||");
      SerialBT.println("Kode unik 4 terkirim!");
      delay(500);
    }else if(curSpeed>highSpeed)
    {
      irsend.sendNEC(UniqCode2);
      SerialBT.print("|||");
      SerialBT.println("Kode unik 2 terkirim!");
      delay(500);
    }else if(curSpeed>normalSpeed)
    {
      irsend.sendNEC(UniqCode1);
      SerialBT.print("|||");
      SerialBT.println("Kode unik 1 terkirim!");
      delay(500);
    }

    dataSpeed[indexData] = curSpeed;

    indexData = (indexData+1)%max_data;
  }
  delay(500);
}
