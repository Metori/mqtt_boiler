#ifndef _TEMPERATURE_H
#define _TEMPERATURE_H

#include <OneWire.h>
#include <DallasTemperature.h>

class CTemperature {
public:
  CTemperature(int pin, unsigned long interval)
    : mOneWire(pin),
      mSensors(&mOneWire),
      mUpdateInterval(interval) {
    mSensors.begin();
    mSensors.getAddress(mSensorAddr, 0);
    mSensors.setResolution(mSensorAddr, 9);

    mSensors.requestTemperatures();
    mTemperatureValue = mSensors.getTempC(mSensorAddr);
    mLastTempUpdateTime = millis();
  }
  ~CTemperature() {

  }

  float update() {
    unsigned long t = millis();
    if (t - mLastTempUpdateTime > mUpdateInterval) {
      mSensors.requestTemperatures();
      mTemperatureValue = mSensors.getTempC(mSensorAddr);
      mLastTempUpdateTime = t;
    }

    return mTemperatureValue;
  }

  float getValue() {
    return mTemperatureValue;
  }
  
private:
  OneWire mOneWire;
  DallasTemperature mSensors;
  DeviceAddress mSensorAddr;
  
  float mTemperatureValue = 0;
  unsigned long mLastTempUpdateTime = 0;
  const unsigned long mUpdateInterval;
};

extern CTemperature gTemperature;

#endif // _TEMPERATURE_H
