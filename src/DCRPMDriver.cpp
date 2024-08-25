#include "DCRPMDriver.h"
#include "DCMotor.h"
#include "Arduino.h"
#include "HardwareSerial.h"

DCRPMDriver::DCRPMDriver(DCMotor* motor)
{   
    Motor=motor;

}

DCRPMDriver::~DCRPMDriver()
{
    //dtor
}


void DCRPMDriver::stop(){
    Motor->stop();
    Motor->stop_coils();
    movement=false; 
}


void DCRPMDriver::set_speed(float speed_in){
    if (speed_in<0){
        speed=0;
    }else if (speed_in>100){
        speed=100;
    }else{
        speed=speed_in;    
    }
    pwm=floatMap(speed, 0, 100, 0, 255);
    if (movement){
       Motor->start(pwm);
    }
}


void DCRPMDriver::set_direction(bool direction_in){
    direction=direction_in;
    if (movement){
       Motor->set_coils(direction);
    } 
}

void DCRPMDriver::start(){
    Motor->set_coils(direction);
    Motor->start(pwm);
    movement=true;
}


void DCRPMDriver::start(float speed_in, bool direction_in){
    set_speed(speed_in);
    set_direction(direction_in);
    start();
}





float DCRPMDriver::floatMap(float x, float in_min, float in_max, float out_min, float out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

String DCRPMDriver::get_direction(){
    if (direction){
     return "CW";
  }else{
     return "CCW";
  }

}

int DCRPMDriver::process_command(String *input_command, HardwareSerial * Serial){        
        //change of motor direction
        if (input_command->substring(input_command->indexOf("_"),input_command->indexOf("_")+3)=="CW"){
            set_direction(1);
            return 0;
         //change of motor direction
         }else if (input_command->substring(input_command->indexOf("_"),input_command->indexOf("_")+4)=="CCW"){
            set_direction(0);
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+4)=="off"){
            stop();
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+3)=="on"){
            start();
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+4)=="dir"){
            Serial->println(get_direction());
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1).toInt()<=0){
            //unallowed command            
            return 1;
            //set count at inf
         }else{
            set_speed(input_command->substring(input_command->indexOf("_")+1).toInt());         
            return 0;
         }
}
