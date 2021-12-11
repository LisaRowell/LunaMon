#ifndef COORDINATE_H
#define COORDINATE_H

class Coordinate {
    private:
        uint32_t tenthsSecondScaler();
        uint16_t tenthsOfSeconds();

    protected:
        uint8_t degrees;
        uint8_t minutes;
        uint8_t minutePrecision;
        uint32_t minuteFraction;

        bool setDegrees(const String &string, unsigned startDegrees, unsigned endDegrees, uint8_t maxDegrees);
        bool setMinutes(const String &string, unsigned startMinutes);
        void print();
};

#endif