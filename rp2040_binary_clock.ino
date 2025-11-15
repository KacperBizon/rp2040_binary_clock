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
  pinMode(decrease_time_pin, INPUT_PULLUP);
  pinMode(increase_time_pin, INPUT_PULLUP);
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

  Wire.end();
  delay(10);
  Wire.begin();
  delay(20);

  if (!rtc.begin()) 
  {
    digitalWrite(ledPinsHr[nBitsHr - 1], HIGH);

    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower())
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
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

void checkButtons()
{
  dst = digitalRead(time_zone_pin) == HIGH;

  if(digitalRead(decrease_time_pin) == LOW)
  {
    if(minutes == 0)
    {
      minutes = 59;
      hours = (hours + 23) % 24;
    }
    else
    {
      --minutes;
    }
    seconds = 0;

    rtc.adjust(DateTime(rtc.now().year(), rtc.now().month(), rtc.now().day(), hours, minutes, seconds));
    dispBinary(seconds, ledPinsSec, nBitsSec);
    dispBinary(minutes, ledPinsMin, nBitsMin);
    dispBinary(hours, ledPinsHr, nBitsHr);
    delay(150);
  }

  if(digitalRead(increase_time_pin) == LOW)
  {
    if(minutes == 59)
    {
      minutes = 0;
      hours = (hours + 1) % 24;
    }
    else
    {
      ++minutes;
    }
    seconds = 0;

    rtc.adjust(DateTime(rtc.now().year(), rtc.now().month(), rtc.now().day(), hours, minutes, seconds));
    dispBinary(seconds, ledPinsSec, nBitsSec);
    dispBinary(minutes, ledPinsMin, nBitsMin);
    dispBinary(hours, ledPinsHr, nBitsHr);
    delay(200);
  }
}

void loop()
{
  checkButtons();

  DateTime now = rtc.now();

  if(now.second() != lastSecond)
  {
    lastSecond = now.second();

    seconds = (now.second());
    minutes = (now.minute());
    hours = (now.hour());

    //utc + 1 / utc + 2
    if(dst)
      hours = (hours + 1) % 24;
    
    dispBinary(seconds, ledPinsSec, nBitsSec);
    dispBinary(minutes, ledPinsMin, nBitsMin);
    dispBinary(hours, ledPinsHr, nBitsHr);
  }
}
