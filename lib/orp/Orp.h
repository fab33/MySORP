#include <Arduino.h>

// I2C ID of ORP sensor
#ifndef AS_ORP_ID
#define AS_ORP_ID 0x62
#endif

#define ORP_WAITING 0
#define ORP_READING 1
#define ORP_CALIBRATING 2

#define CALIBRATION_OK true
#define CALIBRATION_FAIL false

// time for reading data (in ms)
#define READ_TIME 1000
#define CAL_TIME 1300

// function definition
// Initialize library
void Init_ORP();

// Request ORP reading
void Request_ORP();

// return ORP value in result and true if value is valid else false
boolean Read_ORP(float *presult);

// Start calibration of ORP sensor
void Calibrate_ORP(float cal_value);

boolean Read_Cal(boolean *cal);
