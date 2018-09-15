#include "BoilerConfig.h"
#include "Temperature.h"
#include "Screen.h"
#include "Controls.h"
#include "Heater.h"
#include "res/Strings.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
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
 */

#define DEVICE_NAME "MQTT Boiler controller by Artem Pinchuk"
#define DEVICE_HW_VERSION "0.1"
#define DEVICE_SW_VERSION "0.6"

// ***** CONFIG *****
// Hardware configuration
#define PIN_THERMOMETER 10
#define PIN_OLED_RES 1
#define PIN_OLED_DC 3
#define PIN_OLED_CS 15
#define PIN_POT_SW 2
#define PIN_POT_DT 16
#define PIN_POT_CLK 0
#define PIN_HEATER_LO_RELAY 4
#define PIN_HEATER_HI_RELAY 5

// Software configuration
#define POT_DEBOUNCE_MS 15
#define POT_SW_DEBOUNCE_MS 25

#define TEMP_UPDATE_INTERVAL_MS 2000
#define TEMP_VALID_MIN_C 0
#define TEMP_VALID_MAX_C 90

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

// High level status protocol messages
const uint8_t MSG_STATUS_OUT_READY = 0x52; //'R'
const char* MSG_STATUS_OUT_LAST_WILL = "L"; //null terminated 'L'
const uint8_t MSG_STATUS_IN_PING = 0x50; //P
// ***** END OF CONFIG *****

CHeater gHeater(PIN_HEATER_LO_RELAY, PIN_HEATER_HI_RELAY);

CControls gControls(PIN_POT_CLK,
                    PIN_POT_DT,
                    PIN_POT_SW,
                    POT_DEBOUNCE_MS,
                    POT_SW_DEBOUNCE_MS);

Adafruit_SSD1306 gDisp(PIN_OLED_DC, PIN_OLED_RES, PIN_OLED_CS);
CScreen* curScreenPtr = nullptr;
CTemperature gTemperature(PIN_THERMOMETER, TEMP_UPDATE_INTERVAL_MS);
CBoilerConfig gBoilerConfig;

bool error = false;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void msgStatusSend(uint8_t msg) {
  mqttClient.publish(MQTT_STATUS_TOPIC_OUT, &msg, 1);
}

void msgSend(const char* msg) {
  mqttClient.publish(MQTT_TOPIC_OUT, msg);
}

bool mqttReconnect() {
  // Serial.println();
  // Serial.println();
  // Serial.print("(Re)connecting to MQTT server on ");
  // Serial.print(MQTT_SERVER_ADDR);
  // Serial.print("...");

  for (int i = 0; !mqttClient.connected(); i++) {
    if (mqttClient.connect(MQTT_CLIENT_ID,
                           MQTT_USER_NAME,
                           MQTT_USER_PASSWORD,
                           MQTT_STATUS_TOPIC_OUT,
                           0,
                           0,
                           MSG_STATUS_OUT_LAST_WILL)) {
      msgSend(MSG_STATUS_OUT_READY);
      mqttClient.subscribe(MQTT_STATUS_TOPIC_IN);
      mqttClient.subscribe(MQTT_TOPIC_IN);
    } else {
      delay(5000);
      if (i >= 5) return false;
    }
  }
  // Serial.println(" Done");
  // Serial.println("Current state: IDLE");
  // digitalWrite(PIN_LED_STATUS, LED_ON);
  return true;
}

void wifiDisconnect() {
  WiFi.disconnect();
  // digitalWrite(PIN_LED_STATUS, LED_OFF);
}

void wifiConnect() {
  // Serial.println();
  // Serial.println();
  // Serial.print("(Re)connecting to Wi-Fi \"");
  // Serial.print(WIFI_SSID);
  // Serial.print("\"...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  // Serial.println(" Done");
  // Serial.println("IP address: ");
  // Serial.print(WiFi.localIP());
}

void wifiReconnect() {
  wifiDisconnect();
  wifiConnect();
}

void allReconnect() {
  do {
    if (WiFi.status() != WL_CONNECTED) {
      wifiReconnect();
    }
  } while(!mqttReconnect());
}

void onMqttMsgReceived(char* topic, byte* payload, unsigned int len) {
  // Status request
  if (!strcmp(topic, MQTT_TOPIC_STATUS_IN)) {
    if (len != 1) return;
    uint8_t cmd = (uint8_t)payload[0];
    switch (cmd) {
      case MSG_IN_PING:
        msgStatusSend(MSG_STATUS_OUT_READY);
        break;

      default:
        break;
    }
  }
  // Control request
  else if (!strcmp(topic, MQTT_TOPIC_IN)) {
    //TODO
  }
}

void mqttInit() {
  mqttClient.setServer(MQTT_SERVER_ADDR, MQTT_SERVER_PORT);
  mqttClient.setCallback(onMqttMsgReceived);
}

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
    curScreenPtr = new CCurrentTempScreen();
  }
  
  mqttInit();
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
  
  if ((WiFi.status() != WL_CONNECTED) || (!mqttClient.connected())) {
    allReconnect();
  }

  mqttClient.loop();
}

