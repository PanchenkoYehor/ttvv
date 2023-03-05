#include <Wire.h>

#include <Wire.h>     //needed because DS3231 uses I2C Bus
#include <RTClib.h>   //needed becuase we have ready-made functions of this librray

RTC_DS3231 rtc;     //the object rtc is created from the class RTC_DS3231
char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
#define deviceAddress 0x68


void setup()
{
  Serial.begin(9600);
  rtc.begin();
  // Serial.println("I called setup");
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time

  //rtc.adjust(DateTime(2018, 12, 23, 9, 15, 17));//set date-time manualy:yr,mo,dy,hr,mn,sec
}

void  loop()
{
  //--read Date and Tme from DS3231 using the method now()---------------------;
  //--store the Date and Time into another user define object bamed nowTime
  DateTime nowTime = rtc.now();

  //---show Day and Date on Top Line of LCD--------------
  int x = nowTime.dayOfTheWeek(); //dyOfTheWeek() is a pre-defined method
  // Serial.println(x);
  Serial.print(daysOfTheWeek[x]);    //days of the Week[] is the user defined array
  Serial.print(": ");

  Serial.print(nowTime.day());
  Serial.print("/");

  Serial.print(nowTime.month());
  Serial.print("/");

  Serial.print(nowTime.year());

  Wire.beginTransmission(deviceAddress); //START, Roll Cal
  Wire.write(0x00); //set SEC Register address
  Wire.endTransmission(); //Execute the above queued data, ACK, STOP

  Wire.requestFrom(deviceAddress, 3);   //SEC, MIN, and HRS to read from RTC as BCD
  byte bcdSeconds = Wire.read();
  byte bcdMinutes = Wire.read();
  byte bcdHours = Wire.read();
  //-----------------------------
  
  Serial.print("  Time: ");
  //show HRS--
  Serial.print(bcdHours >> 4);
  Serial.print(bcdHours & 0x0F);
  Serial.print(':');

  //show MIN--
  Serial.print(bcdMinutes >> 4);
  Serial.print(bcdMinutes & 0x0F);
  Serial.print(':');
  
  //shiw SEC
  Serial.print(bcdSeconds >> 4);
  Serial.print(bcdSeconds & 0x0F);
  Serial.println();
}