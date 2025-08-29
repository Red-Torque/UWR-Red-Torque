// === MOTOR PINS ===
#define DRIVE_IN1 26
#define DRIVE_IN2 27
#define DRIVE_ENA 25  // PWM

#define STEER_IN3 14
#define STEER_IN4 12
#define STEER_ENB 13  // PWM

// === ULTRASONIC PINS ===
#define TRIG_FL 22
#define ECHO_FL 23
#define TRIG_FC 33
#define ECHO_FC 32
#define TRIG_FR 17
#define ECHO_FR 5
#define TRIG_BR 19
#define ECHO_BR 18
#define TRIG_BC 16
#define ECHO_BC 4
#define TRIG_BL 2
#define ECHO_BL 15

// === PARAMETERS ===
#define DRIVE_SPEED 100   // ~70% duty (0-255)
#define STEER_SPEED 180
#define SAFE_DIST 6       // stop distance in cm
#define RETRY_LIMIT 3

// --- MOTOR CONTROL ---
void driveForward(int speed) {
  digitalWrite(DRIVE_IN1, HIGH);
  digitalWrite(DRIVE_IN2, LOW);
  ledcWrite(0, speed);
}
void driveBackward(int speed) {
  digitalWrite(DRIVE_IN1, LOW);
  digitalWrite(DRIVE_IN2, HIGH);
  ledcWrite(0, speed);
}
void stopDrive() {
  digitalWrite(DRIVE_IN1, LOW);
  digitalWrite(DRIVE_IN2, LOW);
  ledcWrite(0, 0);
}
void steerLeft(int speed) {
  digitalWrite(STEER_IN3, HIGH);
  digitalWrite(STEER_IN4, LOW);
  ledcWrite(1, speed);
}
void steerRight(int speed) {
  digitalWrite(STEER_IN3, LOW);
  digitalWrite(STEER_IN4, HIGH);
  ledcWrite(1, speed);
}
void steerStop() {
  digitalWrite(STEER_IN3, LOW);
  digitalWrite(STEER_IN4, LOW);
  ledcWrite(1, 0);
}

// --- SENSOR ---
long getDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 20000);
  long dist = duration * 0.034 / 2;
  if (dist == 0) return 999;  // no echo
  return dist;
}

// --- MOVEMENT HELPERS ---
bool safeToMoveForward() {
  return (getDistance(TRIG_FC, ECHO_FC) > SAFE_DIST);
}
bool safeToMoveBackward() {
  return (getDistance(TRIG_BC, ECHO_BC) > SAFE_DIST);
}

void moveForwardLeft(int duration) {
  Serial.println("Forward Left");
  if (!safeToMoveForward()) { Serial.println("Blocked front!"); return; }
  driveForward(DRIVE_SPEED);
  steerLeft(STEER_SPEED);
  unsigned long t = millis();
  while (millis() - t < duration) {
    if (!safeToMoveForward()) break;
  }
  stopDrive(); steerStop();
}

void moveBackwardRight(int duration) {
  Serial.println("Backward Right");
  if (!safeToMoveBackward()) { Serial.println("Blocked back!"); return; }
  driveBackward(DRIVE_SPEED);
  steerRight(STEER_SPEED);
  unsigned long t = millis();
  while (millis() - t < duration) {
    if (!safeToMoveBackward()) break;
  }
  stopDrive(); steerStop();
}

// --- EXIT PARALLEL PARKING ---
void exitParallelParking(bool leftOpen) {
  int retries = 0;
  bool success = false;

  while (retries < RETRY_LIMIT && !success) {
    Serial.print("Attempt "); Serial.println(retries + 1);

    if (leftOpen) {
      moveBackwardRight(1500);
      delay(500);

      moveForwardLeft(1500);
      delay(500);

      moveBackwardRight(1500);
      delay(500);

      moveForwardLeft(1500);
      delay(500);

    } else { // Right side open, mirror moves
      moveBackwardLeft(1500);
      delay(500);

      moveForwardRight(1500);
      delay(500);

      moveBackwardLeft(1500);
      delay(500);

      moveForwardRight(1500);
      delay(500);
    }

    // check if out (front + back clear)
    if (safeToMoveForward() && safeToMoveBackward()) {
      Serial.println("✅ Successfully exited parking!");
      success = true;
    } else {
      retries++;
      Serial.println("⚠️ Blocked, retrying...");
    }
  }

  if (!success) {
    Serial.println("❌ FAILED EXIT");
    stopDrive(); steerStop();
  }
}

void setup() {
  Serial.begin(115200);

  // motor setup
  pinMode(DRIVE_IN1, OUTPUT);
  pinMode(DRIVE_IN2, OUTPUT);
  pinMode(STEER_IN3, OUTPUT);
  pinMode(STEER_IN4, OUTPUT);

  ledcAttachPin(DRIVE_ENA, 0);
  ledcAttachPin(STEER_ENB, 1);
  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);

  // ultrasonic setup
  int trigPins[] = {TRIG_FL, TRIG_FC, TRIG_FR, TRIG_BR, TRIG_BC, TRIG_BL};
  int echoPins[] = {ECHO_FL, ECHO_FC, ECHO_FR, ECHO_BR, ECHO_BC, ECHO_BL};
  for (int i = 0; i < 6; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  Serial.println("Starting parallel parking exit...");

  // check which side is open
  long leftDist = getDistance(TRIG_BL, ECHO_BL);
  long rightDist = getDistance(TRIG_BR, ECHO_BR);

  if (leftDist > rightDist) {
    Serial.println("Left side open, exiting...");
    exitParallelParking(true);
  } else {
    Serial.println("Right side open, exiting...");
    exitParallelParking(false);
  }
}

void loop() {
  // Nothing, exit runs once
}

// --- Extra turning helpers ---
void moveBackwardLeft(int duration) {
  Serial.println("Backward Left");
  if (!safeToMoveBackward()) { Serial.println("Blocked back!"); return; }
  driveBackward(DRIVE_SPEED);
  steerLeft(STEER_SPEED);
  unsigned long t = millis();
  while (millis() - t < duration) {
    if (!safeToMoveBackward()) break;
  }
  stopDrive(); steerStop();
}
void moveForwardRight(int duration) {
  Serial.println("Forward Right");
  if (!safeToMoveForward()) { Serial.println("Blocked front!"); return; }
  driveForward(DRIVE_SPEED);
  steerRight(STEER_SPEED);
  unsigned long t = millis();
  while (millis() - t < duration) {
    if (!safeToMoveForward()) break;
  }
  stopDrive(); steerStop();
}
