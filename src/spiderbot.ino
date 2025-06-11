// Define Blynk credentials FIRST (IMPORTANT!)
#define BLYNK_TEMPLATE_ID "TMPL3uMj2e6Pd"
#define BLYNK_TEMPLATE_NAME "new bot"
#define BLYNK_AUTH_TOKEN "fQE4IV4qnwC65PHOFrcpaQwNaxWMcxtQ"

// Include necessary libraries
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// WiFi credentials (hotspot or router)
char ssid[] = "Redmi Note 10 Lite";
char pass[] = "12345678";

// Create the PWM driver object
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// PWM pulse limits for standard servos
#define SERVO_MIN 100
#define SERVO_MAX 500

// Movement angle definitions
#define LIFTED     45
#define DOWN       90
#define FORWARD    60
#define BACKWARD   120
#define NEUTRAL    90

// Movement flags
bool forwardPressed = false;
bool backwardPressed = false;
bool turnLeftPressed = false;
bool stopPressed = false;

// Leg definitions for 8-leg robot (horizontal, vertical servo pairs)
struct Leg {
  uint8_t horizontal;
  uint8_t vertical;
};

// Define legs from front to back, left to right
Leg legs[8] = {
  {0, 1},   // Front Left
  {2, 3},   // Front Right  
  {4, 5},   // Mid Left
  {6, 7},   // Mid Right
  {8, 9},   // Rear Left
  {10, 11}, // Rear Right
  {12, 13}, // Back Left
  {14, 15}  // Back Right
};

// Function prototypes
void moveServo(uint8_t num, uint16_t angle);
void liftLeg(uint8_t legIndex);
void lowerLeg(uint8_t legIndex);
void moveLegTo(uint8_t legIndex, uint16_t angle);
void walkForwardTripodGait();
void walkBackwardTripodGait();
void turnLeftTripodGait();
void stopMovement();

// Blynk virtual button handlers
BLYNK_WRITE(V0) {
  forwardPressed = param.asInt();
  stopPressed = false;
}

BLYNK_WRITE(V1) {
  backwardPressed = param.asInt();
  stopPressed = false;
}

BLYNK_WRITE(V2) {
  turnLeftPressed = param.asInt();
  stopPressed = false;
}

BLYNK_WRITE(V3) {  // Stop button
  stopPressed = param.asInt();
  forwardPressed = false;
  backwardPressed = false;
  turnLeftPressed = false;
}

void setup() {
  Serial.begin(115200);

  // Start Blynk and PCA9685
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pwm.begin();
  pwm.setPWMFreq(50);  // Standard servo frequency

  // Initialize all servos to default position
  for (int i = 0; i < 8; i++) {
    moveServo(legs[i].horizontal, NEUTRAL);
    moveServo(legs[i].vertical, DOWN);
    delay(50); // Small delay to prevent servo overload
  }
}

void loop() {
  Blynk.run();  // Run Blynk code to check button presses

  // Continuous movement logic
  if (forwardPressed && !stopPressed) {
    walkForwardTripodGait();
  }
  else if (backwardPressed && !stopPressed) {
    walkBackwardTripodGait();
  }
  else if (turnLeftPressed && !stopPressed) {
    turnLeftTripodGait();
  }
  else if (stopPressed) {
    stopMovement();
  }
}

// Function to move servo by angle
void moveServo(uint8_t num, uint16_t angle) {
  uint16_t pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
  pwm.setPWM(num, 0, pulse);
}

void liftLeg(uint8_t legIndex) {
  moveServo(legs[legIndex].vertical, LIFTED);
}

void lowerLeg(uint8_t legIndex) {
  moveServo(legs[legIndex].vertical, DOWN);
}

void moveLegTo(uint8_t legIndex, uint16_t angle) {
  liftLeg(legIndex);
  delay(100);
  moveServo(legs[legIndex].horizontal, angle);
  delay(150);
  lowerLeg(legIndex);
  delay(100);
}

