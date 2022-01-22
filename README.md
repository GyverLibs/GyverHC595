![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)
![author](https://img.shields.io/badge/author-AlexGyver-informational.svg)
# GyverHC595
Библиотека для управления каскадом сдвиговых регистров 74HC595 для Arduino
- Обращение по номеру пина в каскаде
- Встроенный буфер
- Аппаратный SPI или bit bang режим (любые пины)
- Ускоренный в 5 раз bit bang вывод для AVR Arduino

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

## Содержание
- [Установка](#install)
- [Подключение](#wiring)
- [Инициализация](#init)
- [Использование](#usage)
- [Пример](#example)
- [Версии](#versions)
- [Баги и обратная связь](#feedback)

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **GyverHC595** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/GyverHC595/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="wiring"></a>
## Подключение
### Общая схема
![scheme](/doc/scheme.png)

### Один чип
- Q0.. Q7 - управляемые выходы
- VCC/GND - питание
- MR подключить к **VCC**
- OE подключить к **GND**
- DS - вход данных, **DT**, (**MOSI**)
- SHCP - вход тактирования, **CLK** (**SCK**)
- STCP - chip select, **CS**
- Q7S - выход данных (никуда не подключен, если чип один)

### Каскад
Можно соединить несколько микросхем 74HC595 "каскадом":
- Пины SHCP, STCP, VCC, GND, а также OE и MR подключаются параллельно у всех микросхем
- Вход данных DS *первой микросхемы* подключается к выходу данных с микроконтроллера (пин DT/MOSI)
- Пин Q7S *первой микросхемы* подключается к DS второй микросхемы, и так далее по цепочке

### Аппаратный SPI
Использование аппаратного SPI позволяет очень быстро обновлять состояния ног микросхем: 
на Arduino Nano (16 MHz) при использовании 8 MHz SPI данные на одну микросхему (1 байт) передаются за 1.9 мкс.  
Для подключения к аппаратной шине нужно найти на распиновке микроокнтроллера пины MOSI и SCK, подключить их:
- DS - MOSI
- SHCP - SCK
- STCP - любой пин МК, указывается при инициализации

#### Примеры
- Arduino NANO
  - [SCK] D13 -> SHCP
  - [MOSI] D11 -> DS
  - [CS] Любой пин -> STCP
- ATmega328
  - [SCK] PB5 -> SHCP
  - [MOSI] PB3 -> DS
  - [CS] Любой пин -> STCP
- esp8266
  - [SCK] GPIO14 -> SHCP
  - [MOSI] GPIO13 -> DS
  - [CS] Любой пин -> STCP

### Программный SPI, bit bang
Библиотека поддерживает программный SPI, подключение к любым трём цифровым пинам. Для AVR Arduino 
используется ускоренный IO, позволяющий передать один байт за 22 мкс (105 мкс при обычном shiftOut()), 
что примерно соответствует аппаратному SPI на 500 kHz.

<a id="init"></a>
## Инициализация
```cpp
GyverHC595<amount, HC_SPI> reg(pinCS);                  // аппаратный SPI, скорость 4MHz (по умолч.)
GyverHC595<amount, HC_SPI, clock> reg(pinCS);           // аппаратный SPI, кастомная скорость
GyverHC595<amount, HC_PINS> reg(pinCS, pinDT, pinCLK);  // программный SPI (bit bang)

// amount - количество чипов 74HC595, подключенных каскадом
// pinCS - пин STCP (12 ножка микросхемы)
// pinDT - пин DS (14 ножка микросхемы)
// pinCLK - пин SHCP (11 ножка микросхемы)
```

<a id="usage"></a>
## Использование
```cpp
void write(uint16_t num, bool state);   // записать состояние пина под номером
void set(uint16_t num);                 // включить пин под номером
void clear(uint16_t num);               // выключить пин под номером
void update();                          // обновить состояния пинов
uint8_t buffer[];                       // доступ к буферу
```
Порядок пинов соответствует порядку пинов микросхем 74HC595 (Q0.. Q7) и растёт в сторону удаления от первой микросхемы:  
```cpp
"Номер" в библиотеке:  [0    1    2..  7 ]   [8    10.. 15]   [16.. 23]
Пин микросхемы:        [Q0   Q1   Q2.. Q7]   [Q0   Q2.. Q7]   [Q0.. Q7]
Микросхема по счёту:   [        1        ]   [     2      ]   [   3   ]
```

Пример выполнения кода:
```cpp
  reg.write(0, 1);
  reg.write(1, 1);
  reg.set(2);
  reg.set(8);
  reg.update();
```
![scheme](/doc/examp.png)

<a id="example"></a>
## Примеры
Остальные примеры смотри в **examples**!
```cpp
#include <GyverHC595.h>

GyverHC595<2, HC_SPI, 8000000> reg(10); // аппаратный SPI, кастомная скорость
//GyverHC595<2, HC_PINS> reg(10, 11, 13); // программный SPI (bit bang)

void setup() {
  reg.set(0);       // включить пин 0
  reg.write(8, 1);  // включить пин 8
  reg.write(8, 0);  // выключить пин 8
  
  reg.update();     // обновить
}

void loop() {
}
```

<a id="versions"></a>
## Версии
- v1.0

<a id="feedback"></a>
## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!
