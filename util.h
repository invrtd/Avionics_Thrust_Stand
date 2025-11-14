#include <Arduino.h>
#pragma once 

/**
 * @brief Build an NMEA-like sentence with any number of fields.
 * @example build_nmea("power", voltage, amps, watts)
 */
template<typename... Args>
String build_nmea(const String& type, Args... args) {
    String msg = "$" + type;

    ((msg += "," + String(args)), ...);

    msg += ",";
    msg += String(millis());

    uint8_t cs = 0;
    for (int i = 1; i < msg.length(); i++)
        cs ^= msg[i];

    char buf[8];
    sprintf(buf, "*%02X\r\n", cs);
    msg += buf;

    return msg;
}
