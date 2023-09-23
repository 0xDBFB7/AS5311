#ifndef AS5311_h
#define AS5311_h

#include "Arduino.h"

class AS5311
{
  public:
    AS5311(uint16_t DataPin, uint16_t ClockPin, uint16_t ChipSelectPin, uint16_t IndexPin);
    void read(void);
    uint32_t read_raw_sensor_word(void);
    double encoder_position_within_pole_um(void);
    uint32_t encoder_raw_counts(void);
    uint32_t encoder_error(void); 
    bool parse_status(void);

    bool parity_check(uint32_t);
    uint32_t latest_raw_sensor_word = 0;
    uint32_t last_raw_valid_counts = 0;
    double accumulated_position = 0;
    struct err_struct{
  	bool DECn;
    bool INCn;
    bool OCF;
    bool COF;
    bool LIN; 
    bool PARITY_OK;
    bool READING_VALID;} err_value;
  private:
    const uint16_t _clock;        // clock pin: output from arduino to as5311
    const uint16_t _cs;           // chip select: output
    const uint16_t _data;         // data pin: input
    const uint16_t _index;	  // index pin: input
    
};

#endif
