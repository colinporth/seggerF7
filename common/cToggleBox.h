// cToggleBox.h
#pragma once

class cToggleBox : public cApp::cBox {
public:
  //{{{
  cToggleBox (float width, float height, const char* title, bool& value, bool& changed)
      : cBox("toggle", width, height), mTitle(title), mValue(value), mChanged(changed) {
    mChanged = false;
    }
  //}}}
  virtual ~cToggleBox() {}

  //{{{
  bool onDown (cPoint pos)  {
    mValue = !mValue;
    mChanged = true;
    return true;
    }
  //}}}

  void onDraw (cLcd* lcd) {
    lcd->fillRectCpu (mValue ? LCD_COLOR_GREEN : LCD_COLOR_LIGHT_GREY, mRect);
    lcd->displayStringAt (LCD_COLOR_BLACK, mRect.getTL(), mTitle, cLcd::LEFT_MODE);
    }

private:
  const char* mTitle;
  bool& mChanged;
  bool& mValue;
  };
