#include "Controls.h"

EControlEvent CControls::update() {
  bool clkChanged = mDebouncerPotClk.update();
  mDebouncerPotDt.update();
  if (clkChanged) {
    if (mDebouncerPotClk.read() == mDebouncerPotDt.read()) {
      mEvent = EControlEvent::POT_STEP_CCW;
    } else {
      mEvent = EControlEvent::POT_STEP_CW;
    }
  } else {
    mEvent = EControlEvent::NO_EVENT;
  }

  mDebouncerPotSw.update();
  if (mDebouncerPotSw.fell()) {
    mEvent = EControlEvent::POT_SW_PRESS;
  }
  else if (mDebouncerPotSw.rose()) {
    mEvent = EControlEvent::POT_SW_RELEASE;
  }
  
  return mEvent;
}

EControlEvent CControls::getEvent() {
  return mEvent;
}

