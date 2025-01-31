#include <Arduino.h>

// Motor 1 (Primary)
#define Step1STEP 2
#define Step1DIR 3
#define Step1EN 4

// Motor 2 (Secondary)
#define Step2STEP 8
#define Step2DIR 9
#define Step2EN 10

#define STEPS_PER_REV 200       // 200 steps for 1 full revolution (full step mode)
#define MICROSTEPS 32           // Microstepping setting (1/32 step)
#define STEPS_PER_MICROSTEP (STEPS_PER_REV * MICROSTEPS) // Total steps per revolution in microstepping mode

// Mechanic values in mm
float spool_width = 70.0;
float wire_diameter = 0.254;

// Variables
bool forward = false;
int step1Delay = 20;  // Step delay in microseconds (adjust for speed)
int step2Delay = 125;
int revs_per_layer = spool_width / wire_diameter; // Number of full wraps per layer
int count = 0;

// Setup function
void set_up_motors() {
  pinMode(Step1STEP, OUTPUT);
  pinMode(Step1DIR, OUTPUT);
  pinMode(Step1EN, OUTPUT);
  pinMode(Step2STEP, OUTPUT);
  pinMode(Step2DIR, OUTPUT);
  pinMode(Step2EN, OUTPUT);
  digitalWrite(Step1EN, HIGH);
  digitalWrite(Step2EN, HIGH);

  Serial.begin(9600);
  Serial.println("Press SPACE or ENTER to start the motor...");
}

// Change motor direction
void change_direction() {
  forward = !forward;
  digitalWrite(Step2DIR, forward ? HIGH : LOW);
}

// Function to pulse the STEP pin
void stepMotor(int steps, int motor) {
  int stepPin = (motor == 1) ? Step1STEP : Step2STEP;
  int enPin = (motor == 1) ? Step1EN : Step2EN;
  int stepDelay = (motor == 1) ? step1Delay : step2Delay;
  digitalWrite(enPin, LOW);
  
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  
  digitalWrite(enPin, HIGH);
}

// Drive both motors
void drive_motors() {
  int stepsPerLayer = round(STEPS_PER_MICROSTEP/8 * wire_diameter);
  int revs_left = revs_per_layer;  // Reset per layer

  Serial.print("Revs per layer: ");
  Serial.println(revs_left);
  Serial.print("Steps per lateral movement: ");
  Serial.println(stepsPerLayer);

  if (revs_left <= 0) return; // Prevent running if invalid

  while (revs_left > 0) {
    stepMotor(STEPS_PER_MICROSTEP, 1); // Wrap wire
    count++;
    // if(count>10){
    //   step1Delay =50;
    // }
    stepMotor(stepsPerLayer, 2); // Move laterally
    Serial.print("Count: ");
    Serial.println(count);
    revs_left--;

    if (revs_left == 0) {
      change_direction();
      Serial.println("Layer complete. Ready for next trigger.");
      count = 0;
      return;  // Exit function to wait for the next trigger
    }
  }
}

void setup() {
  set_up_motors();
  digitalWrite(Step1DIR, HIGH);
  digitalWrite(Step2DIR, forward ? HIGH : LOW);
}

void loop() {
  // Clear any leftover Serial input to prevent immediate retriggering
  while (Serial.available()) {
    Serial.read(); // Read and discard any remaining input
  }

  // Wait for SPACE (' ') or ENTER ('\n') input to start motors
  if (Serial.available()) {
    char input = Serial.read();
    
    if (input == ' ' || input == '\n') {
      Serial.println("Starting motors...");
      drive_motors();
      Serial.println("Cycle complete. Press SPACE or ENTER to run again.");
    }
  }
}
