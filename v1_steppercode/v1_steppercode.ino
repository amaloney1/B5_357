#define STEP_PIN 2
#define DIR_PIN 3

const long TOTAL_STEPS = 100000;

// Speed settings (microseconds per HALF pulse)
int FORWARD_MIN_DELAY = 60;
int BACKWARD_MIN_DELAY = 240;

// Acceleration settings
const int START_DELAY = 600;
const int RAMP_STEPS = 5000;

bool stopRequested = false;

void stepPulse(int delay_us) {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(delay_us);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(delay_us);
}

void checkSerialStop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 's') {
      stopRequested = true;
      Serial.println("STOP requested");
    }
  }
}

void moveMotor(long steps, int minDelay, bool direction) {
  digitalWrite(DIR_PIN, direction);
  stopRequested = false;

  for (long i = 0; i < steps; i++) {
    checkSerialStop();
    if (stopRequested) return;

    int currentDelay;

    // Acceleration
    if (i < RAMP_STEPS) {
      currentDelay = START_DELAY - ((START_DELAY - minDelay) * i / RAMP_STEPS);
    }
    // Deceleration
    else if (i > steps - RAMP_STEPS) {
      long decelStep = steps - i;
      currentDelay = START_DELAY - ((START_DELAY - minDelay) * decelStep / RAMP_STEPS);
    }
    // Constant speed
    else {
      currentDelay = minDelay;
    }

    stepPulse(currentDelay);
  }
}

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("Stepper Ready");
  Serial.println("Commands:");
  Serial.println("f = forward (40 cm in 12 s)");
  Serial.println("b = backward (48 s)");
  Serial.println("t = full cycle");
  Serial.println("s = stop");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'f') {
      Serial.println("Moving forward...");
      moveMotor(TOTAL_STEPS, FORWARD_MIN_DELAY, HIGH);
      Serial.println("Done.");
    }

    else if (cmd == 'b') {
      Serial.println("Moving backward...");
      moveMotor(TOTAL_STEPS, BACKWARD_MIN_DELAY, LOW);
      Serial.println("Done.");
    }

    else if (cmd == 't') {
      Serial.println("Running full cycle...");

      moveMotor(TOTAL_STEPS, FORWARD_MIN_DELAY, HIGH);
      if (!stopRequested) delay(1000);

      moveMotor(TOTAL_STEPS, BACKWARD_MIN_DELAY, LOW);

      Serial.println("Cycle complete.");
    }

    else if (cmd == 's') {
      stopRequested = true;
      Serial.println("STOP command received.");
    }
  }
}