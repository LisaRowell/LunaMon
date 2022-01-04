#ifndef NMEALINE_H
#define NMEALINE_H

const unsigned maxNMEALineLength = 82;

class NMEALine {
    private:
        bool overflowed;
        char buffer[maxNMEALineLength + 1];
        unsigned length;
        unsigned position;
        unsigned remaining;
        // This flag is used to indentify the lines which are in the encapsulated encoding scheme
        // used for AIS messages (and possibly others), versus the normal style NMEA 0183 CSV data.
        bool encapsulatedData;

        void stripParity();
        bool checkParity();
        String bufferSubstring(unsigned start, unsigned end);
        void logLine();

    public:
        NMEALine();
        NMEALine(String &inputString);
        void reset();
        void append(const char *srcBuffer, unsigned start, unsigned end);
        bool isEmpty();
        bool isEncapsulatedData();
        bool sanityCheck();
        bool extractChar(char &character);
        bool extractWord(String &word);
};

#endif
