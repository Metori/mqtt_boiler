#include "BoilerConfig.h"
#include "Temperature.h"
#include "Screen.h"
#include "Controls.h"
#include "Heater.h"
#include "res/Strings.h"
#include <cmath>

/* HW history
 *  0.1 - Initial HW for testing
 */

/* SW history
 * 0.1 - Initial code. Can only measure temperature
 * 0.2 - Controls, Screens, BoilerConfig added. Development is in progress
 * 0.3 - Temperature element added. Code style fixes. Development is in progress
 * 0.4 - Heater component added. Development is in progress
 */

#define DEVICE_NAME "MQTT Boiler controller by Artem Pinchuk"
#define DEVICE_HW_VERSION "0.1"
#define DEVICE_SW_VERSION "0.4"

// ***** CONFIG *****
// Hardware configuration
#define PIN_THERMOMETER 10
#define PIN_OLED_DC 3
#define PIN_OLED_CS 15
#define PIN_POT_SW 2
#define PIN_POT_DT 16
#define PIN_POT_CLK 0
#define PIN_HEATER_LO_RELAY 4
#define PIN_HEATER_HI_RELAY 5

#define POT_DEBOUNCE_MS 15
#define POT_SW_DEBOUNCE_MS 25

#define TEMP_UPDATE_INTERVAL_MS 5000
#define TEMP_VALID_MIN_C 0
#define TEMP_VALID_MAX_C 90
// ***** END OF CONFIG *****

CHeater gHeater(PIN_HEATER_LO_RELAY, PIN_HEATER_HI_RELAY);

CControls gControls(PIN_POT_CLK,
                    PIN_POT_DT,
                    PIN_POT_SW,
                    POT_DEBOUNCE_MS,
                    POT_SW_DEBOUNCE_MS);

Adafruit_SSD1306 gDisp(PIN_OLED_DC, -1, PIN_OLED_CS);
CScreen* curScreenPtr = nullptr;
CTemperature gTemperature(PIN_THERMOMETER, TEMP_UPDATE_INTERVAL_MS);
CBoilerConfig gBoilerConfig;

bool error = false;

void setupDisplay() {
  gDisp.begin(SSD1306_SWITCHCAPVCC);
  gDisp.clearDisplay();
}

void updateDisplay() {
  curScreenPtr = curScreenPtr->transition();
  curScreenPtr->draw();
}

void setup(void) {
  Serial.begin(9600);
  Serial.println(DEVICE_NAME " HW Ver. " DEVICE_HW_VERSION " SW Ver. " DEVICE_SW_VERSION);

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
    curScreenPtr = new CCurrentTempScreen();
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

  // Need to update display even in error condition
  updateDisplay();
}

