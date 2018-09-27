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

  unsigned long m = millis();
  mDebouncerPotSw.update();
  if (mDebouncerPotSw.fell()) {
    mEvent = EControlEvent::POT_SW_PRESS;
    mLastPressTime = m;
    mLongClickEventFired = false;
  }
  else if (mDebouncerPotSw.rose() &&
           !mLongClickEventFired) {
    mEvent = EControlEvent::POT_SW_SHORT_CLICK;
  }
  else if (!mDebouncerPotSw.read() && // SW is in pressed state now
           !mLongClickEventFired &&
           m - mLastPressTime > LONG_CLICK_TIME_MS) {
    mEvent = EControlEvent::POT_SW_LONG_CLICK;
    mLongClickEventFired = true;
  }
  
  return mEvent;
}

EControlEvent CControls::getEvent() {
  return mEvent;
}

void CControls::resetCurrentClick() {
  mLongClickEventFired = true;
}

