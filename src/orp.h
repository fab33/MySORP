// I2C ID of ORP sensor
#ifndef AS_ORP_ID
#define AS_ORP_ID 98
#endif

// time for reading data (in ms)
#define AS_READ_TIME 1000

// function definition
// Initialize library
void Init_ORP();

// Request ORP reading
void Request_ORP();

// return ORP value in result and true if value is valid else false
boolean Read_ORP(float *presult);
