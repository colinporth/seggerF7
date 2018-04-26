#pragma once
#include "system.h"

//{{{
#ifdef __cplusplus
  extern "C" {
#endif
//}}}

//{{{
class cPoint {
public:
  //{{{
  cPoint()  {
    x = 0;
    y = 0;
    }
  //}}}
  //{{{
  cPoint (uint16_t x, uint16_t y) {
    this->x = x;
    this->y = y;
    }
  //}}}

  //{{{
  cPoint operator - (const cPoint& point) const {
    return cPoint (x - point.x, y - point.y);
    }
  //}}}
  //{{{
  cPoint operator + (const cPoint& point) const {
    return cPoint (x + point.x, y + point.y);
    }
  //}}}
  //{{{
  cPoint operator * (const int16_t s) const {
    return cPoint (x * s, y * s);
    }
  //}}}
  //{{{
  cPoint operator * (const float s) const {
    return cPoint (int16_t(x * s), int16_t(y * s));
    }
  //}}}
  //{{{
  cPoint operator / (const int s) const {
    return cPoint (x / s, y / s);
    }
  //}}}
  //{{{
  cPoint operator / (const int16_t s) const {
    return cPoint (x / s, y / s);
    }
  //}}}
  //{{{
  cPoint operator / (const float s) const {
    return cPoint (int16_t(x / s), int16_t(y / s));
    }
  //}}}

  //{{{
  const cPoint& operator += (const cPoint& point)  {
    x += point.x;
    y += point.y;
    return *this;
    }
  //}}}
  //{{{
  const cPoint& operator -= (const cPoint& point)  {
    x -= point.x;
    y -= point.y;
    return *this;
    }
  //}}}

  //{{{
  bool inside (const cPoint& pos) const {
  // return pos inside rect formed by us as size
    return pos.x >= 0 && pos.x < x && pos.y >= 0 && pos.y < y;
    }
  //}}}
  //{{{
  float magnitude() const {
  // return magnitude of point as vector
    return sqrt (float(x*x) + float(y*y));
    }
  //}}}

  int16_t x;
  int16_t y;
  };
//}}}
//{{{
class cRect {
public:
  //{{{
  cRect()  {
    left = 0;
    bottom = 0;
    right = 0;
    bottom = 0;
    }
  //}}}
  //{{{
  cRect (const cPoint& size)  {
  left = 0;
  top = 0;
  right = size.x;
  bottom = size.y;
  }
  //}}}
  //{{{
  cRect (const cPoint& topLeft, const cPoint& bottomRight)  {
   left = topLeft.x;
   top = topLeft.y;
   right = bottomRight.x;
   bottom = bottomRight.y;
   }
  //}}}
  //{{{
  cRect (uint16_t l, uint16_t t, uint16_t r, uint16_t b) {
    left = l;
    top = t;
    right = r;
    bottom = b;
    }
  //}}}

  //{{{
  cRect operator + (const cPoint& point) const {
    return cRect (left + point.x, top + point.y, right + point.x, bottom + point.y);
    }
  //}}}

  int getWidth() const { return right - left; }
  int getHeight() const { return bottom - top; }
  int getWidthInt() const { return int(right - left); }
  int getHeightInt() const { return int(bottom - top); }

  cPoint getTL() const { return cPoint(left, top); }
  cPoint getTL (int offset) const { return cPoint(left+offset, top+offset); }
  cPoint getTR() const { return cPoint(right, top); }
  cPoint getBL() const { return cPoint(left, bottom); }
  cPoint getBR() const { return cPoint(right, bottom); }

  cPoint getSize() const { return cPoint(right-left, bottom-top); }
  cPoint getCentre() const { return cPoint(getCentreX(), getCentreY()); }
  int getCentreX() const { return (left + right)/2.f; }
  int getCentreY() const { return (top + bottom)/2.f; }
  //{{{
  bool inside (const cPoint& pos) const {
  // return pos inside rect
    return (pos.x >= left) && (pos.x < right) && (pos.y >= top) && (pos.y < bottom);
    }
  //}}}

  int16_t left;
  int16_t right;
  int16_t top;
  int16_t bottom;
  };
//}}}

//{{{
#ifdef __cplusplus
}
#endif
//}}}
