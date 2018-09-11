#ifndef _STRINGS_H
#define _STRINGS_H

#define PGMSTR(x) (__FlashStringHelper*)(x)

const char STR_CURRENT_TEMP[] PROGMEM      = "CURRENT TEMP";
const char STR_SET_TARGET_TEMP[] PROGMEM   = "SET TARGET TEMP";
const char STR_SELECT_POWER_MODE[] PROGMEM = "SELECT POWER MODE";
const char STR_MORE_OPTIONS[] PROGMEM      = "MORE OPTIONS";

const char STR_SELECT_TEMP[] PROGMEM       = "SELECT TEMP";

#endif // _STRINGS_H