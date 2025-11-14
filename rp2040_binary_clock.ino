#include "RTClib.h"
RTC_DS3231 rtc;

int ledPinsHr[] = {19, 20, 21, 22, 23};
int ledPinsMin[] = {13, 14, 15, 16, 17, 18};
int ledPinsSec[] = {7, 8, 9, 10, 11, 12};
int decrease_time_pin = 2;
int increase_time_pin = 3;
int time_zone_pin = 6;

bool dst = false;

int countS = 0;   // S
int countM = 0 ;  // Min
int countH = 0;  // H

unsigned long delay_ms = 999;

byte countSec;
byte countMin;
byte countHr;

#define nBitsHr sizeof(ledPinsHr)/sizeof(ledPinsHr[0])
#define nBitsMin sizeof(ledPinsMin)/sizeof(ledPinsMin[0])
#define nBitsSec sizeof(ledPinsSec)/sizeof(ledPinsSec[0])

void setup(void)
{
  pinMode(time_zone_pin, INPUT_PULLUP);
  
  for (byte i = 0; i < nBitsSec; i++) {
    pinMode(ledPinsSec[i], OUTPUT);
  }
  for (byte i = 0; i < nBitsMin; i++) {
    pinMode(ledPinsMin[i], OUTPUT);
  }
  for (byte i = 0; i < nBitsHr; i++) {
    pinMode(ledPinsHr[i], OUTPUT);
  }

  if (! rtc.begin()) 
  {
    Serial.flush();
    while (1) delay(10);
  }
}

void loop()
{
  dst = digitalRead(time_zone_pin) == HIGH;

  DateTime now = rtc.now();

  int m = (now.second());
  int y = (now.minute());
  int z = (now.hour());

    

  //utc + 1 / utc + 2
  if(dst)
    z = (z + 1) % 24;
   
   
  delay(delay_ms);
  delayMicroseconds(500);

  /* rtc ds3231 handles time
  countS = (countS + 1);
  if (countS > 59)
  {
    countS = 0;
    countM = (countM + 1);
    if (countM > 59)
    {
      countM = 0;
      countH = (countH + 1);
      if (countH > 23)
      {
        countH = 0;
        countM = 0;
        countS = 0;
      }
    }
  }
*************************/
  //if (z > 5 && z < 24)
  //{
    dispBinarySec(m);
    dispBinaryMin(y);
    dispBinaryHr(z);
  //}
  //else
  //{
  //  dispBinarySec(0);
  //  dispBinaryMin(0);
  //  dispBinaryHr(0);
  //}
}

void dispBinaryHr(byte nHr)
{
  for (byte i = 0; i < nBitsHr; i++)
  {
    digitalWrite(ledPinsHr[i], nHr & 1);
    nHr /= 2;
  }
}
void dispBinaryMin(byte nMin)
{
  for (byte i = 0; i < nBitsMin; i++)
  {
    digitalWrite(ledPinsMin[i], nMin & 1);
    nMin /= 2;
  }
}
void dispBinarySec(byte nSec)
{
  for (byte i = 0; i < nBitsSec; i++)
  {
    digitalWrite(ledPinsSec[i], nSec & 1);
    nSec /= 2;
  }
}
