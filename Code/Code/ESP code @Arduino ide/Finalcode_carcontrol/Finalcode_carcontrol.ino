// === DRIVE MOTOR PINS ===
#define DRIVE_IN1 26
#define DRIVE_IN2 27
#define DRIVE_ENA 25  // PWM

// === STEER MOTOR PINS ===
#define STEER_IN3 14
#define STEER_IN4 12
#define STEER_ENB 13  // PWM

// === PWM SETUP ===
#define PWM_CHANNEL_DRIVE 0
#define PWM_CHANNEL_STEER 1
#define PWM_FREQ 1000
#define PWM_RES 8

// === SPEED SETTINGS ===
int FORWARD_SPEED = 75;   // Forward speed
int REVERSE_SPEED = 85;   // Slightly faster reverse

// === ULTRASONIC SENSOR PINS ===
#define TRIG_FL 22   // Front Left
#define ECHO_FL 23
#define TRIG_FC 33   // Front Center
#define ECHO_FC 32
#define TRIG_FR 17   // Front Right
#define ECHO_FR 5
#define TRIG_BR 19   // Back Right
#define ECHO_BR 18
#define TRIG_BC 16   // Back Center
#define ECHO_BC 4
#define TRIG_BL 2    // Back Left
#define ECHO_BL 15

// === DISTANCE STRUCT ===
struct Dist {
  long distFL, distFC, distFR, distBL, distBC, distBR;
};

// === FUNCTION TO READ DISTANCE ===
long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
  long distance = duration * 0.034 / 2;
  if (distance == 0 || distance > 400) return 400; // max cap
  return distance;
}

Dist readAll() {
  Dist d;
  d.distFL = readDistance(TRIG_FL, ECHO_FL);
  d.distFC = readDistance(TRIG_FC, ECHO_FC);
  d.distFR = readDistance(TRIG_FR, ECHO_FR);
  d.distBL = readDistance(TRIG_BL, ECHO_BL);
  d.distBC = readDistance(TRIG_BC, ECHO_BC);
  d.distBR = readDistance(TRIG_BR, ECHO_BR);
  return d;
}

// === MOTOR FUNCTIONS ===
void driveForward(int speed) {
  digitalWrite(DRIVE_IN1, HIGH);
  digitalWrite(DRIVE_IN2, LOW);
  ledcWrite(PWM_CHANNEL_DRIVE, speed);
}

void driveReverse(int speed) {
  digitalWrite(DRIVE_IN1, LOW);
  digitalWrite(DRIVE_IN2, HIGH);
  ledcWrite(PWM_CHANNEL_DRIVE, speed);
}

void driveStop() {
  digitalWrite(DRIVE_IN1, LOW);
  digitalWrite(DRIVE_IN2, LOW);
  ledcWrite(PWM_CHANNEL_DRIVE, 0);
}

void steerLeft() {
  digitalWrite(STEER_IN3, HIGH);
  digitalWrite(STEER_IN4, LOW);
  ledcWrite(PWM_CHANNEL_STEER, 255);  // full steer
}

void steerRight() {
  digitalWrite(STEER_IN3, LOW);
  digitalWrite(STEER_IN4, HIGH);
  ledcWrite(PWM_CHANNEL_STEER, 255);  // full steer
}

void steerCenter() {
  digitalWrite(STEER_IN3, LOW);
  digitalWrite(STEER_IN4, LOW);
  ledcWrite(PWM_CHANNEL_STEER, 0);   // no steer
}

// === SETUP ===
void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(DRIVE_IN1, OUTPUT);
  pinMode(DRIVE_IN2, OUTPUT);
  pinMode(DRIVE_ENA, OUTPUT);

  pinMode(STEER_IN3, OUTPUT);
  pinMode(STEER_IN4, OUTPUT);
  pinMode(STEER_ENB, OUTPUT);

  // Attach PWM
  ledcSetup(PWM_CHANNEL_DRIVE, PWM_FREQ, PWM_RES);
  ledcAttachPin(DRIVE_ENA, PWM_CHANNEL_DRIVE);
  ledcSetup(PWM_CHANNEL_STEER, PWM_FREQ, PWM_RES);
  ledcAttachPin(STEER_ENB, PWM_CHANNEL_STEER);

  // Ultrasonic pins
  pinMode(TRIG_FL, OUTPUT); pinMode(ECHO_FL, INPUT);
  pinMode(TRIG_FC, OUTPUT); pinMode(ECHO_FC, INPUT);
  pinMode(TRIG_FR, OUTPUT); pinMode(ECHO_FR, INPUT);
  pinMode(TRIG_BL, OUTPUT); pinMode(ECHO_BL, INPUT);
  pinMode(TRIG_BC, OUTPUT); pinMode(ECHO_BC, INPUT);
  pinMode(TRIG_BR, OUTPUT); pinMode(ECHO_BR, INPUT);

  steerCenter();
  driveStop();
}

// === MAIN LOOP ===
void loop() {
  Dist d = readAll();

  Serial.print("FL:"); Serial.print(d.distFL);
  Serial.print(" FC:"); Serial.print(d.distFC);
  Serial.print(" FR:"); Serial.print(d.distFR);
  Serial.print(" BL:"); Serial.print(d.distBL);
  Serial.print(" BC:"); Serial.print(d.distBC);
  Serial.print(" BR:"); Serial.println(d.distBR);

  // 🚘 MAIN LOGIC
  if (d.distFC <= 25) {
    // Stop if front blocked
    driveStop();
    delay(300);

    // Decide reverse direction
    if (d.distFL > d.distFR) {
      steerLeft();
      driveReverse(REVERSE_SPEED);
    } else {
      steerRight();
      driveReverse(REVERSE_SPEED);
    }
    delay(600); // reverse for some time
    driveStop();
    steerCenter();
  } 
  else {
    // If too close left wall → steer right
    if (d.distFL <= 20) {
      steerRight();
      driveForward(FORWARD_SPEED);
    }
    // If too close right wall → steer left
    else if (d.distFR <= 20) {
      steerLeft();
      driveForward(FORWARD_SPEED);
    }
    else {
      steerCenter();
      driveForward(FORWARD_SPEED);
    }
  }

  delay(100);
}

