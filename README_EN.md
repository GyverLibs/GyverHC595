This is an automatic translation, may be incorrect in some places. See sources and examples!

# GyverHC595
74HC595 shift register cascade control library for Arduino
- Access by pin number in the cascade
- Built-in buffer
- Hardware SPI or bit bang mode (any pins)
- 5 times faster bit bang output for AVR Arduino

### Compatibility
Compatible with all Arduino platforms (using Arduino functions)

## Content
- [Install](#install)
- [Connection](#wiring)
- [Initialization](#init)
- [Usage](#usage)
- [Example](#example)
- [Versions](#versions)
- [Bugs and feedback](#feedback)

<a id="install"></a>
## Installation
- The library can be found under the name **GyverHC595** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download Library](https://github.com/GyverLibs/GyverHC595/archive/refs/heads/main.zip) .zip archive for manual installation:
    - Unzip and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unzip and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/*
    - (Arduino IDE) automatic installation from .zip: *Sketch/Include library/Add .ZIP library…* and specify the downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE% D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="wiring"></a>
## Connection
### General scheme
![scheme](/doc/scheme.png)

### One chip
- Q0.. Q7 - controlled outputs
- VCC/GND - power supply
- MR connect to **VCC**
- OE connect to **GND**
- DS - data input, **DT**, (**MOSI**)
- SHCP - clock input, **CLK** (**SCK**)
- STCP - chip select, **CS**
- Q7S - data output (not connected anywhere if there is only one chip)

### Cascade
You can connect several 74HC595 chips in a "cascade":
- Pins SHCP, STCP, VCC, GND, as well as OE and MR are connected in parallel for all microcircuits
- Data input DS *of the first chip* is connected to the data output from the microcontroller (pin DT / MOSI)
- Pin Q7S * of the first chip * is connected to the DS of the second chip, and so on along the chain

### Hardware SPI
Using hardware SPI allows you to very quickly update the states of the legs of the microcircuits:
on Arduino Nano (16 MHz) when using 8 MHz SPI, data per chip (1 byte) is transmitted in 1.9 µs.
To connect to the hardware bus, you need to find the MOSI and SCK pins on the pinout of the microcontroller, connect them:
- DS - MOSI
- SHCP - SCK
- STCP - any MK pin, specified during initialization

#### Examples
- Arduino NANO
  - [SCK] D13 -> SHCP
  - [MOSI] D11 -> DS
  - [CS] Any pin -> STCP
-ATmega328
  - [SCK] PB5 -> SHCP
  - [MOSI] PB3 -> DS
  - [CS] Any pin -> STCP
-esp8266
  - [SCK] GPIO14 -> SHCP
  - [MOSI] GPIO13 -> DS
  - [CS] Any pin -> STCP

### Software SPI, bit bang
The library supports software SPI, connection to any three digital pins. For AVR Arduino
accelerated IO is used, allowing one byte to be transferred in 22 µs (105 µs with a normal shiftOut()),
which roughly corresponds to hardware SPI at 500 kHz.

<a id="init"></a>
## Initialization
```cpp
GyverHC595<amount, HC_SPI> reg(pinCS); // hardware SPI, speed 4MHz (default)
GyverHC595<amount, HC_SPI, clock> reg(pinCS); // hardware SPI, custom speed
GyverHC595<amount, HC_PINS> reg(pinCS, pinDT, pinCLK); // software SPI (bit bang)

// amount - number of 74HC595 chips connected in cascade
// pinCS - pin STCP (pin 12 microcircuit)
// pinDT - pin DS (14 microcircuit leg)
// pinCLK - SHCP pin (chip pin 11)
```

<a id="usage"></a>
## Usage
```cpp
void write(uint16_t num, bool state); // write the state of the pin under the number
void set(uint16_tnum); // enable pin number
void clear(uint16_tnum); // turn off the pin under the number
void update(); // update pin states
uint8_tbuffer[]; // buffer access
```
The order of the pins corresponds to the order of the pins of the 74HC595 microcircuits (Q0 .. Q7) and grows away from the first microcircuit:
```cpp
"Number" in the library: [0 1 2.. 7 ] [8 10.. 15] [16.. 23]
Chip pin: [Q0 Q1 Q2.. Q7] [Q0 Q2.. Q7] [Q0.. Q7]
Chip by count: [ 1 ] [ 2 ] [ 3 ]
```

Code execution example:
```cpp
  reg.write(0, 1);
  reg.write(1, 1);
  regset(2);
  regset(8);
  reg.update();
```
![scheme](/doc/examp.png)

<a id="example"></a>
## Examples
See **examples** for other examples!
```cpp
#include <GyverHC595.h>

GyverHC595<2, HC_SPI, 8000000> reg(10); // hardware SPI, custom speed
//GyverHC595<2, HC_PINS> reg(10, 11, 13); // software SPI (bit bang)

void setup() {
  regset(0); // enable pin 0
  reg.write(8, 1); // enable pin 8
  reg.write(8, 0); // disable pin 8
  
  reg.update(); // update
}

void loop() {
}
```

<a id="versions"></a>
## Versions
- v1.0

<a id="feedback"></a>
## Bugs and feedback
When you find bugs, create an **Issue**, or better, immediately write to the mail [alex@alexgyver.ru](mailto:alex@alexgyver.ru)
The library is open for revision and your **Pull Request**'s!