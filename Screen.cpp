#include "Temperature.h"
#include "Screen.h"
#include "Controls.h"
#include "res/Bitmaps.h"
#include "res/Strings.h"

void printCentered(const char* str, uint16_t x, uint16_t y, uint8_t textSize) {
  int16_t w = strlen(str) * 6 * textSize;
  gDisp.setCursor(x - w / 2, y);
  gDisp.setTextSize(textSize);
  gDisp.print(str);
}

void printCentered(const __FlashStringHelper* str, uint16_t x, uint16_t y, uint8_t textSize) {
  int16_t w = strlen_P((const char*)str) * 6 * textSize;
  gDisp.setCursor(x - w / 2, y);
  gDisp.setTextSize(textSize);
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
          ret = new CPowerModeSelectScreen();
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

  gDisp.clearDisplay();

  gDisp.drawBitmap(40, 4, bitmapPtr, 48, 48, WHITE);

  //menu left right arrows
  gDisp.fillTriangle(15, 31, 25, 21, 25, 41, WHITE);
  gDisp.fillTriangle(113, 31, 103, 21, 103, 41, WHITE);
  
  gDisp.setTextColor(WHITE);
  printCentered(FPSTR(pgmStr), 63, 57);

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
  char buffer[2];
  int8 rounded = (int8)round(gTemperature.getValue());
  itoa(rounded, buffer, 10);

  gDisp.clearDisplay();
  gDisp.setTextColor(WHITE);
  printCentered(buffer, 63, 0, 8);
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

  char buffer[2];
  itoa(mNumber, buffer, 10);
  
  gDisp.clearDisplay();
  gDisp.setTextColor(WHITE);
  printCentered(buffer, 63, 0, 8);
  printCentered(mText, 63, 57);
  gDisp.display();
}

void CNumberSelectScreen::inc() {
  if (++mNumber > mMax) mNumber = mMax;
}

void CNumberSelectScreen::dec() {
  if (--mNumber < mMin) mNumber = mMin;
}

// ***** TARGET TEMP SELECT SCREEN *****
CTargetTempSelectScreen::CTargetTempSelectScreen()
  : CNumberSelectScreen(FPSTR(STR_SELECT_TEMP),
                        gBoilerConfig.getTargetTemp(),
                        30,
                        90) {

}

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

// ***** OPTION CHOOSE SCREEN *****
void COptionChooseScreen::draw() {
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
  gDisp.setTextSize(2);

  gDisp.setCursor(0, 25);
  for (int i = 0; i < mTextOptions.size(); i++) {
    if (i == mSelected) gDisp.setTextColor(BLACK, WHITE);
    else gDisp.setTextColor(WHITE);

    gDisp.print(mTextOptions[i].c_str());
    gDisp.print(" ");
  }

  gDisp.display();
}

void COptionChooseScreen::inc() {
  if (++mSelected >= mTextOptions.size()) mSelected = 0;
}

void COptionChooseScreen::dec() {
  if (mSelected-- == 0) mSelected = mTextOptions.size() - 1;
}

// ***** POWER MODE SELECT SCREEN *****
CPowerModeSelectScreen::CPowerModeSelectScreen()
  : COptionChooseScreen({"LO", "ME", "HI"}, (uint8_t)gBoilerConfig.getPowerMode()) {

}

CScreen* CPowerModeSelectScreen::transition() {
  CScreen* oldThis = this;
  CScreen* ret = CScreen::transition();
  if (ret == oldThis) {
    if (gControls.getEvent() == EControlEvent::POT_SW_PRESS) {
      EPowerMode mode = (EPowerMode)getSelected();
      gBoilerConfig.setPowerMode(mode);
      delete this;
      ret = new CConfirmScreen(2);
    }
  }

  return ret;
}

