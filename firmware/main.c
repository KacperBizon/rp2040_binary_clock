#include "hardware/i2c.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define USE_GRAY_CODE 0

#define I2C_PORT i2c0
#define DS3231_ADDR 0x68
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

const uint8_t ledPinsSec[] = {7, 8, 9, 10, 11, 12};
const uint8_t ledPinsMin[] = {13, 14, 15, 16, 17, 18};
const uint8_t ledPinsHr[] = {19, 20, 21, 22, 23};

const uint8_t nBitsSec = sizeof(ledPinsSec) / sizeof(ledPinsSec[0]);
const uint8_t nBitsMin = sizeof(ledPinsMin) / sizeof(ledPinsMin[0]);
const uint8_t nBitsHr = sizeof(ledPinsHr) / sizeof(ledPinsHr[0]);

const uint8_t decrease_time_pin = 2;
const uint8_t increase_time_pin = 3;
const uint8_t time_zone_pin = 6;

bool dst = false;
uint8_t lastSecond = 0;

uint8_t seconds = 60;
uint8_t minutes = 0;
uint8_t hours = 0;

static inline uint8_t bcd_to_dec(uint8_t val)
{
  return ((val >> 4) * 10) + (val & 0x0F);
}

static inline uint8_t dec_to_bcd(uint8_t val)
{
  return ((val / 10) << 4) | (val % 10);
}

// I2C DS3231 Operations
void rtc_get_time(struct tm *t)
{
  uint8_t reg = 0x00;
  uint8_t buf[7];
  i2c_write_blocking(I2C_PORT, DS3231_ADDR, &reg, 1, true);
  i2c_read_blocking(I2C_PORT, DS3231_ADDR, buf, 7, false);

  t->tm_sec = bcd_to_dec(buf[0] & 0x7F);
  t->tm_min = bcd_to_dec(buf[1]);
  t->tm_hour = bcd_to_dec(buf[2] & 0x3F);
  t->tm_mday = bcd_to_dec(buf[4]);
  t->tm_mon = bcd_to_dec(buf[5] & 0x1F) - 1;
  t->tm_year = bcd_to_dec(buf[6]) + 100;
}

void rtc_set_time(struct tm *t)
{
  uint8_t buf[8];
  buf[0] = 0x00;
  buf[1] = dec_to_bcd(t->tm_sec);
  buf[2] = dec_to_bcd(t->tm_min);
  buf[3] = dec_to_bcd(t->tm_hour);
  buf[4] = 1; // Arbitrary day of week
  buf[5] = dec_to_bcd(t->tm_mday);
  buf[6] = dec_to_bcd(t->tm_mon + 1);
  buf[7] = dec_to_bcd(t->tm_year - 100);

  i2c_write_blocking(I2C_PORT, DS3231_ADDR, buf, 8, false);
}

bool rtc_lost_power()
{
  uint8_t reg = 0x0F;
  uint8_t status;
  i2c_write_blocking(I2C_PORT, DS3231_ADDR, &reg, 1, true);
  i2c_read_blocking(I2C_PORT, DS3231_ADDR, &status, 1, false);
  return (status & 0x80) != 0; // Evaluate OSF
}

void rtc_clear_osf()
{
  uint8_t reg = 0x0F;
  uint8_t status;
  i2c_write_blocking(I2C_PORT, DS3231_ADDR, &reg, 1, true);
  i2c_read_blocking(I2C_PORT, DS3231_ADDR, &status, 1, false);
  status &= ~0x80;
  uint8_t buf[2] = {0x0F, status};
  i2c_write_blocking(I2C_PORT, DS3231_ADDR, buf, 2, false);
}

void rtc_adjust_to_compile_time()
{
  const char *month_names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  char s_month[5];
  int day, year, hour, min, sec;

  sscanf(__DATE__, "%s %d %d", s_month, &day, &year);
  sscanf(__TIME__, "%d:%d:%d", &hour, &min, &sec);

  struct tm t = {0};
  for (int i = 0; i < 12; i++)
  {
    if (strncmp(s_month, month_names[i], 3) == 0)
    {
      t.tm_mon = i;
      break;
    }
  }
  t.tm_mday = day;
  t.tm_year = year - 1900;
  t.tm_hour = hour;
  t.tm_min = min;
  t.tm_sec = sec;

  rtc_set_time(&t);
  rtc_clear_osf();
}

