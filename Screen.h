#ifndef _SCREEN_H
#define _SCREEN_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BoilerConfig.h"

#define CONFIRM_SCREEN_TIMEOUT_MS 1000
#define BACK_TO_DEFAULT_SCREEN_TIMEOUT_MS 10000

extern Adafruit_SSD1306 gDisp;

void printCentered(const char* str, uint16_t x, uint16_t y, uint8_t textSize = 1);
void printCentered(const __FlashStringHelper* str, uint16_t x, uint16_t y, uint8_t textSize = 1);

typedef enum {
  ERR_UNKNOWN,
  ERR_TEMP_SENSOR,
  ERR_TEMP_TOO_LOW,
  ERR_TEMP_TOO_HIGH,
  ERR_EEPROM_FAIL
} EError;

class CScreen {
public:
  CScreen(bool timeoutEnabled = true)
    : mTimeoutEnabled(timeoutEnabled) {
    
  }
  virtual ~CScreen() {
    
  }

  virtual CScreen* transition();
  virtual void draw() = 0;

private:
  bool mTimeoutEnabled;
  unsigned long mNoInputStartTime = millis();
};

class CNumberSelectScreen : public CScreen {
public:
  CNumberSelectScreen(const __FlashStringHelper* pgmText, int8 initial, int8 min, int8 max)
    : mText(pgmText),
      mNumber(initial),
      mMin(min),
      mMax(max) {
    
  }
  virtual ~CNumberSelectScreen() override {
    
  }

  virtual CScreen* transition() = 0;
  virtual void draw() override;

  int8 getNumber() {
    return mNumber;
  }

private:
  void inc();
  void dec();

  const __FlashStringHelper* mText;
  int8 mNumber;
  int8 mMin;
  int8 mMax;
};

class COptionChooseScreen : public CScreen {
public:
  COptionChooseScreen(std::vector<std::string> textOptions, uint8_t initial)
    : mTextOptions(textOptions),
      mSelected(initial) {

  }
  virtual ~COptionChooseScreen() override {

  }

  virtual CScreen* transition() = 0;
  virtual void draw() override;

  uint8_t getSelected() {
    return mSelected;
  }

private:
  void inc();
  void dec();

  std::vector<std::string> mTextOptions;
  uint8_t mSelected;
};

class CErrorScreen : public CScreen {
public:
  CErrorScreen(EError error)
    : CScreen(false),
      mError(error) {

  }
  virtual ~CErrorScreen() override {

  }

  virtual void draw() override;

private:
  EError mError;
};

class CConfirmScreen : public CScreen {
public:
  CConfirmScreen(CScreen* nextScreen, unsigned long timeout = CONFIRM_SCREEN_TIMEOUT_MS)
    : CScreen(false),
      mStartTime(millis()),
      mNextScreen(nextScreen),
      mTimeout(timeout) {
    
  }
  virtual ~CConfirmScreen() override {
    
  }

  virtual CScreen* transition() override;
  virtual void draw() override;

private:
  unsigned long mStartTime;
  CScreen* mNextScreen;
  unsigned long mTimeout;
};

class CMessageScreen : public CConfirmScreen {
public:
  CMessageScreen(const __FlashStringHelper* caption,
                 const __FlashStringHelper* msg,
                 CScreen* nextScreen)
    : CConfirmScreen(nextScreen, 0),
      mCaption(caption),
      mMsg(msg) {

  }
  virtual ~CMessageScreen() override {

  }

  virtual void draw() override;

private:
  const __FlashStringHelper* mCaption;
  const __FlashStringHelper* mMsg;
};

class CMainScreen : public CScreen {
public:
  CMainScreen(uint8_t initial)
    : mSelectedItem(initial) {
    
  }
  CMainScreen()
    : CMainScreen(0) {
    
  }
  virtual ~CMainScreen() override {
    
  }

  virtual CScreen* transition() override;
  virtual void draw() override;

private:
  void incSelection();
  void decSelection();

  uint8_t mSelectedItem = 0;
};

class CCurrentTempScreen : public CScreen {
public:
  CCurrentTempScreen()
    : CScreen(false) {
      Serial.println("CurrentTempScreen created");
  }
  virtual ~CCurrentTempScreen() override {
    
  }

  virtual CScreen* transition() override;
  virtual void draw() override;
};

class CTargetTempSelectScreen : public CNumberSelectScreen {
public:
  CTargetTempSelectScreen();
  virtual ~CTargetTempSelectScreen() override {
    
  }

  virtual CScreen* transition() override;
};

class CPowerModeSelectScreen : public COptionChooseScreen {
public:
  CPowerModeSelectScreen();
  virtual ~CPowerModeSelectScreen() override {

  }

  virtual CScreen* transition() override;
};

#endif // _SCREEN_H
