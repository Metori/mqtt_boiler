#include "BoilerConfig.h"
#include "Temperature.h"
#include "Screen.h"
#include "Controls.h"
#include "Heater.h"
#include "Network.h"
#include "res/Strings.h"
#include <cmath>

/* HW history
 * 0.1 - Initial HW for testing
 */

/* SW history
 * 0.1 - Initial code. Can only measure temperature
 * 0.2 - Controls, Screens, BoilerConfig added. Development is in progress
 * 0.3 - Temperature element added. Code style fixes. Development is in progress
 * 0.4 - Heater component added. Development is in progress
 * 0.5 - Tuning
 * 0.6 - Wifi and MQTT initial code added
 * 0.7 - Temp offset introduced. pin remap
 */

#define DEVICE_NAME "MQTT Boiler controller by Artem Pinchuk"
#define DEVICE_HW_VERSION "0.1"
#define DEVICE_SW_VERSION "0.7"

// ***** CONFIG *****
// Hardware configuration
#define PIN_THERMOMETER 10
#define PIN_OLED_RES 1
#define PIN_OLED_DC 3
#define PIN_OLED_CS 15
#define PIN_POT_SW 0
#define PIN_POT_DT 16
#define PIN_POT_CLK 2
#define PIN_HEATER_LO_RELAY 4
#define PIN_HEATER_HI_RELAY 5

// Software configuration
#define POT_DEBOUNCE_MS 15
#define POT_SW_DEBOUNCE_MS 25

#define TEMP_UPDATE_INTERVAL_MS 2000
#define TEMP_VALID_MIN_C 0.0f
#define TEMP_VALID_MAX_C 90.0f

#define SCREEN_CONNECTED_TIMEOUT_MS 10000

const char* WIFI_SSID = "WIFI_SSID";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";
const char* MQTT_SERVER_ADDR = "1.2.3.4";
const uint16_t MQTT_SERVER_PORT = 1883;
const char* MQTT_USER_NAME = "MQTT_USER_NAME";
const char* MQTT_USER_PASSWORD = "MQTT_USER_PASSWORD";
const char* MQTT_CLIENT_ID = "boiler";
const char* MQTT_TOPIC_IN = "boiler/data/in";
const char* MQTT_TOPIC_OUT = "boiler/data/out";
const char* MQTT_TOPIC_STATUS_IN = "boiler/status/in";
const char* MQTT_TOPIC_STATUS_OUT = "boiler/status/out";
// ***** END OF CONFIG *****

CHeater gHeater(PIN_HEATER_LO_RELAY, PIN_HEATER_HI_RELAY);

CControls gControls(PIN_POT_CLK,
                    PIN_POT_DT,
                    PIN_POT_SW,
                    POT_DEBOUNCE_MS,
                    POT_SW_DEBOUNCE_MS);

Adafruit_SSD1306 gDisp(PIN_OLED_DC,
                       PIN_OLED_RES,
                       PIN_OLED_CS);

CScreen* curScreenPtr = nullptr;

CTemperature gTemperature(PIN_THERMOMETER,
                          TEMP_UPDATE_INTERVAL_MS);

CBoilerConfig gBoilerConfig;
CNetwork gNetwork({
  WIFI_SSID,
  WIFI_PASSWORD,
  MQTT_SERVER_ADDR,
  MQTT_SERVER_PORT,
  MQTT_USER_NAME,
  MQTT_USER_PASSWORD,
  MQTT_CLIENT_ID,
  MQTT_TOPIC_IN,
  MQTT_TOPIC_OUT,
  MQTT_TOPIC_STATUS_IN,
  MQTT_TOPIC_STATUS_OUT
});

bool error = false;
bool lastNetworkConnectedStatus = false;

void setupDisplay() {
  gDisp.begin(SSD1306_SWITCHCAPVCC);
  gDisp.clearDisplay();
}

void updateDisplay() {
  curScreenPtr = curScreenPtr->transition();
  curScreenPtr->draw();
}

void setup(void) {
  //Serial.begin(9600);
  //Serial.println(DEVICE_NAME " HW Ver. " DEVICE_HW_VERSION " SW Ver. " DEVICE_SW_VERSION);

  setupDisplay();

  // Startup error handling
  if (!gBoilerConfig.init()) {
    curScreenPtr = new CErrorScreen(EError::ERR_EEPROM_FAIL);
    error = true;
  }
  else if (!gTemperature.init()) {
    curScreenPtr = new CErrorScreen(EError::ERR_TEMP_SENSOR);
    error = true;
  }
  else if (gBoilerConfig.isFactoryDefault()) {
    curScreenPtr = new CMessageScreen(FPSTR(STR_INITIAL_MSG_CAP),
                                      FPSTR(STR_INITIAL_MSG_TEXT),
                                      new CMainScreen(1));
  }
  else {
    curScreenPtr = new CMessageScreen(FPSTR(STR_CONNECTING_MSG_CAP),
                                      FPSTR(STR_CONNECTING_MSG_TEXT),
                                      new CCurrentTempScreen());
  }
}

void loop(void) {
  if (!error) {
    float t = gTemperature.update();

    // Runtime error handling
    if (t < TEMP_VALID_MIN_C || t > TEMP_VALID_MAX_C) {
      gHeater.disable();

      error = true;
      delete curScreenPtr;
      if (t < TEMP_VALID_MIN_C) {
        curScreenPtr = new CErrorScreen(EError::ERR_TEMP_TOO_LOW);
      }
      else if (t > TEMP_VALID_MAX_C) {
        curScreenPtr = new CErrorScreen(EError::ERR_TEMP_TOO_HIGH);
      }
    }

    if (!error) {
      // Thermostat
      float target = (float)gBoilerConfig.getTargetTemp();
      float tol = gBoilerConfig.getTempHoldTolerance();
      bool heating = gHeater.isEnabled();
      if (!heating && ( t < (target - tol) )) {
        gHeater.enable();
      }
      else if (heating && ( t > (target + tol) )) {
        gHeater.disable();
      }
    }
  }

  // Need to update display and connectivity even in error condition
  updateDisplay();
  gNetwork.loop();

  bool isConnected = gNetwork.isConnected();
  if (isConnected != lastNetworkConnectedStatus) {
    delete curScreenPtr;
    if (isConnected) {
      curScreenPtr = new CMessageScreen(FPSTR(STR_CONNECTED_MSG_CAP),
                                        FPSTR(STR_CONNECTED_MSG_TEXT),
                                        new CCurrentTempScreen(),
                                        SCREEN_CONNECTED_TIMEOUT_MS);
    } else {
      curScreenPtr = new CMessageScreen(FPSTR(STR_CONN_LOST_MSG_CAP),
                                        FPSTR(STR_CONN_LOST_MSG_TEXT),
                                        new CCurrentTempScreen());
    }
    lastNetworkConnectedStatus = isConnected;
  }
}

