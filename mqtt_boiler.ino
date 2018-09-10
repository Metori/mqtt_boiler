#include "BoilerConfig.h"
#include "Temperature.h"
#include "Screen.h"
#include "Controls.h"
#include <cmath>

/* HW history
 *  0.1 - Initial HW for testing
 */

/* SW history
 *  0.1 - Initial code. Can only measure temperature
 *  0.2 - Controls, Screens, BoilerConfig added. Development is in progress
 *  0.3 - Temperature element added. Code style fixes. Development is in progress
 */

#define DEVICE_NAME "MQTT Boiler controller by Artem Pinchuk"
#define DEVICE_HW_VERSION "0.1"
#define DEVICE_SW_VERSION "0.3"

// ***** CONFIG *****
// Hardware configuration
#define PIN_THERMOMETER 2
#define PIN_OLED_DC 0
#define PIN_OLED_CS 15
#define PIN_POT_SW 4
#define PIN_POT_DT 16
#define PIN_POT_CLK 5

#define POT_DEBOUNCE_MS 15
#define POT_SW_DEBOUNCE_MS 25

#define TEMP_UPDATE_INTERVAL_MS 5000
// ***** END OF CONFIG *****

CControls gControls(PIN_POT_CLK,
                    PIN_POT_DT,
                    PIN_POT_SW,
                    POT_DEBOUNCE_MS,
                    POT_SW_DEBOUNCE_MS);

Adafruit_SSD1306 gDisp(PIN_OLED_DC, -1, PIN_OLED_CS);
CScreen* curScreenPtr = nullptr;
CTemperature gTemperature(PIN_THERMOMETER, TEMP_UPDATE_INTERVAL_MS);
CBoilerConfig gBoilerConfig;

void setupDisplay() {
  gDisp.begin(SSD1306_SWITCHCAPVCC);
  gDisp.clearDisplay();

  curScreenPtr = new CCurrentTempScreen();
}

void updateDisplay() {
  curScreenPtr = curScreenPtr->transition();
  curScreenPtr->draw();
}

void setup(void) {
  setupDisplay();

  Serial.begin(9600);
  Serial.println(DEVICE_NAME " HW Ver. " DEVICE_HW_VERSION " SW Ver. " DEVICE_SW_VERSION);
}

void loop(void) {
  gTemperature.update();
  updateDisplay();
}

