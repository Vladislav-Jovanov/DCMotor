#ifndef DCRPMDRIVER_H
#define DCRPMDRIVER_H
#include "HardwareSerial.h"
#include "DCMotor.h"

//this is in header so the main sketch can see it
#define DRIVERCOMMANDS "Motor controls:\n'm_s<num>' - set motor to <num>% power\n'm_off' - turn motor off\n'm_CW' - set direction clockwise\n'm_CCW' - set direction counterclockwise\n'm_d?' - get direction\nm_s? - get current and final speed\nm_a<num> set acceleration as power/s\nm_a? -get acceleration \nm_l<num> set the turn to turn (calibration still missing)\nm_l? get time to turn\nAny other command will stop the motor."

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
        int process_command(String * input_command, HardwareSerial * Serial);
        void set_parameters(dir direction_in,float speed=NULL, float accl=NULL, float trn=NULL);        
        void update_direction(dir direction_in);
        void update_speed(float speed);
        void update_accl(float accl);
        bool get_status(){return movement;};        
        float get_turns(){return turns;};
        String get_speed(){return String(speed)+"/"+String(final_speed);};
        float get_accl(){return acclvalue;};        
        String get_direction();        
        void enable_serial(){serial_enabled=true;};
    protected:

    private:
        float check_speed(float speed);
        bool serial_enabled=false;
        void update_sign(float &value);
        void ramp_up_down();
        unsigned long now_time;
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
};

#endif // DCRPMDRIVER_H
