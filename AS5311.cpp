#include "Arduino.h"
#include "AS5311.h"
#include  <util/parity.h>

double POLE_SPACING = 2000.0; //micrometers

AS5311::AS5311(uint16_t DataPin, uint16_t ClockPin, uint16_t ChipSelectPin, uint16_t IndexPin)
           : _data(DataPin), _clock(ClockPin), _cs(ChipSelectPin), _index(IndexPin)
{
    pinMode(_data, INPUT);
    pinMode(_clock, OUTPUT);
    pinMode(_cs, OUTPUT);
    pinMode(_index, INPUT);
}
// should have an interrupt counting INDEX pulses
// or just overflow counting

double AS5311::encoder_position(void)
{
  //returns micrometers
  return ((encoder_value() * POLE_SPACING)/4096);
}
// we also want to be able to associate a specific error code with a specific reading (if the parity fails, we want to throw out that specific value)
// Should have a ::read function and then a position / value decoder 
uint32_t AS5311::encoder_value(void)
{
  return (read_chip() >> 6);
}

uint32_t AS5311::encoder_error()
{

  error_code = last_raw_value & 0b000000000000111111;
  err_value.DECn = error_code & 2;
  err_value.INCn = error_code & 4;
  err_value.LIN = error_code & 8;
  err_value.COF = error_code & 16;
  err_value.OCF = error_code & 32;
  err_value.PARITY_OK = (last_raw_value == !parity_even_bit(last_raw_value & 0b111111111111111110));
  //OCF should be 1 if everything's OK
  if(err_value.DECn || err_value.INCn || err_value.LIN || err_value.COF ||
             err_value.OCF != 1 || err_value.PARITY_OK){
    return 1;
  }
  else{
    return 0;
  }
}

uint32_t AS5311::read_chip(void)
{
  uint32_t raw_value = 0;
  uint16_t inputstream = 0;
  uint16_t c;
  digitalWrite(_cs, HIGH);
  digitalWrite(_clock, HIGH);
  delay(100);
  digitalWrite(_cs, LOW);
  delay(10);
  digitalWrite(_clock, LOW);
  delay(10);
  for (c = 0; c < 18; c++)
  {
    digitalWrite(_clock, HIGH);
    delay(10);
    inputstream = digitalRead(_data);
    raw_value = ((raw_value << 1) + inputstream);
    digitalWrite(_clock, LOW);
    delay(10);
  }
 last_raw_value = raw_value
 return raw_value;
}
  


