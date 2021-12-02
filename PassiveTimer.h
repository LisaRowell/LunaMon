#ifndef PASSIVE_TIMER_H
#define PASSIVE_TIMER_H

class PassiveTimer {
  private:
    const uint32_t halfMilliTimerRange = 0x80000000;
    uint32_t endTime;

  public:
    void setMilliSeconds(uint32_t milliSeconds);
    void setSeconds(uint32_t seconds);
    bool expired();
};

#endif