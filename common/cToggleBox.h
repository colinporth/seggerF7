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

  bool onProx (cPoint pos) { 
    return false; 
    }

  //{{{
  bool onDown (cPoint pos)  {
    mValue = !mValue;
    mChanged = true;
    return true;
    }
  //}}}

  void onDraw (cLcd* lcd) {
    lcd->fillRectCpu (mValue ? LCD_COLOR_YELLOW : LCD_COLOR_LIGHT_GREY, mRect);
    if (mPick)
      lcd->drawRect (LCD_COLOR_WHITE, mRect);
    lcd->displayStringAt (LCD_COLOR_BLACK, mRect.getTL(), mTitle, cLcd::eTextLeft);
    }

private:
  const char* mTitle;
  bool& mChanged;
  bool& mValue;
  };
