#ifndef REALTIME_CLOCK_H
#define REALTIME_CLOCK_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

class RealtimeClock {
private:
    const char* ntpServer;
    long gmtOffsetSec;
    int daylightOffsetSec;

    unsigned long lastUpdateMillis = 0;
    bool timeReady = false;

public:
    RealtimeClock(
        const char* server = "pool.ntp.org",
        long gmtOffset = 7 * 3600,
        int daylightOffset = 0
    );

    void begin();
    bool update();

    bool isReady();

    String getTime();
    String getDate();
    String getDateTime();

    int getHour();
    int getMinute();
    int getSecond();
};

#endif