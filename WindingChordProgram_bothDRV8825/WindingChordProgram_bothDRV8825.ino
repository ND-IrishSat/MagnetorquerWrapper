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
#define MICROSTEPS 32           // Microstepping setting (1/16 step)
#define STEPS_PER_MICROSTEP (STEPS_PER_REV * MICROSTEPS) // Total steps per revolution in microstepping mode

// Mechanic values in mm
int spool_width = 34.7;
float wire_diameter = 0.46;

// Variables
// int revs_left = 0;
bool forward = true;
int step1Delay = 100;  // Step delay in microseconds (adjust for speed)
int step2Delay = 500;
int revs_left = (spool_width / wire_diameter);
int count = 0;

// Setup function
void set_up_motors() {
  pinMode(Step1STEP, OUTPUT);
  pinMode(Step1DIR, OUTPUT);
  pinMode(Step1EN, OUTPUT);
  pinMode(Step2STEP, OUTPUT);
  pinMode(Step2DIR, OUTPUT);
  pinMode(Step2EN, OUTPUT);
  digitalWrite(Step1EN,HIGH);
  digitalWrite(Step2EN,HIGH);

  Serial.begin(9600);
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
  int stepsPerLayer = ((STEPS_PER_MICROSTEP / 8) *wire_diameter ); // Steps per layer of wire
  // Wind wire layer by layer
  while (revs_left > 0) {
    // Motor 1 handles wire wrap
    stepMotor(STEPS_PER_MICROSTEP, 1);
    
    // Motor 2 handles lateral movement for alignment
    stepMotor(stepsPerLayer, 2); // 1 revolution for lateral alignment
    count += 1;
    Serial.println(count);
    
    // Adjust remaining revolutions

  }
  change_direction();
}

void setup() {
  set_up_motors();
  // digitalWrite(Step1DIR, forward ? HIGH : LOW); // Set initial direction
  //currently just switching this value to HIGH or LOW and then re uploading code after finishing a layer bc changing direction is too hard.
  digitalWrite(Step1DIR, HIGH);
  digitalWrite(Step2DIR, forward ? HIGH : LOW);
}

void loop() {
  // First layer
  drive_motors();

  // Move wire back with an offset
  // revs_left = spool_width / wire_diameter - 2;
  // drive_motors();
}
