#include <Arduino.h>

// HW-216 (DRV8825) Pin Mapping
#define Step2STEP 4  // Pulse pin Motor 2
#define Step1DIR 3   // Direction pin Motor 2
#define Step1STEP 8  // Pulse pin Motor 1
#define Step2DIR 9   // Direction pin Motor 1
#define Step2EN 10   // Shared Enable pin


#define STEPS_PER_REV 200                // 1.8 deg per step
#define MICROSTEPS 1                     // Set based on M0, M1, M2 jumper configuration
#define MM_PER_STEP (0.04 / MICROSTEPS)  // 0.04 mm per step at full stepping

float wire_diameter = 0.254;
bool forward = true;
int stepDelay = 2500;  // Cruise speed delay in us
int count = 0;

void set_up_motor() {
  pinMode(Step2STEP, OUTPUT);
  pinMode(Step1DIR, OUTPUT);
  pinMode(Step1STEP, OUTPUT);
  pinMode(Step2DIR, OUTPUT);
  pinMode(Step2EN, OUTPUT);


  // Enable driver and initialize BOTH direction pins
  digitalWrite(Step2EN, HIGH);
  digitalWrite(Step1DIR, forward ? HIGH : LOW);
  digitalWrite(Step2DIR, forward ? HIGH : LOW);

  Serial.begin(9600);
  while (!Serial) { ; }
  delay(1000);
  Serial.println("HW-216 Stepper Driver Ready.");
  Serial.println("Press SPACE to start motor...");
}

void stepMotor(int steps) {
  digitalWrite(Step2EN, LOW);

  int startDelay = 5000;  // Slow start speed (5ms delay)
  int currentDelay = startDelay;

  for (int i = 0; i < steps; i++) {
    // Pulse HIGH
    digitalWrite(Step2STEP, HIGH);
    digitalWrite(Step1STEP, HIGH);
    delayMicroseconds(20);

    // Pulse LOW
    digitalWrite(Step1STEP, LOW);
    digitalWrite(Step2STEP, LOW);

    // Smooth Acceleration Ramp
    if (i < 50 && currentDelay > stepDelay) {
      currentDelay -= (startDelay - stepDelay) / 50;
    }

    delayMicroseconds(currentDelay);
  }
}

void change_direction() {
  forward = !forward;
  digitalWrite(Step1DIR, forward ? HIGH : LOW);
  digitalWrite(Step2DIR, forward ? HIGH : LOW);

  count = 0;
  Serial.print("Direction updated: ");
  Serial.println(forward ? "FORWARD" : "BACKWARD");
}

void drive_motor() {
  int stepsPerMove = max(1, (int)round(wire_diameter / MM_PER_STEP));
  int revs_left = 250;

  Serial.print("Revs to process: ");
  Serial.println(revs_left);
  Serial.print("Steps per movement: ");
  Serial.println(stepsPerMove);

  while (revs_left > 0) {
    stepMotor(stepsPerMove);
    count++;
    revs_left--;

    if (Serial.available()) {
      char input = Serial.read();
      if (input == 's' || input == 'S') {
        Serial.println("Stopping motor...");
        return;
      }
    }
  }

  change_direction();
  digitalWrite(Step2EN, HIGH);

}

void setup() {
  set_up_motor();
}

void loop() {
  if (Serial.available()) {
    char input = Serial.read();
    if (input == 'c' || input == 'C') {
      change_direction();
    } else if (input == ' ') {
      Serial.println("Starting motor execution...");
      drive_motor();
    }
  }
}