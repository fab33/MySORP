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
  if (pending_request != ORP_WAITING)  // if doing something return !
    return;
  pending_request = ORP_READING;
  Wire.beginTransmission(AS_ORP_ID); // call the circuit by its ID number.
  Wire.write('r');        		        // request a reading by sending 'r'
  Wire.endTransmission();          	        // end the I2C data transmission.
  time_data_avaiable = millis() + READ_TIME; // calculate the next time to request a reading
}

boolean Read_ORP(float *presult)
{
  uint8_t code;
  char sensorData[8];
  unsigned int bytes_received = 0;

  if(pending_request == ORP_READING && millis() > time_data_avaiable) {
    Wire.requestFrom(AS_ORP_ID, 9, 1);    // call the circuit and request 9 bytes (1 for status and 8 max for value).
    code = Wire.read(); // read return code before data
    time_data_avaiable = 0; // no more data
    pending_request = ORP_WAITING;
    if(code != 1)  // an error occurs
      return false;
    while (Wire.available() && bytes_received < 9)
      sensorData[bytes_received++] = Wire.read();          // are there bytes to receive?
    sscanf(sensorData, "%f", presult);
    return true;
  }
  else {
    return false;
  }
}

void Calibrate_ORP(float *val)
{
  if (pending_request != ORP_WAITING)
    return;
  if(*val<0 || *val>1000)
    return;
  char tmp[11] = "Cal,";
  dtostrf(*val,3,1, &tmp[4]); // We have to use dtostrf because Arduino doesn't support float in snprintf
  Wire.beginTransmission(AS_ORP_ID); // call the circuit by its ID number.
  Wire.write(tmp);        		        // request a calibration with cal_value
  Wire.endTransmission();          	        // end the I2C data transmission.
  time_data_avaiable = millis() + CAL_TIME;
  pending_request = ORP_CALIBRATING;
  *val = 0;
  return;
}

boolean Calibrate_response_ORP(boolean* cal)
{
  uint8_t code;

  if(pending_request == ORP_CALIBRATING && millis() > time_data_avaiable) {
    Wire.requestFrom(AS_ORP_ID, 9, 1);    // call the circuit and request 9 bytes (1 for status and 8 max for value).
    code = Wire.read(); // read return code before data
    while (Wire.available()) { Wire.read(); } // flush bytes
    if(code == 1)  {
      *cal = CALIBRATION_OK;
    }
    else {
      *cal = CALIBRATION_FAIL;
      Serial.printf("Code : %d\n", code);
    }
    time_data_avaiable = 0;
    pending_request = ORP_WAITING;
    return true;
  }
  else {
    return false;
  }
}

boolean Get_ORPCal() {
  uint8_t code;
  char cal;
  uint8_t i;

  Wire.beginTransmission(AS_ORP_ID); // call the circuit by its ID number.
  Wire.write("Cal,?");        		        // request a calibration with cal_value
  Wire.endTransmission();          	        // end the I2C data transmission.  pending_request = ORP_CALIBRATING;
  delay(300); // wait 300ms for response
  Wire.requestFrom(AS_ORP_ID, 9, 1);    // call the circuit and request 9 bytes (1 for status and 8 max for value).
  code = Wire.read(); // read return code before data
  if (code != 1) { return false; }
  for(i=0; i<5; i++) {
    Serial.print((char)Wire.read()); // read return code before data
  }
  cal = Wire.read(); // read cal status (ASCII !)
  Serial.print((char)cal);
  while (Wire.available()) { Wire.read(); } // flush bytes
  if(cal == '1')  {
    return true;
  }
  else {
    return false;
  }
}
