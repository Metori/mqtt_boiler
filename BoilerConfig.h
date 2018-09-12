#ifndef _BOILER_CONFIG_H
#define _BOILER_CONFIG_H

#include "EEPROM.h"

#define CONFIG_VALID_MAGIC 0x92
#define CONFIG_INITIAL_POWER_MODE EPowerMode::POWER_HIGH
#define CONFIG_INITIAL_TARGET_TEMP 60
#define CONFIG_INITIAL_TEMP_HOLD_TOL 1.0f

typedef enum {
  POWER_LOW,
  POWER_MEDIUM,
  POWER_HIGH
} EPowerMode;

class CBoilerConfig {
public:
  CBoilerConfig() {
    EEPROM.begin(512);
  }
  ~CBoilerConfig() {
    
  }

  bool init() {
    EEPROM.get(0, mMagic);

    if (mMagic != CONFIG_VALID_MAGIC) {
      mMagic = CONFIG_VALID_MAGIC;
      commitAll();
    }

    EEPROM.get(0, *this);

    return mMagic == CONFIG_VALID_MAGIC;
  }

  void commitAll() {
    EEPROM.put(0, *this);
    EEPROM.commit();
  }

  EPowerMode getPowerMode() {
    return mPowerMode;
  }
  void setPowerMode(EPowerMode mode) {
    mPowerMode = mode;
    commitAll();
  }

  int8 getTargetTemp() {
    return mTargetTemp;
  }
  void setTargetTemp(int8 temp) {
    mTargetTemp = temp;
    commitAll();
  }

  float getTempHoldTolerance() {
    return mTempHoldTolerance;
  }
  void setTempHoldTolerance(float tolerance) {
    mTempHoldTolerance = tolerance;
    commitAll();
  }

private:
  uint8_t mMagic = 0;
  EPowerMode mPowerMode = CONFIG_INITIAL_POWER_MODE;
  int8 mTargetTemp = CONFIG_INITIAL_TARGET_TEMP;
  float mTempHoldTolerance = CONFIG_INITIAL_TEMP_HOLD_TOL;
};

extern CBoilerConfig gBoilerConfig;

#endif // _BOILER_CONFIG_H
