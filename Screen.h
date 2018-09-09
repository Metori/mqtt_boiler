#ifndef _SCREEN_H
#define _SCREEN_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Global.h"
#include "BoilerConfig.h"

#define CONFIRM_SCREEN_TIMEOUT_MS 1000
#define BACK_TO_DEFAULT_SCREEN_TIMEOUT_MS 10000

extern Adafruit_SSD1306 gDisp;

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
  CNumberSelectScreen(std::string text, int8 initial, int8 min, int8 max)
    : mText(text),
      mNumber(initial),
      mMin(min),
      mMax(max) {
    
  }
  CNumberSelectScreen()
    : CNumberSelectScreen("", 0, 0, 0) {
    
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

  std::string mText;
  int8 mNumber;
  int8 mMin;
  int8 mMax;
};

class CConfirmScreen : public CScreen {
public:
  CConfirmScreen(uint8_t initial)
    : mStartTime(millis()),
      mMainScreenItemInitial(initial) {
    
  }
  virtual ~CConfirmScreen() override {
    
  }

  virtual CScreen* transition() override;
  virtual void draw() override;

private:
  unsigned long mStartTime;
  uint8_t mMainScreenItemInitial;
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
  CTargetTempSelectScreen()
    : CNumberSelectScreen("SELECT TEMP", gBoilerConfig.getTargetTemp(), 30, 90) {
    
  }
  virtual ~CTargetTempSelectScreen() override {
    
  }

  virtual CScreen* transition() override;
};

#endif // _SCREEN_H
