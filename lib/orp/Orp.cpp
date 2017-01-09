#include <Arduino.h>
#include <Wire.h>
#include <Orp.h>


// time when data are avaibale
unsigned int time_data_avaiable = 0;

// request pending
uint8_t pending_request = ORP_WAITING;

void Init_ORP()
{
  // Next code from http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html
  #if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
  #endif

  pinMode(SDA, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(SCL, INPUT_PULLUP);

  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power

  boolean SDA_LOW = (digitalRead(SDA) == LOW);  // vi. Check SDA input.
  int clockCount = 20; // > 2x9 clock

  while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
    clockCount--;
    // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(SCL, INPUT); // release SCL pullup so that when made output it will be LOW
    pinMode(SCL, OUTPUT); // then clock SCL Low
    delayMicroseconds(10); //  for >5uS
    pinMode(SCL, INPUT); // release SCL LOW
    pinMode(SCL, INPUT_PULLUP); // turn on pullup resistors again
    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS
    // The >5uS is so that even the slowest I2C devices are handled.
    boolean SCL_LOW = (digitalRead(SCL) == LOW); // Check if SCL is Low.
    int counter = 20;
    while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(SCL) == LOW);
    }
    if (SCL_LOW) { // still low after 2 sec error
      break; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    }
    SDA_LOW = (digitalRead(SDA) == LOW); //   and check SDA input again and loop
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(SDA, INPUT); // remove pullup.
  pinMode(SDA, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(SDA, INPUT); // remove output low
  pinMode(SDA, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10); // x. wait >5uS
  pinMode(SDA, INPUT); // and reset pins as tri-state inputs which is the default state on reset
  pinMode(SCL, INPUT);

  Wire.begin();
  pending_request = ORP_WAITING;
}

void Request_ORP()
{
  pending_request = ORP_READING;
  Wire.beginTransmission(AS_ORP_ID); // call the circuit by its ID number.
  Wire.write('r');        		        // request a reading by sending 'r'
  Wire.endTransmission();          	        // end the I2C data transmission.
  time_data_avaiable = millis() + READ_TIME; // calculate the next time to request a reading
}

boolean Read_ORP(float *presult)
{
  if(pending_request != ORP_READING) // We are not waiting for ORP reading
    return false;
  uint8_t code;
  char sensorData[8];
  unsigned int bytes_received = 0;

  if(!pending_request || millis() < time_data_avaiable) // no request or request not ready
    return false;
  Wire.requestFrom(AS_ORP_ID, 9, 1);    // call the circuit and request 9 bytes (1 for status and 8 max for value).
  code = Wire.read(); // read return code before data
  time_data_avaiable = 0; // no more data
  if(code != 1)  // an error occurs
    return false;
  while (Wire.available() && bytes_received < 9)
    sensorData[bytes_received++] = Wire.read();          // are there bytes to receive?
  sscanf(sensorData, "%f", presult);
  return true;
}

void Calibrate_ORP(float cal_value)
{
  return;
}

boolean Read_Cal(boolean *cal)
{
  return false;
}
