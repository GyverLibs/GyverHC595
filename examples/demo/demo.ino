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
