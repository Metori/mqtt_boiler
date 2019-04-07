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
    int oldPinLoState = digitalRead(mPinLo);
    int oldPinHiState = digitalRead(mPinHi);
    int newPinLoState;
    int newPinHiState;

    if (mPowerOn) {
      switch (gBoilerConfig.getPowerMode()) {
        case EPowerMode::POWER_LOW:
          newPinLoState = RELAY_ON;
          newPinHiState = RELAY_OFF;
          break;
        case EPowerMode::POWER_MEDIUM:
          newPinLoState = RELAY_OFF;
          newPinHiState = RELAY_ON;
          break;
        default:
          newPinLoState = RELAY_ON;
          newPinHiState = RELAY_ON;
          break;
      }
    } else {
      newPinLoState = RELAY_OFF;
      newPinHiState = RELAY_OFF;
    }

    if (newPinLoState != oldPinLoState) {
      digitalWrite(mPinLo, newPinLoState);
      gBoilerConfig.incRelayLoStat();
    }
    if (newPinHiState != oldPinHiState) {
      digitalWrite(mPinHi, newPinHiState);
      gBoilerConfig.incRelayHiStat();
    }
  }

private:
  int mPinLo;
  int mPinHi;
  bool mPowerOn;
};

extern CHeater gHeater;

#endif // _HEATER_H
