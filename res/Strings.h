#ifndef _STRINGS_H
#define _STRINGS_H

const char STR_CURRENT_TEMP[] PROGMEM          = "CURRENT TEMP";
const char STR_SET_TARGET_TEMP[] PROGMEM       = "SET TARGET TEMP";
const char STR_SELECT_POWER_MODE[] PROGMEM     = "SELECT POWER MODE";
const char STR_MORE_OPTIONS[] PROGMEM          = "MORE OPTIONS";

const char STR_SELECT_TEMP[] PROGMEM           = "SELECT TEMP";

const char STR_POWER_MODE_LO[] PROGMEM         = "LOW";
const char STR_POWER_MODE_ME[] PROGMEM         = "MEDIUM";
const char STR_POWER_MODE_HI[] PROGMEM         = "HIGH";

const char STR_MORE_DEVICE_STATUS[] PROGMEM    = "MAIN STATUS";
const char STR_MORE_NETWORK_STATUS[] PROGMEM   = "NETWORK STATUS";
const char STR_MORE_FW_VERSION[] PROGMEM       = "ABOUT";
const char STR_MORE_BACK[] PROGMEM             = "BACK";

const char STR_INITIAL_MSG_CAP[] PROGMEM       = "HELLO";
const char STR_INITIAL_MSG_TEXT[] PROGMEM      = "factory config used";

const char STR_CONNECTED_MSG_CAP[] PROGMEM     = "MQTT";
const char STR_CONNECTED_MSG_TEXT[] PROGMEM    = "Connected to server";

const char STR_CONNECTING_MSG_CAP[] PROGMEM    = "MQTT";
const char STR_CONNECTING_MSG_TEXT[] PROGMEM   = "Connecting...";

const char STR_CONN_LOST_MSG_CAP[] PROGMEM     = "MQTT";
const char STR_CONN_LOST_MSG_TEXT[] PROGMEM    = "Reconnecting...";

const char STR_ERROR[] PROGMEM                 = "E";
const char STR_ARR_ERRORS[][22] PROGMEM = {
  "unknown",
  "temp sensor not found",
  "temp < 0",
  "temp > 90",
  "EEPROM failure"
};

#endif // _STRINGS_H