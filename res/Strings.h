#ifndef _STRINGS_H
#define _STRINGS_H

#define PGMSTR(x) (__FlashStringHelper*)(x)

const char STR_CURRENT_TEMP[] PROGMEM      = "CURRENT TEMP";
const char STR_SET_TARGET_TEMP[] PROGMEM   = "SET TARGET TEMP";
const char STR_SELECT_POWER_MODE[] PROGMEM = "SELECT POWER MODE";
const char STR_MORE_OPTIONS[] PROGMEM      = "MORE OPTIONS";

const char STR_SELECT_TEMP[] PROGMEM       = "SELECT TEMP";

const char STR_ERROR[] PROGMEM             = "E";
const char STR_ARR_ERRORS[][22] PROGMEM = {
  "unknown",
  "temp sensor not found",
  "temp < 0",
  "temp > 90",
  "EEPROM failure"
};

#endif // _STRINGS_H