// Improved tripod gait for forward movement
void walkForwardTripodGait() {
  // Phase 1: Move first tripod (alternating pattern for stability)
  // Front Left, Mid Right, Rear Left, Back Right
  moveLegTo(0, FORWARD);  // Front Left
  delay(50);
  moveLegTo(3, FORWARD);  // Mid Right  
  delay(50);
  moveLegTo(4, FORWARD);  // Rear Left
  delay(50);
  moveLegTo(7, FORWARD);  // Back Right
  
  delay(100);  // Pause between phases
  
  // Phase 2: Move second tripod
  // Front Right, Mid Left, Rear Right, Back Left
  moveLegTo(1, FORWARD);  // Front Right
  delay(50);
  moveLegTo(2, FORWARD);  // Mid Left
  delay(50);
  moveLegTo(5, FORWARD);  // Rear Right
  delay(50);
  moveLegTo(6, FORWARD);  // Back Left
  
  // Phase 3: Reset first tripod to neutral (while second tripod supports)
  moveServo(legs[0].horizontal, NEUTRAL);  // Front Left
  delay(25);
  moveServo(legs[3].horizontal, NEUTRAL);  // Mid Right
  delay(25);
  moveServo(legs[4].horizontal, NEUTRAL);  // Rear Left
  delay(25);
  moveServo(legs[7].horizontal, NEUTRAL);  // Back Right
  
  delay(100);
  
  // Phase 4: Reset second tripod to neutral
  moveServo(legs[1].horizontal, NEUTRAL);  // Front Right
  delay(25);
  moveServo(legs[2].horizontal, NEUTRAL);  // Mid Left
  delay(25);
  moveServo(legs[5].horizontal, NEUTRAL);  // Rear Right
  delay(25);
  moveServo(legs[6].horizontal, NEUTRAL);  // Back Left
  
  delay(50);
}

// Improved tripod gait for backward movement
void walkBackwardTripodGait() {
  // Phase 1: Move first tripod backward
  moveLegTo(0, BACKWARD);  // Front Left
  delay(50);
  moveLegTo(3, BACKWARD);  // Mid Right  
  delay(50);
  moveLegTo(4, BACKWARD);  // Rear Left
  delay(50);
  moveLegTo(7, BACKWARD);  // Back Right
  
  delay(100);
  
  // Phase 2: Move second tripod backward
  moveLegTo(1, BACKWARD);  // Front Right
  delay(50);
  moveLegTo(2, BACKWARD);  // Mid Left
  delay(50);
  moveLegTo(5, BACKWARD);  // Rear Right
  delay(50);
  moveLegTo(6, BACKWARD);  // Back Left
  
  // Phase 3: Reset first tripod to neutral
  moveServo(legs[0].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[3].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[4].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[7].horizontal, NEUTRAL);
  
  delay(100);
  
  // Phase 4: Reset second tripod to neutral
  moveServo(legs[1].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[2].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[5].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[6].horizontal, NEUTRAL);
  
  delay(50);
}

// Improved tripod gait for turning left
void turnLeftTripodGait() {
  // Phase 1: First tripod - left legs back, right legs forward
  moveLegTo(0, BACKWARD);  // Front Left - backward
  delay(50);
  moveLegTo(3, FORWARD);   // Mid Right - forward
  delay(50);
  moveLegTo(4, BACKWARD);  // Rear Left - backward
  delay(50);
  moveLegTo(7, FORWARD);   // Back Right - forward
  
  delay(100);
  
  // Phase 2: Second tripod - left legs back, right legs forward
  moveLegTo(1, FORWARD);   // Front Right - forward
  delay(50);
  moveLegTo(2, BACKWARD);  // Mid Left - backward
  delay(50);
  moveLegTo(5, FORWARD);   // Rear Right - forward
  delay(50);
  moveLegTo(6, BACKWARD);  // Back Left - backward
  
  // Phase 3: Reset first tripod to neutral
  moveServo(legs[0].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[3].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[4].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[7].horizontal, NEUTRAL);
  
  delay(100);
  
  // Phase 4: Reset second tripod to neutral
  moveServo(legs[1].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[2].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[5].horizontal, NEUTRAL);
  delay(25);
  moveServo(legs[6].horizontal, NEUTRAL);
  
  delay(50);
}

// Stop all movement and set servos to default position
void stopMovement() {
  forwardPressed = false;
  backwardPressed = false;
  turnLeftPressed = false;
  
  // Gently return all legs to neutral position
  for (int i = 0; i < 8; i++) {
    moveServo(legs[i].horizontal, NEUTRAL);
    moveServo(legs[i].vertical, DOWN);
    delay(25);
  }
}