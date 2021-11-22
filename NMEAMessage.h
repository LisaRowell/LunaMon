#ifndef NMEAMESSAGE_H
#define NMEAMESSAGE_H

#include "NMEATalker.h"
#include "NMEALine.h"


class NMEAMessage {
  protected:
    enum NMEATalker talker;

  public:
    void setTalker(enum NMEATalker talker);
    virtual void print();
};

extern NMEAMessage *parseNMEAMessage(NMEALine &nmeaLine);

#endif