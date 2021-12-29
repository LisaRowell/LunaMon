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

        void stripParity();
        bool checkParity();
        String bufferSubstring(unsigned start, unsigned end);

    public:
        NMEALine();
        NMEALine(String &inputString);
        void reset();
        void append(const char *srcBuffer, unsigned start, unsigned end);
        bool isEmpty();
        bool sanityCheck();
        bool extractChar(char &character);
        bool extractWord(String &word);
};

#endif
