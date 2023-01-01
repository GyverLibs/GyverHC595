/*
    Библиотека для управления каскадом сдвиговых регистров 74HC595 для Arduino
    GitHub: https://github.com/GyverLibs/GyverHC595
    Возможности:
    - Обращение по номеру пина в каскаде
    - Встроенный буфер
    - Аппаратный SPI или bit bang режим (любые пины)
    - Ускоренный в 5 раз bit bang вывод для AVR Arduino
    
    AlexGyver, alex@alexgyver.ru
    https://alexgyver.ru/
    MIT License

    Версии:
    v1.0
    v1.1 - улучшена производительность esp8266
*/

#ifndef _GyverHC595_h
#define _GyverHC595_h

#include <Arduino.h>
#include <SPI.h>

#define HC_PINS 0
#define HC_SPI 1

#define _HC_CS 0
#define _HC_DT 1
#define _HC_CLK 2

template < const uint16_t _AMNT, const bool _MODE = HC_SPI, const uint32_t _SPD = 4000000ul >
class GyverHC595 {
public:
    // конструктор с пином CS
    GyverHC595(uint8_t CS) {
        configurePin(_HC_CS, CS);
        setPin(_HC_CS, HIGH);
        if (_MODE) SPI.begin();
    }

    // конструктор с пинами CS, DT, CLK
    GyverHC595(uint8_t CS, uint8_t DT, uint8_t CLK) {
        configurePin(_HC_CS, CS);
        setPin(_HC_CS, HIGH);
        if (!_MODE) {
            configurePin(_HC_DT, DT);
            configurePin(_HC_CLK, CLK);
        }
        if (_MODE) SPI.begin();
    }

    // записать состояние пина под номером
    void write(uint16_t num, bool state) {
        if (num >= (_AMNT << 3)) return;
        if (state) bitSet(buffer[_AMNT - 1 - (num >> 3)], num & 0b111);
        else bitClear(buffer[_AMNT - 1 - (num >> 3)], num & 0b111);
    }

    // включить пин под номером
    void set(uint16_t num) {
        if (num < (_AMNT << 3)) bitSet(buffer[_AMNT - 1 - (num >> 3)], num & 0b111);
    }

    // выключить пин под номером
    void clear(uint16_t num) {
        if (num < (_AMNT << 3)) bitClear(buffer[_AMNT - 1 - (num >> 3)], num & 0b111);
    }

    // записать байт данных (можно делать это напрямую с массивом)
    void writeByte(uint16_t b, uint8_t data) {
        if (b < _AMNT) buffer[b] = data;
    }
    
    // обновить состояния пинов
    void update() {
        if (_MODE) SPI.beginTransaction(SPISettings(_SPD, MSBFIRST, SPI_MODE0));
        setPin(_HC_CS, LOW);
        for (int i = 0; i < _AMNT; i++) {
            if (_MODE) SPI.transfer(buffer[i]);
            else {
                uint8_t data = buffer[i];
                for (uint8_t b = 0; b < 8; b++)  {
                    setPin(_HC_DT, data & (1 << 7));
                    data <<= 1;
                    setPin(_HC_CLK, HIGH);
                    setPin(_HC_CLK, LOW);
                }
            }
        }
        setPin(_HC_CS, HIGH);
        if (_MODE) SPI.endTransaction();
    }

    // доступ к буферу
    uint8_t buffer[_AMNT];

private:
    void configurePin(uint8_t num, uint8_t pin) {
        pinMode(pin, OUTPUT);
        #ifdef __AVR__
        _port_reg[num] = portOutputRegister(digitalPinToPort(pin));
        _bit_mask[num] = digitalPinToBitMask(pin);
        #else
        _pins[num] = pin;
        #endif
    }

    void setPin(int num, bool state) {
        #if defined(__AVR__)
        if (state) *_port_reg[num] |= _bit_mask[num];
        else *_port_reg[num] &= ~_bit_mask[num];
        #elif defined(ESP8266)
        if (state) GPOS = (1 << _pins[num]);
        else GPOC = (1 << _pins[num]);
        #else
        digitalWrite(_pins[num], state);
        #endif
    }

#ifdef __AVR__
    volatile uint8_t *_port_reg[_MODE ? 1 : 3];
    volatile uint8_t _bit_mask[_MODE ? 1 : 3];
#else
    uint8_t _pins[_MODE ? 1 : 3];
#endif
};
#endif