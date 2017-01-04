#include <Arduino.h>
#include <Wire.h>
#include "Orp.h"

// time when data are avaibale
unsigned int time_data_avaiable = 0;

// request pending
boolean pending_request = false;


void Init_ORP()
{
  Wire.begin();
  pending_request = false;
}

void Request_ORP()
{
  pending_request = true;
  Wire.beginTransmission(AS_ORP_ID); // call the circuit by its ID number.
  Wire.write('r');        		        // request a reading by sending 'r'
  Wire.endTransmission();          	        // end the I2C data transmission.
  time_data_avaiable = millis() + AS_READ_TIME; // calculate the next time to request a reading
}


boolean Read_ORP(float *presult)
{
  uint8_t code;
  char sensorData[8];
  unsigned int bytes_received = 0;

  if(!pending_request || millis() < time_data_avaiable) // no request or request not ready
    return false;
  Wire.requestFrom(AS_ORP_ID, 9, 1);    // call the circuit and request 9 bytes (1 for status and 8 max for value).
  code = Wire.read(); // read return code before data
  if(code != 1)  // an error occurs
    return false;
  while (Wire.available() && bytes_received < 9)
    sensorData[bytes_received++] = Wire.read();          // are there bytes to receive?
  sscanf(sensorData, "%f", presult);
  return true;
}
