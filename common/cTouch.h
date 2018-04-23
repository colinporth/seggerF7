// cTouch.h
#pragma once
//{{{  includes
#include "cLcd.h"
#include "stm32746g_discovery_ts.h"
//}}}
//{{{
#ifdef __cplusplus
  extern "C" {
#endif
//}}}

class cTouch {
public:
  //{{{
  cTouch (int x, int y) {
    BSP_TS_Init (x, y);
    }
  //}}}
  //{{{
  void handleTouch (int touch, cPoint pos, int z) {

    BSP_TS_GetState (&mTsState);

    if (touch) {
      // pressed
      if (touch > 1) {
        mHit = eScroll;
        onScroll (pos - mLastPos, z);
        }
      else if (mHit == ePressed)
        onMove (pos - mLastPos, z);
      else if ((mHit == eReleased) && (z > 20)) {
        // press
        mHitPos = pos;
        onPress (mHitPos);
        mHit = ePressed;
        }
      else if (mHit == eProx)
        onProx (pos - mLastPos, z);
      else
        mHit = eProx;
      mLastPos = pos;
      }
    else {
      // release
      if (mHit == ePressed)
        onRelease (mLastPos);
      mHit = eReleased;
      }
    }
  //}}}
  //{{{
  void pollTouch() {

    BSP_TS_GetState (&mTsState);
    //lcd->debug (LCD_COLOR_YELLOW, "%d x:%d y:%d w:%d e:%d a:%d g:%d",
    //       mTsState.touchDetected, mTsState.touchX[0],mTsState.touchY[0], mTsState.touchWeight[0],
    //       mTsState.touchEventId[0], mTsState.touchArea[0], mTsState.gestureId);

    handleTouch (mTsState.touchDetected, cPoint (mTsState.touchX[0], mTsState.touchY[0]), mTsState.touchWeight[0]);
    }
  //}}}

  //{{{
  void show() {
    for (unsigned int i = 0u; i < mTsState.touchDetected; i++) {
      //BSP_LCD_SetTextColor (LCD_COLOR_YELLOW);
      //BSP_LCD_FillCircle (mTsState.touchX[i], mTsState.touchY[i],
      //                    mTsState.touchWeight[i] ? mTsState.touchWeight[i] : 1);
      }
    }
  //}}}

protected:
  virtual void onProx (cPoint pos, uint8_t z) {}
  virtual void onPress (cPoint pos) {}
  virtual void onMove (cPoint pos, uint8_t z) {}
  virtual void onScroll (cPoint pos, uint8_t z) {}
  virtual void onRelease (cPoint pos) {}
  virtual void onKey (uint8_t ch, bool release) {}

  enum eHit { eReleased, eProx, ePressed, eScroll };
  eHit mHit = eReleased;

  cPoint mHitPos;
  cPoint mLastPos;

  TS_StateTypeDef mTsState;
  };

//{{{
#ifdef __cplusplus
}
#endif
//}}}
