#include "config.h";

// overwrite default settings (found in config.h)
// #define MIN_MOISTURE_PERCENTAGE 60...

// count of settings despite to your 
// hardware setup / connected pumps
const int SETS_COUNT = 1;

// your flower config
struct ConfigStruct FLOWER_SET[SETS_COUNT] = { 
    { "Tomato #1", A0, 13, 0, 2000, 6000, 346, 640 }
};


void setupPumpPins (struct ConfigStruct set[], int count) {
  // init pump pin modes
  for (int i = 0; i < count; ++i ) {
    int pumpPin = set[i].relayInputPin;
    pinMode(pumpPin, OUTPUT);

    // just to be sure
    setPumpState(pumpPin, false);
  }
}

int getMoisturePercentage(int pin, int min, int max) {
  int value = analogRead(pin);

  // give the arduino some time to read the analogue value carefully
  delay(1);

  // reset value to bounces
  if (value < min) {
    value = min;
  } else if (value > max) {
    value = max;
  }

  // smaller value means higher moisture
  // so we have to invert the value mapping
  return map(
     value,
     max,
     min,
     0,
     100
   );
}

void watering (ConfigStruct set[], int count) {
  for (int i = 0; i < count; ++i ) {
    wateringSet(set[i]);
  }
}

void wateringSet (ConfigStruct set) {
  String setName = String(set.displayName);

  int pumpPin = set.relayInputPin;
  int moistureValue = set.moistureValue;
  int pumpTimeOffset = set.pumpTimeOffset;
  int pumpDuration = set.pumpDuration;

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

void readMoistureValues (ConfigStruct set[], int count) {
  for (int i = 0; i < count; ++i ) {
    int moisturePin = set[i].moistureSensorPin;
    int min = set[i].minMoistureSensorVal;
    int max = set[i].maxMoistureSensorVal;

    // saves moisture value
    set[i].moistureValue = getMoisturePercentage(moisturePin, min, max);
  }
}


void setup() {
    Serial.begin(9600);

    // init pump pin modes 
    setupPumpPins(FLOWER_SET, SETS_COUNT);
}

 
void loop() {
    readMoistureValues(FLOWER_SET, SETS_COUNT);
    watering(FLOWER_SET, SETS_COUNT);

    Serial.println(":sleeping_accommodation: Sleep for " + String(NEXT_CHECK_DELAY / 3600000) + " hour(s) ...");
    delay(NEXT_CHECK_DELAY);
}
