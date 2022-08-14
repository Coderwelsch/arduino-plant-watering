#define MIN_MOISTURE_PERCENTAGE 60

#define MAX_WATERING_TIME 12000
#define NEXT_CHECK_DELAY 3600000

#define MIN_MOISTURE_VAL 346
#define MAX_MOISTURE_VAL 640

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
    { "Tomato #1", A0, 13, 0, 2000, 6000 } /*,
    { "Tomato #2", A1, 12, 0, 2000, 2000 } */
};


void setup() {
    Serial.begin(9600);

    // init pump pin modes 
    for (int i = 0; i < SETS_COUNT; ++i ) {
        int pumpPin = FLOWER_SET[i].relayInputPin;
        pinMode(pumpPin, OUTPUT);

        // just to be sure
        // setPumpState(pumpPin, false);
    }
}

 
void loop() {
    readValues();
    watering();

    Serial.println(":sleeping_accommodation: Sleep for " + String(NEXT_CHECK_DELAY / 3600000) + " hour(s) ...");
    delay(NEXT_CHECK_DELAY);
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
        
        int computedPumpTime = min(
            MAX_WATERING_TIME, 
            pumpTimeOffset + pumpDuration + pumpValueOffset
        );
        

        float timeInSecs = computedPumpTime / 1000;
        
        Serial.println(
            ":potable_water: Watering set «" + setName + "»" +
            ", Moisture: " + String(moistureValue) + " %" +
            ", Duration: " + String(timeInSecs) + " s"
        );
        
        setPumpState(pumpPin, true);
        delay(computedPumpTime);

        Serial.println(":ok: Watered set «" + setName + "»");
    } else {
        Serial.println(":ok: Do nothing for set " + setName + ", Moisture: " + moistureValue);
    }

    // deactive pump pin every time
    setPumpState(pumpPin, false);
}

void setPumpState (int pin, bool active) {
    if (active) {
        // Serial.println("Activate Pump #" + String(pin));
        digitalWrite(pin, LOW);
    } else {
        // Serial.println("Deactivate Pump #" + String(pin));
        digitalWrite(pin, HIGH);
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
