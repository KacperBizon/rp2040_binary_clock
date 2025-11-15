# rp2040 binary clock
![clock image](https://github.com/KacperBizon/rp2040_binary_clock/blob/main/clock.jpeg)

### How to use
- In Arduino IDE add "Arduino Mbed OS RP2040 Boards" in boards manager
- In Arduino IDE add "RTClib" in library manager
- Set the time on the clock to 'standard time' and change time to daylight saving time with slide switch

### Optional resistors
- R4 - 10K - use only when having problems with writing to memory
- R25 - 1K - use only with lithium rechargeable batteries
