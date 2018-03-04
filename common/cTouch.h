// cTouch.h
#pragma once
//{{{  includes
#include "../../../cLcd.h"
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
  void handleTouch (int touch, int x, int y, int z) {

    BSP_TS_GetState (&mTsState);

    if (touch) {
      // pressed
      if (touch > 1) {
        mHit = eScroll;
        onScroll (x - mLastX, y - mLastY, z);
        }
      else if (mHit == ePressed)
        onMove (x - mLastX, y - mLastY, z);
      else if ((mHit == eReleased) && (z > 50)) {
        // press
        mHitX = x;
        mHitY = y;
        onPress (mHitX, mHitY);
        mHit = ePressed;
        }
      else if (mHit == eProx)
        onProx (x - mLastX, y - mLastY, z);
      else
        mHit = eProx;
      mLastX = x;
      mLastY = y;
      }
    else {
      // release
      if (mHit == ePressed)
        onRelease (mLastX, mLastY);
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

    handleTouch (mTsState.touchDetected, mTsState.touchX[0], mTsState.touchY[0], mTsState.touchWeight[0]);
    }
  //}}}

  //{{{
  void show() {
    for (unsigned int i = 0u; i < mTsState.touchDetected; i++) {
      BSP_LCD_SetTextColor (LCD_COLOR_YELLOW);
      BSP_LCD_FillCircle (mTsState.touchX[i], mTsState.touchY[i],
                          mTsState.touchWeight[i] ? mTsState.touchWeight[i] : 1);
      }
    }
  //}}}

protected:
  virtual void onProx (int x, int y, int z) {}
  virtual void onPress (int x, int y) {}
  virtual void onMove (int x, int y, int z) {}
  virtual void onScroll (int x, int y, int z) {}
  virtual void onRelease (int x, int y) {}

  enum eHit { eReleased, eProx, ePressed, eScroll };
  enum eHit mHit = eReleased;

  int mHitX = 0;
  int mHitY = 0;
  int mLastX = 0;
  int mLastY = 0;

  TS_StateTypeDef mTsState;
  };

//{{{
#ifdef __cplusplus
}
#endif
//}}}
