// utils.h
#include "utils.h"

std::string dec (int num, int width) {

  std::string str;
  while ((width > 0) || num) {
    str = char((num % 10) + 0x30) + str;
    num /= 10;
    width--;
    }
  return str;
  }
