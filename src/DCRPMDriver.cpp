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
        speed_in=0;
    }else if (speed_in>100){
        speed_in=100;    
    }
    if (accl){
        new_speed=speed_in;
        //you also need time
    }else{
        speed=speed_in;    
        pwm=floatMap(speed, 0, 100, 0, 255);    
    }
    speed_received=true;
}


void DCRPMDriver::set_direction(bool direction_in){    
    if (accl){
        new_speed=speed;
        speed=0;
        speed_received=true;
        //you also need time
    }else{
        direction_received=true;
    }
    direction=direction_in;
    
}

void DCRPMDriver::start(){
    if (direction!=NULL && pwm!=NULL){
        Motor->set_coils(direction);
        Motor->start(pwm);
        movement=true;
    }
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


void DCRPMDriver::main(){
    if (movement && !accl){
        if (speed_received){        
            Motor->start(pwm);
            speed_received=false;
        }
        if (direction_received){
            Motor->set_coils(direction);
            direction_received=false;
        }
    }
}

int DCRPMDriver::process_command(String *input_command, HardwareSerial * Serial){        
        //change of motor direction
        if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+3)=="CW"){
            set_direction(1);
            return 0;
         //change of motor direction
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+4)=="CCW"){
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
