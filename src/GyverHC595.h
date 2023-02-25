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
    v1.2 - оптимизация, исправлен аппаратный SPI на esp8266, добавлены функции включения/выключения всех ног, инициализация для пустого конструктора
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
    GyverHC595() {}
        
    // конструктор с пином CS
    GyverHC595(int CS) {
        begin(CS);
    }

    // конструктор с пинами CS, DT, CLK
    GyverHC595(int CS, int DT, int CLK) {
        begin(CS, DT, CLK);
    }
    
    // инициализация для SPI
    void begin(int CS) {
        if (_MODE) SPI.begin();
        configurePin(_HC_CS, CS);
        setPin(_HC_CS, HIGH);
    }

    // инициализация для bitbang
    void begin(int CS, int DT, int CLK) {
        begin(CS);
        if (!_MODE) {
            configurePin(_HC_DT, DT);
            configurePin(_HC_CLK, CLK);
        }
    }
    
    // прочитать состояние пина из буфера
    bool read(uint16_t num) {
        return (num < (_AMNT << 3)) ? bitRead(getB(num), num & 0b111) : 0;
    }

    // записать состояние пина под номером
    void write(uint16_t num, bool state) {
        state ? set(num) : clear(num);
    }

    // включить пин под номером
    void set(uint16_t num) {
        if (num < (_AMNT << 3)) bitSet(getB(num), num & 0b111);
    }

    // выключить пин под номером
    void clear(uint16_t num) {
        if (num < (_AMNT << 3)) bitClear(getB(num), num & 0b111);
    }
    
    // включить все
    void setAll() {
        memset(buffer, 0xff, _AMNT);
    }
    
    // выключить все
    void clearAll() {
        memset(buffer, 0, _AMNT);
    }
    
    // записать состояние всех
    void writeAll(bool state) {
        state ? setAll() : clearAll();
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
                    #ifdef HC595_DELAY
                    delayMicroseconds(HC595_DELAY);
                    #endif
                    setPin(_HC_CLK, LOW);
                    #ifdef HC595_DELAY
                    delayMicroseconds(HC595_DELAY);
                    #endif
                }
            }
        }
        setPin(_HC_CS, HIGH);
        if (_MODE) SPI.endTransaction();
    }
    
    // количество доступных пинов
    uint16_t amount() {
        return (_AMNT << 3);
    }

    // доступ к буферу
    uint8_t buffer[_AMNT];

private:
    uint8_t& getB(uint16_t num) {
        return buffer[_AMNT - 1 - (num >> 3)];
    }
    
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