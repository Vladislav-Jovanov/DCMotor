#ifndef DCRPMDRIVER_H
#define DCRPMDRIVER_H
#include "HardwareSerial.h"
#include "DCMotor.h"

//this is in header so the main sketch can see it
#define DRIVERCOMMANDS "initD<CW/CCW>S<num>A<num>T<num>' - set motor direction, speed, accl, turns when motor is off\n'on' -turn motor on\n'off' - turn motor off\n'D<CW/CCW>' - update direction while motor is turning\n'D?' - get direction\n'S<num>' - update final speed while motor is turning\n'S?' - get current and final speed\n'A<num>' -update accl while motor is turning\n'A?' -get acceleration\n'T<num>' - update turns to turn\n'T?' - get turns"

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
        void stop();
        void main();
        void enable_serial(HardwareSerial * Serial);
        int process_command(String * input_command);
        void set_parameters(dir direction_in,float speed=NULL, float accl=NULL, float trn=NULL);
        void update_direction(dir direction_in);
        void update_speed(float speed);
        void update_accl(float accl);
        void update_time(float time);//in seconds
        bool is_motor_turning(){return movement;};
        bool is_action_finished(){return (speed_reached && !turns_to_cover);}
        String get_direction();
        String get_speed(){return String(speed)+"/"+String(final_speed);};
        float get_accl(){return acclvalue;};
        float get_turns(){return turns;};

    protected:

    private:
        float check_speed(float speed);
        bool serial_enabled=false;
        void update_sign(float &value);
        void ramp_up_down();
        unsigned long now_time;
        bool action_printed=true;
        bool speed_reached=true;
        bool direction_received=false;
        float acclvalue=NULL;//in pwm
        dir direction=NDEF;//current
        dir final_direction=NDEF;
        int pwm=NULL;
        bool movement=false;
        float floatMap(float x, float in_min, float in_max, float out_min, float out_max);
        float turns=NULL;
        float speed=NULL;//current
        float final_speed=NULL;
        float time_to_turn=NULL;
        bool turns_to_cover=false;
        DCMotor * Motor=NULL;
        HardwareSerial * Serial=NULL;
};

#endif // DCRPMDRIVER_H
