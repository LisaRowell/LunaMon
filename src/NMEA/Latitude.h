#ifndef LATITUDE_H
#define LATITUDE_H

#include "Coordinate.h"

enum NorthOrSouth {
  NORTH,
  SOUTH
};

class Latitude : public Coordinate {
  private:
    enum NorthOrSouth northOrSouth;

  public:
  
    bool set(const String &string, const String &northOrSouthStr);
    void print();
};

#endif