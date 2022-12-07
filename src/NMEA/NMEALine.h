#ifndef NMEA_LINE_H
#define NMEA_LINE_H

#include <etl/string.h>
#include <etl/string_view.h>

#include <stddef.h>

const size_t maxNMEALineLength = 82;

class NMEALine {
    private:
        etl::string<maxNMEALineLength> line;
        etl::string_view remaining;
        // This flag is used to indentify the lines which are in the encapsulated encoding scheme
        // used for AIS messages (and possibly others), versus the normal style NMEA 0183 CSV data.
        bool encapsulatedData;

        void stripParity();
        bool checkParity();

    public:
        NMEALine();
        void reset();
        void append(const char *srcBuffer, size_t start, size_t end);
        bool isEmpty();
        bool isEncapsulatedData();
        bool sanityCheck();
        bool extractChar(char &character);
        bool getWord(etl::string_view &word);
        bool atEndOfLine();
        void logLine();
};

#endif
