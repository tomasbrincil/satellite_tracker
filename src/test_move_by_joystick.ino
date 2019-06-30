#include <AccelStepper.h>
#include "RunningAverage.h"

RunningAverage joystick_x(10);
RunningAverage joystick_y(10);

// The X Stepper pins
#define STEPPER1_DIR_PIN 2
#define STEPPER1_STEP_PIN 3
// The Y stepper pins
#define STEPPER2_DIR_PIN 4
#define STEPPER2_STEP_PIN 5

#define JOYSTICK_X A0
#define JOYSTICK_Y A1

int j_x, speed_x, j_y, speed_y;

// Define some steppers and the pins the will use
AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);
AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN);

void setup()
{
    stepper1.setMaxSpeed(1800.0);
    stepper2.setMaxSpeed(1800.0);
}

void loop()
{
  j_x = analogRead(JOYSTICK_X);
  j_y = analogRead(JOYSTICK_Y);
  joystick_x.addValue(j_x);
  joystick_y.addValue(j_y);
  speed_x = map(joystick_x.getAverage()-14, 0, 1023, 1, 6);
  speed_y = map(joystick_y.getAverage(), 0, 1023, 1, 6);
  stepper1.setSpeed((speed_x-3)*500);
  stepper2.setSpeed((speed_y-3)*500);
  stepper1.runSpeed();
  stepper2.runSpeed();
}
