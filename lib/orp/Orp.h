#include <Arduino.h>

// I2C ID of ORP sensor
#ifndef AS_ORP_ID
#define AS_ORP_ID 0x62
#endif

// time for reading data (in ms)
#define READ_TIME 1000
#define CAL_TIME 5000

enum _calStatus {
  CAL_UNKOWN, CAL_OK, CAL_FAIL
};
typedef enum _calStatus _calStatus;

enum _smStatus {
  ORP_WAITING, ORP_READING, ORP_CALIBRATING
};
typedef enum _smStatus _smStatus;

struct orpData {
  float sensorValue;    // last reading value
  _calStatus calStatus; // Calibration status of sensor
  float calValue;       // last calibration value
  _smStatus smStatus;     // Status of state machine
  unsigned int timeDataAvaible;
};
typedef orpData orpData;


// function definition
// Initialize library
void initOrpSensor();

// Request ORP reading
void requestOrp();

// Data avaible
boolean orpDataAvailable();

// return ORP value and 0 if an error occurs
float getOrpValue();

// Request initial sensors calibration state
void requestSensorCalState();

// New cal status is available
boolean calStatusAvailable(); 

// Start calibration of ORP sensor
void calibrateOrpSensor(float _val);

// return Calibration status
_calStatus getOrpCalStatus();
