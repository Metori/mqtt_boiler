#ifndef _HEATER_H
#define _HEATER_H

#define RELAY_ON 0
#define RELAY_OFF 1

class CHeater {
public:
  CHeater(int pinLo, int pinHi)
    : mPinLo(pinLo),
      mPinHi(pinHi),
      mPowerOn(false) {
    pinMode(pinLo, OUTPUT);
    pinMode(pinHi, OUTPUT);
    updateRelays();
  }
  ~CHeater() {

  }

  bool isEnabled() {
    return mPowerOn;
  }

  void enable() {
    mPowerOn = true;
    updateRelays();
  }

  void disable() {
    mPowerOn = false;
    updateRelays();
  }

  void updateRelays() {
    if (mPowerOn) {
      switch (gBoilerConfig.getPowerMode()) {
        case EPowerMode::POWER_LOW:
          digitalWrite(mPinLo, RELAY_ON);
          digitalWrite(mPinHi, RELAY_OFF);
          break;
        case EPowerMode::POWER_MEDIUM:
          digitalWrite(mPinLo, RELAY_OFF);
          digitalWrite(mPinHi, RELAY_ON);
          break;
        default:
          digitalWrite(mPinLo, RELAY_ON);
          digitalWrite(mPinHi, RELAY_ON);
          break;
      }
    } else {
      digitalWrite(mPinLo, RELAY_OFF);
      digitalWrite(mPinHi, RELAY_OFF);
    }
  }

private:
  int mPinLo;
  int mPinHi;
  bool mPowerOn;
};

extern CHeater gHeater;

#endif // _HEATER_H
