// cToggleBox.h
#pragma once

class cToggleBox : public cApp::cBox {
public:
  //{{{
  cToggleBox (float width, float height, const char* title, bool& value, bool& changed)
      : cBox("offset", width, height), mTitle(title), mValue(value), mChanged(changed) {
    mChanged = false;
    }
  //}}}
  virtual ~cToggleBox() {}

  //{{{
  bool onDown (bool right, cPoint pos)  {
    mValue = !mValue;
    mChanged = true;
    return true;
    }
  //}}}

  void onDraw (cLcd* lcd) {
    lcd->FillRect (mValue ? LCD_COLOR_GREEN : LCD_COLOR_GREY, mRect.left, mRect.top, mRect.getWidth(), mRect.getHeight());
    lcd->DisplayStringAt (LCD_COLOR_BLACK, mRect.left, mRect.top, mTitle, LEFT_MODE);
    }

private:
  const char* mTitle;
  bool& mChanged;
  bool& mValue;
  };
