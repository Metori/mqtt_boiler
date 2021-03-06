#ifndef _BOILER_CONFIG_H
#define _BOILER_CONFIG_H

#include "EEPROM.h"
#include <ArduinoJson.h>

#define ALLOW_STAT_CORRECTION

#define CONFIG_VALID_MAGIC 0x92
#define CONFIG_INITIAL_POWER_MODE EPowerMode::POWER_HIGH
#define CONFIG_INITIAL_TARGET_TEMP 60
#define CONFIG_INITIAL_TEMP_HOLD_TOL 2.0f
#define CONFIG_INITIAL_TEMP_OFFSET 12.0f
#define CONFIG_INITIAL_RELAY_LO_STAT 0
#define CONFIG_INITIAL_RELAY_HI_STAT 0
#define CONFIG_INITIAL_PAUSED false

#define JSON_FIELD_POWER_MODE "power_mode"
#define JSON_FIELD_TARGET_TEMP "target_temp"
#define JSON_FIELD_TEMP_HOLD_TOL "temp_hold_tol"
#define JSON_FIELD_TEMP_OFFSET "temp_offset"
#define JSON_FIELD_IS_PAUSED "is_paused"
#define JSON_FIELD_RELAY_LO_STAT "relay_lo_stat"
#define JSON_FIELD_RELAY_HI_STAT "relay_hi_stat"

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

  void toJson(JsonObject& root) {
    root[JSON_FIELD_POWER_MODE] = (uint8_t)mPowerMode;
    root[JSON_FIELD_TARGET_TEMP] = mTargetTemp;
    root[JSON_FIELD_TEMP_HOLD_TOL] = mTempHoldTolerance;
    root[JSON_FIELD_TEMP_OFFSET] = mTempOffset;
    root[JSON_FIELD_IS_PAUSED] = mIsPaused;
    root[JSON_FIELD_RELAY_LO_STAT] = mRelayLoStat;
    root[JSON_FIELD_RELAY_HI_STAT] = mRelayHiStat;
  }

  void fromJson(JsonObject& root) {
    JsonVariant val = root[JSON_FIELD_POWER_MODE];
    if (val.success()) setPowerMode(val.as<uint8_t>());
    val = root[JSON_FIELD_TARGET_TEMP];
    if (val.success()) setTargetTemp(val.as<int8_t>());
    val = root[JSON_FIELD_TEMP_HOLD_TOL];
    if (val.success()) setTempHoldTolerance(val.as<float>());
    val = root[JSON_FIELD_TEMP_OFFSET];
    if (val.success()) setTempOffset(val.as<float>());
    val = root[JSON_FIELD_IS_PAUSED];
    if (val.success()) setPaused(val.as<bool>());
#ifdef ALLOW_STAT_CORRECTION
    val = root[JSON_FIELD_RELAY_LO_STAT];
    if (val.success()) setRelayLoStat(val.as<uint32_t>());
    val = root[JSON_FIELD_RELAY_HI_STAT];
    if (val.success()) setRelayHiStat(val.as<uint32_t>());
#endif
  }

  bool isFactoryDefault() {
    bool ret = mIsDefault;
    if (ret) {
      mIsDefault = false;
      commitAll();
    }

    return ret;
  }

  EPowerMode getPowerMode() {
    return mPowerMode;
  }
  void setPowerMode(EPowerMode mode) {
    mPowerMode = mode;
    commitAll();
  }
  bool setPowerMode(uint8_t mode) {
    if (mode >= POWER_LOW && mode <= POWER_HIGH) {
      setPowerMode((EPowerMode)mode);
      return true;
    }
    return false;
  }

  int8_t getTargetTemp() {
    return mTargetTemp;
  }
  bool setTargetTemp(int8_t temp) {
    if (temp >= 30 && temp <= 90) {
      mTargetTemp = temp;
      commitAll();
      return true;
    }
    return false;
  }

  float getTempHoldTolerance() {
    return mTempHoldTolerance;
  }
  bool setTempHoldTolerance(float tolerance) {
    if (tolerance >= -10 && tolerance <= 10) {
      mTempHoldTolerance = tolerance;
      commitAll();
      return true;
    }
    return false;
  }

  float getTempOffset() {
    return mTempOffset;
  }
  void setTempOffset(float offset) {
    // Disabled for security reasons
#if 0
    mTempOffset = offset;
    commitAll();
#endif
  }

  bool isPaused() {
    return mIsPaused;
  }
  void setPaused(bool paused) {
    mIsPaused = paused;
    commitAll();
  }

  uint32_t getRelayLoStat() {
    return mRelayLoStat;
  }
  void incRelayLoStat() {
    mRelayLoStat++;
    commitAll();
  }
#ifdef ALLOW_STAT_CORRECTION
  void setRelayLoStat(uint32_t value) {
    mRelayLoStat = value;
    commitAll();
  }
#endif

  uint32_t getRelayHiStat() {
    return mRelayHiStat;
  }
  void incRelayHiStat() {
    mRelayHiStat++;
    commitAll();
  }
#ifdef ALLOW_STAT_CORRECTION
  void setRelayHiStat(uint32_t value) {
    mRelayHiStat = value;
    commitAll();
  }
#endif

private:
  uint8_t mMagic = 0;
  EPowerMode mPowerMode = CONFIG_INITIAL_POWER_MODE;
  int8_t mTargetTemp = CONFIG_INITIAL_TARGET_TEMP;
  float mTempHoldTolerance = CONFIG_INITIAL_TEMP_HOLD_TOL;
  float mTempOffset = CONFIG_INITIAL_TEMP_OFFSET;
  bool mIsPaused = CONFIG_INITIAL_PAUSED;
  uint32_t mRelayLoStat = CONFIG_INITIAL_RELAY_LO_STAT;
  uint32_t mRelayHiStat = CONFIG_INITIAL_RELAY_HI_STAT;
  bool mIsDefault = true;
};

extern CBoilerConfig gBoilerConfig;

#endif // _BOILER_CONFIG_H
