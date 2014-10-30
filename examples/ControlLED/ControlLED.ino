/*************************************************
 *  ControlLED.ino                               *
 *  Example from the ArduinoTILP library         *
 *           Created by Christopher Mitchell,    *
 *           2011-2014, all rights reserved.     *
 *                                               *
 *  This demo communicates as if it was a CBL2   *
 *  device. Use Send({0}) to send a 1-element    *
 *  list to the Arduino and control the state    *
 *  of digital output lines. You can expand      *
 *  this demo to read or write any GPIO lines.   *
 *                                               *
 *  In its current state, send a value between   *
 *  0 and 15 as the single element of a 1-       *
 *  element list to control digital pins 8, 9,   *
 *  10, and 13.                                  *
 *************************************************/

#include "CBL2.h"

CBL2* cbl;
int lineRed = 7;
int lineWhite = 6;

#define LED_PIN_COUNT 4
int ledPins[LED_PIN_COUNT] = {8, 9, 10, 13};

#define MAXDATALEN 255
uint8_t header[16];
uint8_t data[MAXDATALEN];

void setup() {
  for(int i = 0; i < LED_PIN_COUNT; i++) {
    pinMode(ledPins[i], OUTPUT);
	digitalWrite(ledPins[i], LOW);
  }

  Serial.begin(9600);
  cbl = new CBL2(lineRed, lineWhite);
  cbl->resetLines();
  // cbl->setVerbosity(true, &Serial);			// Comment this in for mesage information
  cbl->setupCallbacks(header, data, MAXDATALEN,
                      onGetAsCBL2, onSendAsCBL2);
}

void loop() {
  int rval;
  rval = cbl->eventLoopTick();
  if (rval && rval != ERR_READ_TIMEOUT) {
    Serial.print("Failed to run eventLoopTick: code ");
    Serial.println(rval);
  }
}

int onGetAsCBL2(uint8_t type, int datalen) {
  Serial.print("Got variable of type ");
  Serial.print(type);
  Serial.println(" from calculator.");
  
  // Turn the LEDs on or off
  int list_len = data[0] | (data[1] << 8);
  if (list_len == 1) {
    // It is a 1-element list now
    if (data[2] == 0) {
      // 1-element list where element 1 is positive
	  int16_t exp = ((int16_t)data[3]) - 0x80;
	  if (exp >= 0 && exp <= 1) {
	    // Compute the single number sent
		int value = (10 * (data[4] >> 4) + (data[4] & 0x0f)) / (exp?1:10);
		Serial.print("Received value ");
		Serial.println(value);
		for(int i = 0; i < LED_PIN_COUNT; i++) {
          digitalWrite(ledPins[i], (value >> i) & 0x01);
		}
	  } else {
        Serial.println("list element 1 must be a value 0-15");
	  }
    } else {
      Serial.println("list element 1 must be positive");
    }
  } else {
    Serial.println("Must send a 1-element list!");
  }
  return 0;
}

int onSendAsCBL2(uint8_t type, int* datalen) {
  Serial.print("Got request for variable of type ");
  Serial.print(type);
  Serial.println(" from calculator.");
  return -1;
}