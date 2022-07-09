#define MIN_MOISTURE_PERCENTAGE 80

#define MIN_MOISTURE_VAL 346
#define MAX_MOISTURE_VAL 540

// count of settings despite to your 
// hardware setup / connected pumps
const int SETS_COUNT = 1;

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
};

// your flower config
struct ConfigStruct FLOWER_SET[SETS_COUNT] = { 
    { "Tomato #1", A0, 13, 0, 2000, 4000 } /*,
    { "Tomato #2", A1, 12, 0, 2000, 2000 } */
};


void setup() {
    Serial.begin(9600);
  
    for (int i = 0; i < SETS_COUNT; ++i ) {
        int pumpPin = FLOWER_SET[i].relayInputPin;
        pinMode(pumpPin, OUTPUT);
    }
}

 
void loop() {
    readValues();
    watering();

    delay(750);
}

void watering () {
    for (int i = 0; i < SETS_COUNT; ++i ) {
        wateringSet(i);
    }
}

void wateringSet (int setIndex) {
    String setName = String(FLOWER_SET[setIndex].displayName);

    int pumpPin = FLOWER_SET[setIndex].relayInputPin;
    int moistureValue = FLOWER_SET[setIndex].moistureValue;
    int pumpTimeOffset = FLOWER_SET[setIndex].pumpTimeOffset;
    int pumpDuration = FLOWER_SET[setIndex].pumpDuration;

    // when measured moisture value felt below the
    // "dry" zone -> pump some water
    if (moistureValue < MIN_MOISTURE_PERCENTAGE) {
        float pumpValueOffset = (float(MIN_MOISTURE_PERCENTAGE) / 100) * (float(100 - moistureValue) / 100) * 2000;
        int computedPumpTime = pumpTimeOffset + pumpDuration + pumpValueOffset;

        float timeInSecs = computedPumpTime / 1000;
        
        Serial.println(
            "Watering set «" + setName + "»" +
            ", Moisture: " + String(moistureValue) + " %" +
            ", Duration: " + String(timeInSecs) + " s"
        );
        
        setPumpState(pumpPin, true);
        delay(computedPumpTime);

        Serial.println("Stop set " + setName);
        setPumpState(pumpPin, false);
    }
}

void setPumpState (int pin, bool active) {
    if (active) {
        //digitalWrite(pin, HIGH);
    } else {
        //digitalWrite(pin, LOW);
    }
}

void readValues () {
    for (int i = 0; i < SETS_COUNT; ++i ) {
        int moisturePin = FLOWER_SET[i].moistureSensorPin;
        
        // saves moisture value
        FLOWER_SET[i].moistureValue = getMoisturePercentage(moisturePin);
    }
}

int getMoisturePercentage(int pin) {
    int value = analogRead(pin);
    
    // give the arduino some time to read the analogue value carefully
    delay(1);

    // reset value to bounces
    if (value < MIN_MOISTURE_VAL) {
        value = MIN_MOISTURE_VAL;
    } else if (value > MAX_MOISTURE_VAL) {
        value = MAX_MOISTURE_VAL;
    }

    // smaller value means higher moisture
    // so we have to invert the value mapping
    return map(
        value, 
        MAX_MOISTURE_VAL, 
        MIN_MOISTURE_VAL,
        0, 
        100
    );
}
