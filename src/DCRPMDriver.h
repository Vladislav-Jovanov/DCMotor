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
        bool get_status(){return movement;};
        void stop();
        void main();
        int process_command(String * input_command, HardwareSerial * Serial);
        String get_direction();
    protected:

    private:
        bool speed_received=false;
        bool direction_received=false;        
        bool accl=false;
        float acclvalue=NULL;
        bool direction=NULL;
        unsigned int pwm=NULL;
        bool movement=NULL;
        float floatMap(float x, float in_min, float in_max, float out_min, float out_max);
        float speed=NULL;
        float new_speed=NULL;
        bool final_speed=true;
        DCMotor * Motor=NULL;
};

#endif // DCRPMDRIVER_H
