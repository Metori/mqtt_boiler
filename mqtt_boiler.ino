#include <OneWire.h>
#include <DallasTemperature.h>

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
// ***** END OF CONFIG *****

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float gTemperature;

float updateTemperature() {
  Serial.print("Requesting temperature... ");
  sensors.requestTemperatures();
  Serial.println("DONE");

  gTemperature = sensors.getTempCByIndex(0);
  
  Serial.print("Temperature is: ");
  Serial.println(gTemperature);
}

void setup(void) {
  Serial.begin(9600);
  Serial.println(DEVICE_NAME " HW Ver. " DEVICE_HW_VERSION " SW Ver. " DEVICE_SW_VERSION);

  sensors.begin();
}

void loop(void) {
  updateTemperature();
}

