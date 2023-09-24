#include "AS5311.h"

AS5311 myAS5311(2,3,4,5); // data, clock, chip select, index

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  myAS5311.read();

  long value;
  value = myAS5311.encoder_raw_counts();
  Serial.print("measured counts: ");
  Serial.println(value);
  double decimal_value;
  decimal_value = myAS5311.encoder_position_within_pole_um();
  Serial.print("measured position within pole: ");
  Serial.println(decimal_value);
  Serial.print("accumulated position: ");
  Serial.println(myAS5311.accumulated_position_um());  
  if (myAS5311.err_value.READING_VALID != true)
  {
    Serial.println("error detected.");
    if (myAS5311.err_value.DECn) Serial.println("DECn error");
    if (myAS5311.err_value.INCn) Serial.println("INCn error");
    if (myAS5311.err_value.COF) Serial.println("COF error");
    if (!myAS5311.err_value.OCF){
            Serial.println("Offset Compensation failed error");
    }
    if (!myAS5311.err_value.PARITY_OK){
      Serial.println("Parity check failed");
    }
    if (myAS5311.err_value.LIN) Serial.println("LIN error");
  }

  delay(10);
}
