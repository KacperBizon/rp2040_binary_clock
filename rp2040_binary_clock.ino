#include "RTClib.h"
RTC_DS3231 rtc;

byte ledPinsSec[] = {7, 8, 9, 10, 11, 12};
byte ledPinsMin[] = {13, 14, 15, 16, 17, 18};
byte ledPinsHr[] = {19, 20, 21, 22, 23};

constexpr byte nBitsSec = sizeof(ledPinsSec)/sizeof(ledPinsSec[0]);
constexpr byte nBitsMin = sizeof(ledPinsMin)/sizeof(ledPinsMin[0]);
constexpr byte nBitsHr = sizeof(ledPinsHr)/sizeof(ledPinsHr[0]);

byte decrease_time_pin = 2;
byte increase_time_pin = 3;
byte time_zone_pin = 6;

bool dst = false;
byte lastSecond = 0;

byte seconds = 0;
byte minutes = 0;
byte hours = 0;

void setup(void)
{
  pinMode(time_zone_pin, INPUT_PULLUP);
  
  for (byte i = 0; i < nBitsSec; i++) 
  {
    pinMode(ledPinsSec[i], OUTPUT);
  }
  for (byte i = 0; i < nBitsMin; i++) 
  {
    pinMode(ledPinsMin[i], OUTPUT);
  }
  for (byte i = 0; i < nBitsHr; i++)
  {
    pinMode(ledPinsHr[i], OUTPUT);
  }

  if (! rtc.begin()) 
  {
    digitalWrite(ledPinsHr[nBitsHr - 1], HIGH);

    Serial.flush();
    while (1) delay(10);
  }
}

void dispBinary(byte value, const byte* pins, byte nBits)
{
    for (byte i = 0; i < nBits; i++)
    {
        gpio_put(pins[i], value & 1);
        value >>= 1;
    }
}

void loop()
{
  DateTime now = rtc.now();

  if(now.second() != lastSecond)
  {
    lastSecond = now.second();

    seconds = (now.second());
    minutes = (now.minute());
    hours = (now.hour());

    dst = digitalRead(time_zone_pin) == HIGH;

    //utc + 1 / utc + 2
    if(dst)
      hours = (hours + 1) % 24;
    
    dispBinary(seconds, ledPinsSec, nBitsSec);
    dispBinary(minutes, ledPinsMin, nBitsMin);
    dispBinary(hours, ledPinsHr, nBitsHr);
  }
}
