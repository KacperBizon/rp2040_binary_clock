#include <DS1307.h>
DS1307 rtc(3, 4);

Time t;
int ledPinsHr[] = {23, 22, 21, 20, 19};
int ledPinsMin[] = {18, 17, 16, 15, 14, 13};
int ledPinsSec[] = {12, 11, 10, 9, 8, 7};
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
  
  rtc.halt(false);
  rtc.writeProtect(true);
  
  for (byte i = 0; i < nBitsSec; i++) {
    pinMode(ledPinsSec[i], OUTPUT);
  }
  for (byte i = 0; i < nBitsMin; i++) {
    pinMode(ledPinsMin[i], OUTPUT);
  }
  for (byte i = 0; i < nBitsHr; i++) {
    pinMode(ledPinsHr[i], OUTPUT);
  }
}

void loop()
{
  dst = digitalRead(time_zone_pin) == HIGH;

    
  t = rtc.getTime();

  int m = (t.sec);
  int y = (t.min);
  int z = (t.hour);

    

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