bool rtc_ping()
{
  uint8_t dummy = 0;
  int ret = i2c_read_blocking(I2C_PORT, DS3231_ADDR, &dummy, 1, false);
  return ret != PICO_ERROR_GENERIC;
}

// Display Logic
#if USE_GRAY_CODE
uint8_t toGray(uint8_t value) { return value ^ (value >> 1); }
#endif

void dispBinary(uint8_t value, const uint8_t *pins, uint8_t nBits)
{
#if USE_GRAY_CODE
  value = toGray(value);
#endif

  for (uint8_t i = 0; i < nBits; i++)
  {
    gpio_put(pins[i], value & 1);
    value >>= 1;
  }
}

// Initialization
void setup(void)
{
  gpio_init(decrease_time_pin);
  gpio_set_dir(decrease_time_pin, GPIO_IN);
  gpio_pull_up(decrease_time_pin);

  gpio_init(increase_time_pin);
  gpio_set_dir(increase_time_pin, GPIO_IN);
  gpio_pull_up(increase_time_pin);

  gpio_init(time_zone_pin);
  gpio_set_dir(time_zone_pin, GPIO_IN);
  gpio_pull_up(time_zone_pin);

  for (uint8_t i = 0; i < nBitsSec; i++)
  {
    gpio_init(ledPinsSec[i]);
    gpio_set_dir(ledPinsSec[i], GPIO_OUT);
  }
  for (uint8_t i = 0; i < nBitsMin; i++)
  {
    gpio_init(ledPinsMin[i]);
    gpio_set_dir(ledPinsMin[i], GPIO_OUT);
  }
  for (uint8_t i = 0; i < nBitsHr; i++)
  {
    gpio_init(ledPinsHr[i]);
    gpio_set_dir(ledPinsHr[i], GPIO_OUT);
  }

  i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA_PIN);
  gpio_pull_up(I2C_SCL_PIN);

  i2c_deinit(I2C_PORT);
  sleep_ms(30);
  i2c_init(I2C_PORT, 400 * 1000);
  sleep_ms(30);

  while (!rtc_ping())
  {
    gpio_put(ledPinsHr[nBitsHr - 1], !gpio_get(ledPinsHr[nBitsHr - 1]));
    i2c_init(I2C_PORT, 400 * 1000);
    sleep_ms(500);
  }

  if (rtc_lost_power())
  {
    rtc_adjust_to_compile_time();
  }

  watchdog_enable(5000, 1);
}

// Update Functions
void checkButtons()
{
  static uint32_t last_button_action = 0;
  uint32_t current_ms = to_ms_since_boot(get_absolute_time());

  dst = gpio_get(time_zone_pin);

  if (current_ms - last_button_action > 220)
  {
    bool decrease_active = !gpio_get(decrease_time_pin);
    bool increase_active = !gpio_get(increase_time_pin);

    if (decrease_active || increase_active)
    {
      last_button_action = current_ms;

      struct tm now;
      rtc_get_time(&now);

      if (decrease_active)
      {
        now.tm_min -= 1;
      }
      else
      {
        now.tm_min += 1;
      }

      now.tm_sec = 0;
      mktime(&now);

      rtc_set_time(&now);
      lastSecond = 60;
    }
  }
}

void updateClock()
{
  static uint32_t lastReadTime = 0;
  uint32_t current_ms = to_ms_since_boot(get_absolute_time());

  if (current_ms - lastReadTime > 100)
  {
    lastReadTime = current_ms;

    struct tm now;
    rtc_get_time(&now);

    if (now.tm_sec != lastSecond)
    {
      lastSecond = now.tm_sec;

      seconds = now.tm_sec;
      minutes = now.tm_min;
      hours = now.tm_hour;

      if (dst)
      {
        hours = (hours + 1) % 24;
      }

      dispBinary(seconds, ledPinsSec, nBitsSec);
      dispBinary(minutes, ledPinsMin, nBitsMin);
      dispBinary(hours, ledPinsHr, nBitsHr);
    }
  }
}

int main()
{
  stdio_init_all();
  setup();

  while (true)
  {
    watchdog_update();
    checkButtons();
    updateClock();
  }

  return 0;
}
