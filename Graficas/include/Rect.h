#pragma once
#include "stdHeaders.h"


class Rect {
public:
  Rect(int32 inIniX, int32 inIniY, int32 inEndX, int32 inEndY) : IniX(inIniX), IniY(inIniY), EndX(inEndX), EndY(inEndY) {};
  ~Rect() = default;

  int32 IniX;
  int32 IniY;
  int32 EndX;
  int32 EndY;
  const static Rect Zero;

};