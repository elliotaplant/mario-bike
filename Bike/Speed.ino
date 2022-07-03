/*
Speed sensor
- Sets up the hall effect sensor
- Establishes a "base" range for the hall effect sensor with a trailing average
- Determines if a given "tick" is touching the magnet 
- Debounces magent touches
- Determines the RPM of the rider
*/

const int HALL_EFFECT_SENSOR_PIN = 16;
const int NUM_SAMPLES = 20;
const int CHANGE_THRESHOLD = 5;
const int ROTATION_DEBOUNCE_MS = 200;
const int RPM_SMOOTHING_CYCLES = 2;
const int SLOWDOWN_THRESHOLD_MS = 3000;
const int MAX_MS_PER_ROTATION = 120000;
const float MS_PER_MINUTE = 60000;
const int MIN_MOVEMENT_THRESHOLD = 1000;

int prevReadings[NUM_SAMPLES];
int readingIndex = 0;
int currentRpm = 0;
int trailingAverageSum = 0;
time_t lastPass = 0;
int prevRotationMs[RPM_SMOOTHING_CYCLES];
int currentMsPerRotationSum = MAX_MS_PER_ROTATION;
int smoothingIndex = 0;

void setupSpeed() {
  for (int i = 0; i < RPM_SMOOTHING_CYCLES; i++) {
    prevRotationMs[i] = MAX_MS_PER_ROTATION / RPM_SMOOTHING_CYCLES;
  }
}


// Calc to determine whether or not the magnet is near the sensor
bool nearMagnet() {
  int hallValue = analogRead(HALL_EFFECT_SENSOR_PIN);

  // Add the new reading to the trailing sum before putting it in the ring buffer
  trailingAverageSum += (hallValue - prevReadings[readingIndex]);
  prevReadings[readingIndex] = hallValue;
  readingIndex = (readingIndex + 1) % NUM_SAMPLES;

  // Find the difference in magnetic field to see if a rotation has happened
  int trailingAverageValue = trailingAverageSum / NUM_SAMPLES;
  int difference = abs(hallValue - trailingAverageValue);
  
  return difference > CHANGE_THRESHOLD;
}

float getRpm() {
  // Get the time since the last rotation
  time_t thisPass = millis();
  time_t msSince = thisPass - lastPass;
  
  if (nearMagnet()) {
    if (msSince > ROTATION_DEBOUNCE_MS) {
      lastPass = thisPass;
      currentMsPerRotationSum += (msSince - prevRotationMs[smoothingIndex]);
      prevRotationMs[smoothingIndex] = msSince;
      smoothingIndex = (smoothingIndex + 1) % RPM_SMOOTHING_CYCLES;
    }
  } else if (
      currentMsPerRotationSum != MAX_MS_PER_ROTATION && 
      msSince > ((2 * currentMsPerRotationSum) / RPM_SMOOTHING_CYCLES)
    ) {
    currentMsPerRotationSum *= 1.5;
  }

  float msPerRotation = currentMsPerRotationSum / RPM_SMOOTHING_CYCLES;
  return MS_PER_MINUTE / msPerRotation;
}

// Super simple calc to determine if the time since the last magnet has 
bool moving() {
  time_t thisPass = millis();
  if (nearMagnet()) {
    lastPass = thisPass;
  }
  return thisPass - lastPass < MIN_MOVEMENT_THRESHOLD;
}
