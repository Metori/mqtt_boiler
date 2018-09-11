#ifndef _HEATER_H
#define _HEATER_H

#define RELAY_ON 0
#define RELAY_OFF 1

typedef enum {
  HEATER_OFF,
  HEATER_ONE_LO,
  HEATER_ONE_HI,
  HEATER_BOTH
} EHeaterPower;

class CHeater {
public:
  CHeater(int pinLo, int pinHi)
    : mPinLo(pinLo),
      mPinHi(pinHi),
      mPower(EHeaterPower::HEATER_OFF) {
    pinMode(pinLo, OUTPUT);
    pinMode(pinHi, OUTPUT);
    updateRelays();
  }
  ~CHeater() {

  }

  EHeaterPower getPower() {
    return mPower;
  }
  void setPower(EHeaterPower power) {
    mPower = power;
    updateRelays();
  }
  
private:
  void updateRelays() {
    switch (mPower) {
      case EHeaterPower::HEATER_ONE_LO:
        digitalWrite(mPinLo, RELAY_ON);
        digitalWrite(mPinHi, RELAY_OFF);
        break;
      case EHeaterPower::HEATER_ONE_HI:
        digitalWrite(mPinLo, RELAY_OFF);
        digitalWrite(mPinHi, RELAY_ON);
        break;
      case EHeaterPower::HEATER_BOTH:
        digitalWrite(mPinLo, RELAY_ON);
        digitalWrite(mPinHi, RELAY_ON);
        break;
      default:
        digitalWrite(mPinLo, RELAY_OFF);
        digitalWrite(mPinHi, RELAY_OFF);
        break;
    }
  }

  int mPinLo;
  int mPinHi;
  EHeaterPower mPower;
};

extern CHeater gHeater;

#endif // _HEATER_H
