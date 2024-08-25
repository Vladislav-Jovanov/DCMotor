#ifndef DCRPMDRIVER_H
#define DCRPMDRIVER_H
#include "HardwareSerial.h"
#include "DCMotor.h"

class DCRPMDriver
{
    public:
        DCRPMDriver(DCMotor* motor);
        virtual ~DCRPMDriver();
        void start();
        void start(float speed,bool direction);
        void set_direction(bool direction_in);        
        void set_speed(float speed);
        float get_speed(){return speed;};      
        void stop();
        int process_command(String * input_command, HardwareSerial * Serial);
        String get_direction();
    protected:

    private:
        bool direction;
        unsigned int pwm;
        bool movement;
        float floatMap(float x, float in_min, float in_max, float out_min, float out_max);
        float speed=NULL;
        DCMotor * Motor=NULL;
};

#endif // DCRPMDRIVER_H
