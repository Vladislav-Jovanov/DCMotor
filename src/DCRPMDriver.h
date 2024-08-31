#ifndef DCRPMDRIVER_H
#define DCRPMDRIVER_H
#include "HardwareSerial.h"
#include "DCMotor.h"

//this is in header so the main sketch can see it
#define COMMANDS "Motor controls:\n'm_s<num>' - set motor to <num>% power\n'm_off' - turn motor off\n'm_CW' - set direction clockwise\n'm_CCW' - set direction counterclockwise\n'm_d?' - get direction\nm_s? - get current and final speed\nm_a? -get acceleration \nAny other command will stop the motor."

//this will go to data formats
enum dir {
    CW=1,
    CCW=-1,
    NDEF=0
};


class DCRPMDriver
{
    public:
        DCRPMDriver(DCMotor* motor);
        virtual ~DCRPMDriver();
        void start();
        void start(float speed,dir direction);
        void set_direction(dir direction_in);        
        void set_speed(float speed);
        String get_speed(){return String(speed)+"/"+String(final_speed);};
        void set_accl(float accl);
        float get_accl(){return acclvalue;};
        bool get_status(){return movement;};
        void stop();
        void main();
        int process_command(String * input_command, HardwareSerial * Serial);
        String get_direction();
    protected:

    private:
        void update_spd_sign();
        void ramp_up_down();
        unsigned long now_time;
        bool speed_received=false;
        bool direction_received=false;        
        float acclvalue=NULL;//in pwm
        dir direction=NDEF;//current
        dir final_direction=NDEF;
        int pwm=NULL;
        bool movement=false;
        float floatMap(float x, float in_min, float in_max, float out_min, float out_max);
        float speed=NULL;//current
        float final_speed=NULL;
        DCMotor * Motor=NULL;
};

#endif // DCRPMDRIVER_H
