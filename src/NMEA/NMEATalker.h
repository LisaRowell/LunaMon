#ifndef NMEA_TALKER_H
#define NMEA_TALKER_H

class NMEATalker {
    private:
        enum Talker {
        NMEA_TALKER_UNKNOWN,
        NMEA_TALKER_GPS
    };

    enum Talker talker;

    public:
        NMEATalker(String &talkerCode);
        const char *name() const;
};

#endif
