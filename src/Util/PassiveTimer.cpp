#include "PassiveTimer.h"
#include "TimeConstants.h"
#include "Error.h"

#include <Arduino.h>

void PassiveTimer::setMilliSeconds(uint32_t milliSeconds) {
    // The algorithm we use for detection of a timer end is designed for system uptimes greater than
    // the 50 days that it takes to wrap around a 32 bit millisecond counter and is based on seeing
    // if the end time is first half of the 2^32 period and the current time is in the later.
    // Because of this, we can't accept a timer period of greater than 25 days, give or take. This
    // should not present a hardship for the average coder.
    if (milliSeconds >= halfMilliTimerRange) {
        fatalError("Attempting to set a timer greater than 25 days.");
    }

    // This may wrap, but that's by design.
    endTime = millis() + milliSeconds;
}

void PassiveTimer::setSeconds(uint32_t seconds) {
    setMilliSeconds(seconds * msInSecond);
}

void PassiveTimer::setNow() {
    setMilliSeconds(0);
}

void PassiveTimer::advanceMilliSeconds(uint32_t milliSeconds) {
    // The algorithm we use for detection of a timer end is designed for system uptimes greater than
    // the 50 days that it takes to wrap around a 32 bit millisecond counter and is based on seeing
    // if the end time is first half of the 2^32 period and the current time is in the later.
    // Because of this, we can't accept a timer period of greater than 25 days, give or take. This
    // should not present a hardship for the average coder.
    if (milliSeconds >= halfMilliTimerRange) {
        fatalError("Attempting to advance a timer greater than 25 days.");
    }

    // This may wrap, but that's by design.
    endTime += milliSeconds;
}

void PassiveTimer::advanceSeconds(uint32_t seconds) {
    advanceMilliSeconds(seconds * msInSecond);
}

bool PassiveTimer::expired() {
    const uint32_t time = millis();

    // We deal with timers wrapping around the 32 bit millisecond timer by looking for the end time
    // being less than the mid point while the current time is after it. Since we limit the timer
    // interval to 2^31, this means that the millis counter needs to wrap around for the timer to
    // expire.
    if (time >= halfMilliTimerRange && endTime < halfMilliTimerRange) {
        return false;
    } else {
        return time >= endTime;
    }
}

uint32_t PassiveTimer::timeInMilliSeconds() {
    return endTime;
}

uint32_t PassiveTimer::elapsedTime() {
    uint32_t now = millis();

    if (now >= endTime) {
        return now - endTime;
    } else {
        return 0xffffffff - endTime + 1 + now;
    }
}
