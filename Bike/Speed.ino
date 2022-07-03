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
const int RPM_SMOOTHING_CYCLES = 3;
const int SLOWDOWN_THRESHOLD_MS = 3000;

int prevReadings[NUM_SAMPLES];
int readingIndex = 0;
int currentRpm = 0;
int trailingAverageSum = 0;
time_t lastPass = 0;
int prevRotationMs[RPM_SMOOTHING_CYCLES];
int currentMsPerRotationSum = 0;
int smoothingIndex = 0;

int getSpeed() {
  int hallValue = analogRead(HALL_EFFECT_SENSOR_PIN);

  // Add the new reading to the trailing sum before putting it in the ring buffer
  trailingAverageSum += (hallValue - prevReadings[readingIndex]);
  prevReadings[readingIndex] = hallValue;
  readingIndex = (readingIndex + 1) % NUM_SAMPLES;

  // Find the difference in magnetic field to see if a rotation has happened
  int trailingAverageValue = trailingAverageSum / NUM_SAMPLES;
  int difference = abs(hallValue - trailingAverageValue);

  // Get the time since the last rotation
  time_t thisPass = millis();
  time_t msSince = thisPass - lastPass;
  
  if (difference > CHANGE_THRESHOLD) {
    Serial.println("MAGNETRON");
    time_t msSince = thisPass - lastPass;
    
    if (msSince > ROTATION_DEBOUNCE_MS) {
      lastPass = thisPass;
      currentMsPerRotationSum += (msSince - prevRotationMs[smoothingIndex]);
      prevRotationMs[smoothingIndex] = msSince;
      smoothingIndex = (smoothingIndex + 1) % RPM_SMOOTHING_CYCLES;
    }
  } else if (msSince > SLOWDOWN_THRESHOLD_MS) {
    currentMsPerRotationSum += (msSince - prevRotationMs[smoothingIndex]);
    prevRotationMs[smoothingIndex] = msSince;
    smoothingIndex = (smoothingIndex + 1) % RPM_SMOOTHING_CYCLES;
  }

  return currentMsPerRotationSum / RPM_SMOOTHING_CYCLES;
}
