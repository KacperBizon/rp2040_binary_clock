# rp2040 binary clock
![clock image](https://github.com/KacperBizon/rp2040_binary_clock/blob/main/clock.jpeg)

## Flashing
1. Hold BOOTSEL button while plugging the USB
2. mount the device (RPI-RP2)
3. Drag the rp2040_binary_clock.uf2 to RPI-RP2

## Building from Source (Pico C/C++ SDK)
### Prerequisites
- Install CMake and Make
- Install arm-none-eabi-gcc
- Clone the [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk), ensure the submodules are initialized and set PICO_SDK_PATH.

### Compilation
```bash
git clone https://github.com/KacperBizon/rp2040_binary_clock.git
cd rp2040_binary_clock/firmware
mkdir build
cd build
cmake ..
make
```

## Optional resistors
- R4 - 10K - use only when having problems with writing to memory
- R25 - 1K - use only with lithium rechargeable batteries

## License & Acknowledgments
This project is distributed under the [MIT License](LICENSE).

Third-Party Components:
The `firmware/pico_sdk_import.cmake` file included in this repository is the intellectual property of Raspberry Pi (Trading) Ltd. and is distributed under the BSD 3-Clause License.
