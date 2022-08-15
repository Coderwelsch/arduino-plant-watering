#pragma once

#define MIN_MOISTURE_PERCENTAGE 60

#define MAX_WATERING_TIME 12000
#define NEXT_CHECK_DELAY 3600000 // 1 hour

struct ConfigStruct {
    char *displayName;
    int moistureSensorPin;
    int relayInputPin;
    int moistureValue;
    // despite the length of your pump water tubes you should
    // test the optimum delay the water needs to travel to your plantie <3
    int pumpTimeOffset;
    // the main duration of pumping water
    int pumpDuration;
    int minMoistureSensorVal;
    int maxMoistureSensorVal;
};
