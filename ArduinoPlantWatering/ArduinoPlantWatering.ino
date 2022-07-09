#define MIN_MOISTURE_PERCENTAGE 80

// despite the length of your pump water tubes you should
// test the optimum delay the water needs to travel to your planty
#define PUMP_TIME_OFFSET 2000
#define PUMP_DURATION 2000

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
};

// your flower config
struct ConfigStruct FLOWERING_SETS[SETS_COUNT] = { 
    { "Tomato #1", A0, 13, 0 } /*, 
    { "Tomato #2", A1, 12, 0 } */
};


void setup() {
    Serial.begin(9600);
  
    for (int i = 0; i < SETS_COUNT; ++i ) {
        int pumpPin = FLOWERING_SETS[i].relayInputPin;
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
    int pumpPin = FLOWERING_SETS[setIndex].relayInputPin;
    int moistureValue = FLOWERING_SETS[setIndex].moistureValue;
    String setName = String(FLOWERING_SETS[setIndex].displayName);

    // when measured moisture value felt below the
    // "dry" zone -> pump some water
    if (moistureValue < MIN_MOISTURE_PERCENTAGE) {
        float pumpValueOffset = float(MIN_MOISTURE_PERCENTAGE) / float(moistureValue);
        int computedPumpTime = PUMP_TIME_OFFSET + PUMP_DURATION * pumpValueOffset;

        float timeInSecs = computedPumpTime / 1000;
        
        Serial.println(
            "Watering set \"" + setName + "\"" + 
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
        digitalWrite(pin, HIGH);
    } else {
        digitalWrite(pin, LOW);
    }
}

void readValues () {
    for (int i = 0; i < SETS_COUNT; ++i ) {
        int moisturePin = FLOWERING_SETS[i].moistureSensorPin;
        
        // saves moisture value
        FLOWERING_SETS[i].moistureValue = getMoisturePercentage(moisturePin);
    }
}

int getMoisturePercentage(int pin) {
    int value = analogRead(pin);
    
    // give the arduino some time to read the analogue value carefully
    delay(1);

    // Serial.println(String(pin) + ": " + String(value));

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
