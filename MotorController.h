#ifndef MotorController_h
#define MotorController_h
class MotorController
{
private:
    byte _left_motor_pin1;
    byte _left_motor_pin2;
    byte _right_motor_pin1;
    byte _right_motor_pin2;

public:
    MotorController(byte left_motor_pin1, byte left_motor_pin2, byte right_motor_pin1, byte right_motor_pin2);
    void init();
    void stop();
    void forward();
    void backward();
    void left();
    void right();
};
#endif
