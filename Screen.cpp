#include "Temperature.h"
#include "Screen.h"
#include "Controls.h"
#include "res/Bitmaps.h"
#include "res/Strings.h"

void printCentered(char* str, uint16_t x, uint16_t y) {
  int16_t x1, x2;
  uint16_t w, h;
  gDisp.getTextBounds(str, 0, 0, &x1, &x2, &w, &h);
  gDisp.setCursor(x - w / 2, y);
  gDisp.print(str);
}

CScreen* CScreen::transition() {
  CScreen* ret;

  EControlEvent event = gControls.update();

  if (event != EControlEvent::NO_EVENT) {
    mNoInputStartTime = millis();
  }

  if (mTimeoutEnabled &&
      millis() - mNoInputStartTime > BACK_TO_DEFAULT_SCREEN_TIMEOUT_MS) {
    delete this;
    ret = new CCurrentTempScreen();
  } else {
    ret = this;
  }

  //TODO: check if we need to power off by long press

  return ret;
}

// ***** MAIN SCREEN *****
CScreen* CMainScreen::transition() {
  CScreen* oldThis = this;
  CScreen* ret = CScreen::transition();
  if (ret == oldThis) {
    if (gControls.getEvent() == EControlEvent::POT_SW_PRESS) {
      delete this;
      switch (mSelectedItem) {
        case 0:
          ret = new CCurrentTempScreen();
          break;
        case 1:
          ret = new CTargetTempSelectScreen();
          break;
        case 2:
          //TODO
          break;
        default:
          //TODO
          break;
      }
    }
  }

  return ret;
}

void CMainScreen::draw() {
  switch (gControls.getEvent()) {
    case EControlEvent::POT_STEP_CW:
      incSelection();
      break;
    case EControlEvent::POT_STEP_CCW:
      decSelection();
      break;
    default:
      break;
  }

  const unsigned char* bitmapPtr;
  const char* pgmStr;
  switch (mSelectedItem) {
    case 0:
      bitmapPtr = BITMAP_WATER_TEMP;
      pgmStr = STR_CURRENT_TEMP;
      break;
    case 1:
      bitmapPtr = BITMAP_TARGET_TEMP;
      pgmStr = STR_SET_TARGET_TEMP;
      break;
    case 2:
      bitmapPtr = BITMAP_POWER_MODE;
      pgmStr = STR_SELECT_POWER_MODE;
      break;
    default:
      bitmapPtr = BITMAP_MORE;
      pgmStr = STR_MORE_OPTIONS;
      break;
  }

  char buffer[30];
  strcpy_P(buffer, pgmStr);
  
  gDisp.clearDisplay();

  gDisp.drawBitmap(40, 4, bitmapPtr, 48, 48, WHITE);

  //menu left right arrows
  gDisp.fillTriangle(15, 31, 25, 21, 25, 41, WHITE);
  gDisp.fillTriangle(113, 31, 103, 21, 103, 41, WHITE);
  
  gDisp.setTextColor(WHITE);
  gDisp.setTextSize(1);
  printCentered(buffer, 64, 57);

  //Debug info
  gDisp.setCursor(0,0);
  gDisp.print(mSelectedItem);
  
  gDisp.display();
}

void CMainScreen::incSelection() {
  if (++mSelectedItem > 3) mSelectedItem = 0;
}

void CMainScreen::decSelection() {
  if (mSelectedItem-- == 0) mSelectedItem = 3;
}

// ***** CURRENT TEMP SCREEN *****
CScreen* CCurrentTempScreen::transition() {
  CScreen* oldThis = this;
  CScreen* ret = CScreen::transition();
  if (ret == oldThis) {
    if (gControls.getEvent() == EControlEvent::POT_SW_PRESS) {
      delete this;
      ret = new CMainScreen(0);
    }
  }

  return ret;
}

void CCurrentTempScreen::draw() {
  int8 rounded = (int8)round(gTemperature.getValue());

  gDisp.clearDisplay();
  gDisp.setTextColor(WHITE);
  gDisp.setTextSize(9);
  gDisp.setCursor(0,0);
  gDisp.print(rounded);
  gDisp.display();
}

// ***** NUMBER SELECT SCREEN *****
void CNumberSelectScreen::draw() {
  switch (gControls.getEvent()) {
    case EControlEvent::POT_STEP_CW:
      inc();
      break;
    case EControlEvent::POT_STEP_CCW:
      dec();
      break;
    default:
      break;
  }
  
  gDisp.clearDisplay();
  gDisp.setTextColor(WHITE);
  gDisp.setTextSize(8);
  gDisp.setCursor(0,0);
  gDisp.print(mNumber);
  gDisp.setTextSize(1);
  gDisp.setCursor(20, 56);
  gDisp.print(mText.c_str());
  gDisp.display();
}

void CNumberSelectScreen::inc() {
  if (++mNumber > mMax) mNumber = mMax;
}

void CNumberSelectScreen::dec() {
  if (--mNumber < mMin) mNumber = mMin;
}

// ***** TARGET TEMP SELECT SCREEN *****
CScreen* CTargetTempSelectScreen::transition() {
  CScreen* oldThis = this;
  CScreen* ret = CScreen::transition();
  if (ret == oldThis) {
    if (gControls.getEvent() == EControlEvent::POT_SW_PRESS) {
      gBoilerConfig.setTargetTemp(getNumber());
      delete this;
      ret = new CConfirmScreen(1);
    }
  }

  return ret;
}

// ***** CONFIRM SCREEN *****
CScreen* CConfirmScreen::transition() {
  CScreen* oldThis = this;
  CScreen* ret = CScreen::transition();
  if (ret == oldThis) {
    if (millis() - mStartTime > CONFIRM_SCREEN_TIMEOUT_MS) {
      delete this;
      ret = new CMainScreen(mMainScreenItemInitial);
    }
  }

  return ret;
}

void CConfirmScreen::draw() {
  gDisp.clearDisplay();
  gDisp.drawBitmap(40, 4, BITMAP_CONFIRM, 48, 48, WHITE);
  gDisp.display();
}
