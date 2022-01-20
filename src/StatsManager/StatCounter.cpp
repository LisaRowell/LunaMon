#include "StatCounter.h"

#include "Util/TimeConstants.h"
#include "Util/Logger.h"

#include <Arduino.h>

StatCounter::StatCounter() : count(0), lastIntervalCount(0) {
}

StatCounter StatCounter::operator ++ (int) {
    count++;

    return *this;
}

void StatCounter::update(DataModelLeaf &countLeaf, DataModelLeaf &rateLeaf, uint32_t msElapsed) {
    countLeaf << count;

    uint32_t countInInterval;
    if (count < lastIntervalCount) {
        // Rollover occurred....wow that must have tacken forever
        countInInterval = 0xffffffff - lastIntervalCount + 1 + count;
    } else {
        countInInterval = count - lastIntervalCount;
    }

    const uint32_t eventsPerSecond = (countInInterval * msInSecond) / msElapsed;
    rateLeaf << eventsPerSecond;

    logger << logDebugStatsManager << "Harvested counter: " << count << " " << eventsPerSecond
           << "/sec" << eol;

    lastIntervalCount = count;
}
