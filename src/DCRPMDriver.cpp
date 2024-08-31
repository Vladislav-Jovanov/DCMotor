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
    speed=0;
    Motor->stop();
    Motor->stop_coils();
    movement=false; 
}

void DCRPMDriver::update_spd_sign(){
    final_speed=abs(final_speed)*direction;        
}

void DCRPMDriver::set_speed(float speed_in){    
    if (speed_in<0){
        speed_in=0;
    }else if (speed_in>100){
        speed_in=100;    
    }
    final_speed=speed_in;
    update_spd_sign();
    if (movement){
        speed_received=true;           
        if (acclvalue!=NULL){
            now_time=millis();
        }    
    }
}

void DCRPMDriver::set_accl(float accl){
    acclvalue=accl;
}


void DCRPMDriver::set_direction(dir direction_in){
    if (direction!=direction_in){
        direction=direction_in;
        update_spd_sign();                
    }
    if (movement){
        if (acclvalue==NULL){
            direction_received=true;
        }else{
            speed_received=true;
            now_time=millis();
        }
    }
    
    
}

void DCRPMDriver::start(){
    if (direction!=NULL && final_speed!=NULL && !movement){
        movement=true;
        direction_received=true;
        speed_received=true;
        if (acclvalue!=NULL){
            now_time=millis();        
        }
    }
}


void DCRPMDriver::start(float speed_in, dir direction_in){
    set_direction(direction_in);    
    set_speed(speed_in);    
    start();
}





float DCRPMDriver::floatMap(float x, float in_min, float in_max, float out_min, float out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

String DCRPMDriver::get_direction(){
    if (direction>0){
     return "CW";
  }else if (direction<0){
     return "CCW";
  }

}


void DCRPMDriver::ramp_up_down(){
    if (speed<final_speed){
        if (speed<0 && speed+acclvalue*25/1000>=0){
            Motor->start(0);
            Motor->set_coils(direction);       
        }
        speed=speed+acclvalue*25/1000;
        if (speed>=final_speed){
            speed=final_speed;
            speed_received=false;            
        }
    }else if (speed>final_speed){
        if (speed>=0 && speed-acclvalue*25/1000<=0){
            Motor->start(0);
            Motor->set_coils(direction);        
        }
        speed=speed-acclvalue*25/1000;
        if (speed<=final_speed){
            speed=final_speed;
            speed_received=false;            
        }
    }
    //Serial.print(speed);
    //Serial.print("/");
    //Serial.println(final_speed);
}

void DCRPMDriver::main(){
    if (movement){
        if (direction_received){
            Motor->set_coils(direction);
            direction_received=false;
        }        
        if (speed_received && acclvalue==NULL){
            pwm=Motor->calc_pwm(abs(final_speed),0,100,0);        
            Motor->start(pwm);
            speed=final_speed;
            speed_received=false;
        }
        if (speed_received && acclvalue!=NULL){
            if (millis()-now_time>=25){
                ramp_up_down();
                pwm=Motor->calc_pwm(abs(speed),0,100,0); 
                Motor->start(pwm);
                now_time=millis();        
            }
        }
    }
}

int DCRPMDriver::process_command(String *input_command, HardwareSerial * Serial){      
        if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+3)=="CW"){
            set_direction(CW);
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+4)=="CCW"){
            set_direction(CCW);
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+4)=="off"){
            stop();
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+3)=="on"){
            start();
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+3)=="d?"){
            Serial->println(get_direction());
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+3)=="s?"){
            Serial->println(get_speed());
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+3)=="a?"){
            Serial->println(get_accl());
            return 0;
         }else if (input_command->substring(input_command->indexOf("s")+1,input_command->indexOf("s")+5)=="NULL"){           
            set_speed(NULL);
            return 0;
         }else if (input_command->substring(input_command->indexOf("a")+1,input_command->indexOf("a")+5)=="NULL"){
            set_accl(NULL);        
            return 0;
         }else if (input_command->indexOf("s")>0){
            if (input_command->substring(input_command->indexOf("s")+1).toFloat()<=0){         
                return 1;
            }else {           
                set_speed(input_command->substring(input_command->indexOf("s")+1).toFloat());
                return 0;
            }
         }else if (input_command->indexOf("a")>0){
            if (input_command->substring(input_command->indexOf("a")+1).toFloat()<=0){          
                return 1;
            }else if (input_command->substring(input_command->indexOf("a")+1).toFloat()>0){
                set_accl(input_command->substring(input_command->indexOf("a")+1).toFloat());        
                return 0;
            }
         }else{
            return 1;
         }
}
