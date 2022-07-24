#include "Arduino.h"
#include "MotorController.h"

MotorController::MotorController(byte left_motor_pin1, byte left_motor_pin2, byte right_motor_pin1, byte right_motor_pin2)
{
    _left_motor_pin1 = left_motor_pin1;
    _left_motor_pin2 = left_motor_pin2;

    _right_motor_pin1 = right_motor_pin1;
    _right_motor_pin2 = right_motor_pin2;
}

void MotorController::init()
{
    pinMode(_left_motor_pin1, OUTPUT);
    pinMode(_left_motor_pin2, OUTPUT);

    pinMode(_right_motor_pin1, OUTPUT);
    pinMode(_right_motor_pin2, OUTPUT);
}

void MotorController::stop()
{
    digitalWrite(_left_motor_pin1, LOW);
    digitalWrite(_left_motor_pin2, LOW);

    digitalWrite(_right_motor_pin1, LOW);
    digitalWrite(_right_motor_pin2, LOW);
}

void MotorController::forward()
{
    digitalWrite(_left_motor_pin1, HIGH);
    digitalWrite(_left_motor_pin2, LOW);

    digitalWrite(_right_motor_pin1, HIGH);
    digitalWrite(_right_motor_pin2, LOW);
}

void MotorController::backward()
{
    digitalWrite(_left_motor_pin1, LOW);
    digitalWrite(_left_motor_pin2, HIGH);

    digitalWrite(_right_motor_pin1, LOW);
    digitalWrite(_right_motor_pin2, HIGH);
}

void MotorController::right()
{
    digitalWrite(_left_motor_pin1, HIGH);
    digitalWrite(_left_motor_pin2, LOW);

    digitalWrite(_right_motor_pin1, LOW);
    digitalWrite(_right_motor_pin2, HIGH);
}

void MotorController::left()
{
    digitalWrite(_left_motor_pin1, LOW);
    digitalWrite(_left_motor_pin2, HIGH);

    digitalWrite(_right_motor_pin1, HIGH);
    digitalWrite(_right_motor_pin2, LOW);
}