#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <cmath>

/* HW history
 *  0.1 - Initial HW for testing
 */

/* SW history
 *  0.1 - Initial code. Can only measure temperature
 */

#define DEVICE_NAME "MQTT Boiler controller by Artem Pinchuk"
#define DEVICE_HW_VERSION "0.1"
#define DEVICE_SW_VERSION "0.1"

// ***** CONFIG *****
// Hardware configuration
#define ONE_WIRE_BUS 2
#define OLED_DC 4
#define OLED_CS 15
#define OLED_RES 5
// ***** END OF CONFIG *****

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

Adafruit_SSD1306 display(OLED_DC, OLED_RES, OLED_CS);

float gTemperature;

float updateTemperature() {
  Serial.print("Requesting temperature... ");
  sensors.requestTemperatures();
  Serial.println("DONE");

  gTemperature = sensors.getTempCByIndex(0);
  
  Serial.print("Temperature is: ");
  Serial.println(gTemperature);
}

void updateDisplay() {
  int8 rounded = (int8)round(gTemperature);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println(rounded);
  display.setTextSize(2);
  display.setCursor(10,40);
  display.println(rounded);
  display.setTextSize(3);
  display.setCursor(74,40);
  display.println(rounded);

  display.display();
}

void setup(void) {
  Serial.begin(9600);
  Serial.println(DEVICE_NAME " HW Ver. " DEVICE_HW_VERSION " SW Ver. " DEVICE_SW_VERSION);

  sensors.begin();
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
}

void loop(void) {
  updateTemperature();
  updateDisplay();
}

