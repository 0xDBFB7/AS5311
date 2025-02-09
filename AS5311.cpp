#include "Arduino.h"
#include "AS5311.h"
//#include "digitalWriteFast.h"

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

void AS5311::accumulate_poles(void){  
  double high_threshold = POLE_SPACING*(9.0/10.0);
  double low_threshold = POLE_SPACING*(1.0/10.0);
  if(this->previous_position_within_pole_um > high_threshold and this->latest_position_within_pole_um < low_threshold){
    this->accumulated_poles++;
  }
  if(this->previous_position_within_pole_um < low_threshold and this->latest_position_within_pole_um > high_threshold){
    this->accumulated_poles--;
  }
}

double AS5311::accumulated_position_um(void){  
  return (this->accumulated_poles * POLE_SPACING) + this->encoder_position_within_pole_um();
}



double AS5311::encoder_position_within_pole_um(void)
{
  //returns micrometers
  return ((encoder_raw_counts() * POLE_SPACING)/4096);
}

// we also want to be able to associate a specific error code with a specific reading (if the parity fails, we want to throw out that specific value)
// Should have a ::read function and then a position / value decoder 
uint32_t AS5311::encoder_raw_counts(void)
{
  return (this->latest_raw_sensor_word >> 6);
}


bool has_even_parity(uint32_t x){
  // from https://stackoverflow.com/a/21618054
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
  // which, because 'int' is 16 bit on Arduino, still doesn't work right.
  
  // Note the different definitions of parity functions.
  // some *give you* the bit you need for even parity:
  // some *check if* the string has even parity. 
  return (parity_bit != has_even_parity(message_except_parity));
}


bool AS5311::parse_status()
{
  uint32_t error_code = this->latest_raw_sensor_word & 0b000000000000111111; 

  err_value.DECn = error_code & 2;
  err_value.INCn = error_code & 4;
  err_value.LIN = error_code & 8;
  err_value.COF = error_code & 16;
  err_value.OCF = error_code & 32;
  
  err_value.PARITY_OK = AS5311::parity_check(this->latest_raw_sensor_word);
  
  /*
  OCF (Offset Compensation Finished) should be 1 if everything's OK.
  COF (Cordic Overflow), logic high indicates an out of range error in the CORDIC part. 
      When this bit is set, the data at D11:D0 (likewise M11:M0) is invalid.
      This alarm may be resolved by bringing the magnet within the X-Y-Z tolerance limits.
  LIN (Linearity Alarm), logic high indicates that the input field generates a critical output linearity. 
      When this bit is set, the data at D11:D0 may still be used, but can contain invalid data. 
      This warning can be resolved by increasing the magnetic field strength. 

  DECn and INCn are a little confusing. They will toggle when the field strength is 
  currently increasing or decreasing. Only if both are ON is the field strength actually low.


  */
  bool field_strength_yellow = (err_value.DECn == 1 && err_value.INCn == 1);

  if(field_strength_yellow || err_value.LIN || err_value.COF ||
             err_value.OCF == 0 || err_value.PARITY_OK == 0){
    err_value.READING_VALID = 0;
  }
  else{
    err_value.READING_VALID = 1;
  }
}



#define TIMING_CYCLE_TIME_US 100

uint32_t AS5311::read_raw_sensor_word(void)
{
  /*
  If CLK is low at the falling edge of CSn, the first 12 bits represent the magnitude information, 
  which is proportional to the magnetic field strength. 
  */
  // thanks to PJE66
  // I wanted to used WriteFast, but behaves weirdly with the constant
  uint32_t raw_value = 0;
  uint16_t inputstream = 0;
  uint16_t c;
   
  // Default State
  digitalWrite(_cs, HIGH);
  digitalWrite(_clock, HIGH);
  delayMicroseconds(TIMING_CYCLE_TIME_US); 
  // Preamble
  digitalWrite(_cs, LOW); 
  delayMicroseconds(TIMING_CYCLE_TIME_US); 
  digitalWrite(_clock, LOW);
  delayMicroseconds(TIMING_CYCLE_TIME_US); 
  digitalWrite(_clock, HIGH);
  // Clock out Data
  for (c = 0; c < 18; c++) {
      delayMicroseconds(TIMING_CYCLE_TIME_US); 
      digitalWrite(_clock, LOW);
      inputstream = digitalRead(_data);
      raw_value = ((raw_value << 1) + inputstream);
      delayMicroseconds(TIMING_CYCLE_TIME_US); 
      digitalWrite(_clock, HIGH);
  }
  delayMicroseconds(TIMING_CYCLE_TIME_US); 
  digitalWrite(_cs, HIGH);
  delayMicroseconds(TIMING_CYCLE_TIME_US); 

  return raw_value;

}
  
void AS5311::read(void){ 
  this->latest_raw_sensor_word = this->read_raw_sensor_word();
  this->parse_status();
  
  if(err_value.READING_VALID){
    this->previous_position_within_pole_um = this->latest_position_within_pole_um;
    
    this->latest_position_within_pole_um = this->encoder_position_within_pole_um();
    this->accumulate_poles();
  }
}
