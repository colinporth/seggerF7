// cToggleBox.h
#pragma once

class cToggleBox : public cApp::cBox {
public:
  //{{{
  cToggleBox (float width, float height, const char* name, bool& value, bool& changed)
      : cBox(name, width, height), mValue(value), mChanged(changed) {
    mChanged = false;
    }
  //}}}
  virtual ~cToggleBox() {}

  bool onProx (cPoint pos) {
    return true;
    }

  bool onPress (cPoint pos, uint8_t z)  {
    mValue = !mValue;
    mThickness = z;
    mChanged = true;
    return true;
    }

  bool onMove (cPoint pos, uint8_t z)  {
    mThickness = z;
    return true;
    }

  void onDraw (cLcd* lcd) {
    mColor = mValue ? LCD_COLOR_YELLOW : LCD_COLOR_LIGHT_GREY;
    mTextColor = LCD_COLOR_BLACK;
    cBox::onDraw (lcd);
    if (mProxed)
      lcd->drawRect (LCD_COLOR_WHITE, mRect, mThickness < 10 ? 1 : mThickness / 10 );
    }

private:
  bool& mChanged;
  bool& mValue;
  uint16_t mThickness = 1;
  };
