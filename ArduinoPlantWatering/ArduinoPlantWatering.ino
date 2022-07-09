#define MIN_MOISTURE_PERCENTAGE 80
#define PUMP_TIME_OFFSET 2000
#define PUMP_DURATION 2000

#define MIN_MOISTURE_VAL 346
#define MAX_MOISTURE_VAL 540

int SETS_COUNT = 2;

int FLOWERING_SETS [2][3] = { 
    // Moisture Sensor Data Pin | Relays Input Pin | (Initial) Moisture Value
    {  A0,                        13,                0                      }, 
    {  A1,                        12,                0                      }, 
};


void setup() {
    Serial.begin(9600);
  
    for (int i = 0; i < SETS_COUNT; ++i ) {
        int pumpPin = FLOWERING_SETS[i][1];
        pinMode(pumpPin, OUTPUT);
    }
}

 
void loop() {
    readValues();
    watering();

    //Serial.println(String(FLOWERING_SETS[0][2]) + " %");

    delay(750);
    
    // digitalWrite(PIN_PUMP_1, HIGH);  // Schaltet einpum
    // delay(1000);

    // digitalWrite(PIN_PUMP_1, LOW);   // Schaltet aus
    // delay(1000); 
}

void watering () {
    for (int i = 0; i < SETS_COUNT; ++i ) {
        int moistureValue = FLOWERING_SETS[i][2];
        int pumpPin = FLOWERING_SETS[i][2];
    
        if (moistureValue < MIN_MOISTURE_PERCENTAGE) {
            float pumpValueOffset = float(MIN_MOISTURE_PERCENTAGE) / float(moistureValue);
            int computedPumpTime = PUMP_TIME_OFFSET + PUMP_DURATION * pumpValueOffset;
            
            Serial.println(
                "Watering set #" + String(i) + 
                ", Moisture: " + String(moistureValue) + " %" +
                ", Duration: " + String(computedPumpTime) + " s"
            );
            
            setPumpState(pumpPin, true);
            delay(computedPumpTime);

            Serial.println("Stop set #" + String(i));
            setPumpState(pumpPin, false);
        }
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
        int moisturePin = FLOWERING_SETS[i][0];
        
        // saves moisture value
        FLOWERING_SETS[i][2] = getMoisturePercentage(moisturePin);
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
