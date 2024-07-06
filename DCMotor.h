#ifndef DCMOTOR_H
#define DCMOTOR_H
#include "HardwareSerial.h"


class DCMotor
{
    public:
        DCMotor(int dir1_pin, int dir2_pin, int pwm_pin);
        virtual ~DCMotor();
        void stopMotor();
        bool get_direction();
        int get_pwm();
        void startMotor(int pwm_in, bool direction_in=1);
        void setupMotor();
        void display_direction(HardwareSerial * Serial);
        int process_command(String * input_command, HardwareSerial * Serial);

    protected:

    private:
        void setPins(String Mode);
        float floatMap(float x, float in_min, float in_max, float out_min, float out_max);
        void set_direction(bool direction_in);
        void set_pwm(int pwm);
        bool direction=true;
        int pwm;
        int pin_dir1;
        int pin_dir2;
        int pin_pwm;
};

#endif // DCMOTOR_H
