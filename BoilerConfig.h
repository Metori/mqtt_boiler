#ifndef _BOILER_CONFIG_H
#define _BOILER_CONFIG_H

#include "EEPROM.h"

typedef enum {
  POWER_OFF,
  POWER_LOW,
  POWER_MEDIUM,
  POWER_HIGH
} EPowerMode;

class CBoilerConfig {
public:
  CBoilerConfig() {
    EEPROM.begin(512);
    EEPROM.get(0, mPowerMode);
    EEPROM.get(sizeof(mPowerMode), mTargetTemp);
  }

  ~CBoilerConfig() {
    
  }

  EPowerMode getPowerMode() {
    return mPowerMode;
  }
  void setPowerMode(EPowerMode mode) {
    mPowerMode = mode;
    EEPROM.put(0, mPowerMode);
    EEPROM.commit();
  }

  int8 getTargetTemp() {
    return mTargetTemp;
  }
  void setTargetTemp(int8 temp) {
    mTargetTemp = temp;
    EEPROM.put(sizeof(mPowerMode), mTargetTemp);
    EEPROM.commit();
  }

private:
  EPowerMode mPowerMode;
  int8 mTargetTemp;
};

extern CBoilerConfig gBoilerConfig;

#endif // _BOILER_CONFIG_H
