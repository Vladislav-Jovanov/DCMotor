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


void DCRPMDriver::set_parameters(dir direction_in,float speed, float accl, float trn){
    direction=direction_in;
    final_speed=check_speed(speed);
    update_sign(final_speed);
    acclvalue=abs(accl);
    turns=abs(trn);
    if (acclvalue==NULL){
        time_to_turn=1000L*turns/abs(final_speed);
    }else{
        time_to_turn=1000L*turns/abs(final_speed) - 1000L*abs(final_speed)/acclvalue;
    }
}


void DCRPMDriver::update_sign(float &value){
    value=abs(value)*direction;        
}

float DCRPMDriver::check_speed(float input){
    if (input<0){
        input=0;
    }else if (input>100){
        input=100;    
    }
    return input;
}

//can be used only if motor is ON and not 
void DCRPMDriver::update_speed(float speed_in){        
    if (!turns_to_cover && movement){    
        final_speed=check_speed(speed_in);
        update_sign(final_speed);
        speed_reached=false;           
        if (acclvalue!=NULL){
            now_time=millis();
        }    
    }
}

void DCRPMDriver::update_accl(float accl){
    if (!turns_to_cover && movement){ 
        acclvalue=abs(accl);
    }
}


void DCRPMDriver::update_direction(dir direction_in){
    if (!turns_to_cover && movement){
        if (direction!=direction_in){
            direction=direction_in;
            update_sign(final_speed);                
        }
        if (acclvalue==NULL){
            direction_received=true;
        }else{
            speed_reached=false;
            now_time=millis();
        }
    }
    
}

void DCRPMDriver::start(){
    if (direction!=NULL && final_speed!=NULL && !movement){
        movement=true;
        direction_received=true;
        speed_reached=false;
        if (acclvalue!=NULL){
            now_time=millis();        
        }        
        if (turns!=NULL){
            turns_to_cover=true;        
        }  
    }
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
            speed_reached=true;
            if (turns_to_cover){
                now_time=millis();                            
            }             
        }
    }else if (speed>final_speed){
        if (speed>=0 && speed-acclvalue*25/1000<=0){
            Motor->start(0);
            Motor->set_coils(direction);        
        }
        speed=speed-acclvalue*25/1000;
        if (speed<=final_speed){
            speed=final_speed;
            speed_reached=true;
            if (turns_to_cover){
                stop();
                turns_to_cover=false;      
            }          
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
        if (!speed_reached && acclvalue==NULL){
            pwm=Motor->calc_pwm(abs(final_speed),0,100,0);        
            Motor->start(pwm);
            speed=final_speed;
            speed_reached=true;
            if (final_speed!=NULL){
                now_time=millis();                            
            }else{
                stop();
                turns_to_cover=false;            
            }
        }
        if (!speed_reached && acclvalue!=NULL){
            if (millis()-now_time>=25){
                ramp_up_down();
                pwm=Motor->calc_pwm(abs(speed),0,100,0); 
                Motor->start(pwm);
                now_time=millis();        
            }
        }
        if (speed_reached && turns_to_cover){
           if (millis()-now_time>=time_to_turn){
                final_speed=0;
                speed_reached=false;           
                if (acclvalue!=NULL){
                    now_time=millis();
                }        
            }
        }
    }
}

int DCRPMDriver::process_command(String *input_command, HardwareSerial * Serial){
    if (serial_enabled){
        if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+5)=="init"){
            float spd=input_command->substring(input_command->indexOf("S")+1,input_command->indexOf("A")).toFloat();
            float accl=input_command->substring(input_command->indexOf("A")+1,input_command->indexOf("T")).toFloat();
            float trn=input_command->substring(input_command->indexOf("T")+1).toFloat();
            if (input_command->substring(input_command->indexOf("D")+1,input_command->indexOf("S"))=="CW"){
                set_parameters(CW,spd,accl,trn);            
            }else if (input_command->substring(input_command->indexOf("D")+1,input_command->indexOf("S"))=="CCW"){
                set_parameters(CCW,spd,accl,trn);
            }
            return 0;
        }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+3)=="CW"){
            update_direction(CW);
            return 0;
        }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+4)=="CCW"){
            update_direction(CCW);
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
        }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+3)=="l?"){
            Serial->println(get_turns());
            return 0;
        }else if (input_command->substring(input_command->indexOf("s")+1,input_command->indexOf("s")+5)=="NULL"){           
            update_speed(NULL);
            return 0;
        }else if (input_command->substring(input_command->indexOf("a")+1,input_command->indexOf("a")+5)=="NULL"){
            update_accl(NULL);        
            return 0;
        }else if (input_command->indexOf("s")>0){
            if (input_command->substring(input_command->indexOf("s")+1).toFloat()<=0){         
                return 1;
            }else {           
                update_speed(input_command->substring(input_command->indexOf("s")+1).toFloat());
                return 0;
            }
        }else if (input_command->indexOf("a")>0){
            if (input_command->substring(input_command->indexOf("a")+1).toFloat()<=0){          
                return 1;
            }else if (input_command->substring(input_command->indexOf("a")+1).toFloat()>0){
                update_accl(input_command->substring(input_command->indexOf("a")+1).toFloat());        
                return 0;
            }
        }else{
            return 1;
        }
    }
}
