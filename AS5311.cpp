#include "Arduino.h"
#include "AS5311.h"
#include "digitalWriteFast.h"

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


//void interrupt(){
    
//}
//  attachInterrupt(13, interrupt, RISING);
// we can't just count index pulses, since there's no indication of the direction
// of travel. however, if there is a jump in position and an index pulse flag was set,
// we know we overflowed.


double AS5311::encoder_position_um(void)
{
  //returns micrometers
  return ((encoder_value() * POLE_SPACING)/4096);
}

// we also want to be able to associate a specific error code with a specific reading (if the parity fails, we want to throw out that specific value)
// Should have a ::read function and then a position / value decoder 
uint32_t AS5311::encoder_value(void)
{
  return (this->last_raw_reading >> 6);
}

bool has_even_parity(uint32_t x){
  //https://stackoverflow.com/a/21618054
    uint32_t count = 0, i, b = 1;

    for(i = 0; i < 32; i++){
        if( x & (b << i) ){count++;}
    }

    if( (count % 2) ){return 0;}

    return 1;
}

bool AS5311::parity_check(uint32_t raw_value){
  // "The parity bit ensures that the total number of 1-bits in the string is even or odd.[1] 
  // Accordingly, there are two variants of parity bits: even parity bit and odd parity bit. 
  // In the case of even parity, for a given set of bits, the bits whose value is 1 are counted. 
  // If that count is odd, the parity bit value is set to 1, 
  // making the total count of occurrences of 1s in the whole set (including the parity bit) an even number. "
  // https://en.wikipedia.org/wiki/Parity_bit
  uint32_t message_except_parity = (raw_value >> 1);

  bool parity_bit = (raw_value & 0b1);

  // First I tried <util/parity.h>
  // but thanks to unittesting, found this only works on 8 bits.

  // Then tried __builtin_parity is a GCC builtin 
  // https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
  
  // Note the different definitions of parity functions.
  // some *give you* the bit you need for even parity:
  // some *check if* the string has even parity. 
  return (parity_bit != has_even_parity(message_except_parity));
}


uint32_t AS5311::encoder_error()
{

  uint32_t error_code = this->last_raw_reading & 0b000000000000111111;
  err_value.DECn = error_code & 2;
  err_value.INCn = error_code & 4;
  err_value.LIN = error_code & 8;
  err_value.COF = error_code & 16;
  err_value.OCF = error_code & 32;
  
  err_value.PARITY_OK = AS5311::parity_check(this->last_raw_reading);
  //OCF should be 1 if everything's OK
  if(err_value.DECn || err_value.INCn || err_value.LIN || err_value.COF ||
             err_value.OCF == 0 || err_value.PARITY_OK == 0){
    return 1;
  }
  else{
    return 0;
  }
}

#define TIMING_CYCLE_TIME_US 100

uint32_t AS5311::read(void)
{
  uint32_t raw_value = 0;
  uint16_t inputstream = 0;
  uint16_t c;
  digitalWriteFast(_cs, HIGH);
  digitalWriteFast(_clock, HIGH);
  delayMicroseconds(TIMING_CYCLE_TIME_US);
  digitalWriteFast(_cs, LOW);
  delayMicroseconds(TIMING_CYCLE_TIME_US);
  digitalWriteFast(_clock, LOW);
  delayMicroseconds(TIMING_CYCLE_TIME_US);
  for (c = 0; c < 18; c++)
  {
    digitalWriteFast(_clock, HIGH);
    delayMicroseconds(TIMING_CYCLE_TIME_US);
    inputstream = digitalReadFast(_data);
    raw_value = ((raw_value << 1) + inputstream);
    digitalWriteFast(_clock, LOW);
    delayMicroseconds(TIMING_CYCLE_TIME_US);
  }
 this->last_raw_reading = raw_value;
 return raw_value;
}
  


