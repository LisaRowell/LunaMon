#include <Arduino.h>

#include "Longitude.h"

bool Longitude::set(const String &string, const String &eastOrWestStr) {
    if (string.length() < 5) {
        return false;
    }

    if (!setDegrees(string, 0, 3, 180)) {
        return false;
    }

    if (!setMinutes(string, 3)) {
        return false;
    }

    if (eastOrWestStr == "E") {
        eastOrWest = EAST;
    } else if (eastOrWestStr == "W") {
        eastOrWest = WEST;
    } else {
        return false;
    }

    return true;
}

void Longitude::print() {
    Coordinate::print();

    switch (eastOrWest) {
        case EAST:
            Serial.print("E");
            break;

        case WEST:
            Serial.print("W");
    }
}

void Longitude::publish(DataModelLeaf &leaf) {
    switch (eastOrWest) {
        case EAST:
            Coordinate::publish(leaf, true);
            break;

        case WEST:
            Coordinate::publish(leaf, false);
    }
}