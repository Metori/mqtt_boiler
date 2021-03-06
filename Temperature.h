#ifndef _TEMPERATURE_H
#define _TEMPERATURE_H

#include "BoilerConfig.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class CTemperature {
public:
  CTemperature(int pin, unsigned long interval)
    : mOneWire(pin),
      mSensors(&mOneWire),
      mUpdateInterval(interval) {

  }
  ~CTemperature() {

  }

  bool init() {
    mSensors.begin();
    bool res = mSensors.getAddress(mSensorAddr, 0);
    if (res) {
      mSensors.setResolution(mSensorAddr, 9);

      measure();
      mLastTempUpdateTime = millis();
    }

    return res;
  }

  float update() {
    unsigned long t = millis();
    if (t - mLastTempUpdateTime > mUpdateInterval) {
      measure();
      mLastTempUpdateTime = t;
    }

    return mTemperatureValue;
  }

  float getValue() {
    return mTemperatureValue;
  }
  
private:
  void measure() {
    mSensors.requestTemperatures();
    mTemperatureValue = mSensors.getTempC(mSensorAddr) + gBoilerConfig.getTempOffset();
  }

  OneWire mOneWire;
  DallasTemperature mSensors;
  DeviceAddress mSensorAddr;
  
  float mTemperatureValue = 0;
  unsigned long mLastTempUpdateTime = 0;
  const unsigned long mUpdateInterval;
};

extern CTemperature gTemperature;

#endif // _TEMPERATURE_H
