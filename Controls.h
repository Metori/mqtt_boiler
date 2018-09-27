#ifndef _CONTROLS_H
#define _CONTROLS_H

#include "Arduino.h"
#include <Bounce2.h>

#define LONG_CLICK_TIME_MS 1000

typedef enum {
  NO_EVENT,
  POT_STEP_CW,
  POT_STEP_CCW,
  POT_SW_PRESS,
  POT_SW_SHORT_CLICK,
  POT_SW_LONG_CLICK
} EControlEvent;

class CControls {
public:
  CControls(int pinPotClk, int pinPotDt, int pinPotSw, uint16_t potDebounce, uint16_t swDebounce) {
    pinMode(pinPotSw, INPUT_PULLUP);
    pinMode(pinPotDt, INPUT);
    pinMode(pinPotClk, INPUT);

    mDebouncerPotClk.attach(pinPotClk);
    mDebouncerPotClk.interval(potDebounce);
    mDebouncerPotDt.attach(pinPotDt);
    mDebouncerPotDt.interval(potDebounce);
    mDebouncerPotSw.attach(pinPotSw);
    mDebouncerPotSw.interval(swDebounce);
  }
  ~CControls() {
    
  }

  EControlEvent update();
  EControlEvent getEvent();
  void resetCurrentClick();

private:
  Bounce mDebouncerPotClk = Bounce();
  Bounce mDebouncerPotDt = Bounce();
  Bounce mDebouncerPotSw = Bounce();

  EControlEvent mEvent = EControlEvent::NO_EVENT;
  unsigned long mLastPressTime = 0;
  bool mLongClickEventFired = true;
};

extern CControls gControls;

#endif // _CONTROLS_H
