/*************************************************
 *  HelloWorld.ino                               *
 *  Example from the ArTICL library              *
 *           Created by Alex Cordonnier, 2017    *
 *                                               *
 *  Use Get(Str1) to request a string from the   *
 *  Arduino. Use Send(Str1) to send a string to  *
 *  the Arduino.                                 *
 *************************************************/

#include "CBL2.h"
#include "TIVar.h"

CBL2 cbl;
const int lineRed = DEFAULT_TIP;
const int lineWhite = DEFAULT_RING;

#define MAXDATALEN 255

uint8_t header[16];
uint8_t data[MAXDATALEN];

void setup() {
    Serial.begin(9600);
    cbl.setLines(lineRed, lineWhite);
    cbl.resetLines();
    cbl.setupCallbacks(header, data, MAXDATALEN, onReceived, onRequest);
}

void loop() {
    cbl.eventLoopTick();
}

int onReceived(uint8_t type, enum Endpoint model, int datalen) {
    if (type != VarTypes82::VarString) {
        return -1; // Can only accept strings
    }

    String str = TIVar::strVarToString8x(data, model);
    Serial.println("Received: " + str);
    return 0;
}

int onRequest(uint8_t type, enum Endpoint model, int* headerlen,
              int* datalen, data_callback* data_callback) {
    if (type != VarTypes82::VarString) {
        return -1; // Can only return strings
    }

    String hello = "Hello, world! :)";
    int rval = TIVar::stringToStrVar8x(hello, data, model);
    if (rval < 0) {
        return -1;
    }
    *datalen = rval;

    memset(header, 0, sizeof(header));
    TIVar::intToSizeWord(rval, header);
    header[2] = VarTypes82::VarString; // Variable type
    header[3] = 0xAA; // Variable name (Str1)
    header[4] = 0x00; // ^
    *headerlen = 13;

    Serial.println("Sending: " + hello);

    return 0;
}