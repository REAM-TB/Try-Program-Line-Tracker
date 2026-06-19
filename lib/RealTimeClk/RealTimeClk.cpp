#include "RealTimeClk.h"

RealtimeClock::RealtimeClock(const char* server, long gmtOffset, int daylightOffset) {
    ntpServer = server;
    gmtOffsetSec = gmtOffset;
    daylightOffsetSec = daylightOffset;
}

void RealtimeClock::begin() {
    configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);
}

bool RealtimeClock::update() {
    if (millis() - lastUpdateMillis >= 1000) {
        lastUpdateMillis = millis();

        struct tm timeinfo;
        if (getLocalTime(&timeinfo, 10)) {
            timeReady = true;
            return true;
        }
    }

    return false;
}

bool RealtimeClock::isReady() {
    return timeReady;
}

String RealtimeClock::getTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 10)) return "--:--:--";

    char buffer[10];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
    return String(buffer);
}

String RealtimeClock::getDate() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 10)) return "--/--/----";

    char buffer[12];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", &timeinfo);
    return String(buffer);
}

String RealtimeClock::getDateTime() {
    return getDate() + " " + getTime();
}

int RealtimeClock::getHour() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 10)) return -1;
    return timeinfo.tm_hour;
}

int RealtimeClock::getMinute() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 10)) return -1;
    return timeinfo.tm_min;
}

int RealtimeClock::getSecond() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 10)) return -1;
    return timeinfo.tm_sec;
}