#include <Arduino.h>
#include <stdio.h>

/**
   The MySensors Arduino library handles the wireless radio link and protocol
   between your home built sensors/actuators and HA controller of choice.
   The sensors forms a self healing radio network with optional repeaters. Each
   repeater and gateway builds a routing tables in EEPROM which keeps track of the
   network topology allowing messages to be routed to nodes.

   Created by mboyer85
   Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors

   Documentation: http://www.mysensors.org
   Support Forum: http://forum.mysensors.org

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.

 *******************************

   DESCRIPTION

   ORP sensor from Atlas scientific

*/

#define MY_DEBUG

#define MY_NODE_ID 10

// Enable and select radio type attached
#define MY_RADIO_RFM69
#define MY_RFM69_NETWORKID 50

// sur un UNO SS en D13 et IRQ en PD2 il faut inverser les broches sur un Wemos D1

#ifdef ESP8266
#define MY_RF69_IRQ_PIN 15 // SS du RFM69 sur la GPIO15
#define MY_RF69_SPI_CS 16  // IRQ du RFM69 sur la GPIO16
#define MY_RF69_IRQ_NUM          15
#endif

#define SLEEP_TIME 10000
#define CHILD_ORP 0

#include "orp.h"

#include <MySensors.h>

boolean metric = true;

MyMessage msgORP(CHILD_ORP, V_ORP);

void setup()
{
  Init_ORP();
}

void presentation()
{
	sendSketchInfo("ORP Sensor", "1.0");
	present(CHILD_ORP, S_WATER_QUALITY);
}

void loop()
{
  float orp;
  Request_ORP();
  while(!(Read_ORP(&orp))) { yield();} // wait for orp acq
  send(msgORP.set(orp,1)); //send ORP to gateway
	delay(SLEEP_TIME);
}
