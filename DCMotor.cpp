#include "DCMotor.h"
#include "Arduino.h"
#include "HardwareSerial.h"

DCMotor::DCMotor(int dir1_pin, int dir2_pin, int pwm_pin)
{   
    pin_dir1=dir1_pin;
    pin_dir2=dir2_pin;
    pin_pwm=pwm_pin;
    pwm=0;
}

DCMotor::~DCMotor()
{
    //dtor
}

void DCMotor::setPins(String Mode){
  if (Mode=="Stop"){
    digitalWrite(pin_dir1, LOW);
    digitalWrite(pin_dir2, LOW);
  }else if(Mode=="CW"){
    digitalWrite(pin_dir1, HIGH);
    digitalWrite(pin_dir2, LOW);
  }else if(Mode=="CCW"){
    digitalWrite(pin_dir1, LOW);
    digitalWrite(pin_dir2, HIGH);
  }

}

void DCMotor::stopMotor(){
    set_pwm(0);
    setPins("Stop");   
    delay(250);
}

bool DCMotor::get_direction(){
    return direction;
}

void DCMotor::set_direction(bool direction_in){
    direction=direction_in;
    if(direction){
      setPins("CW");    
    }else{
      setPins("CCW");
    }
}

int DCMotor::get_pwm(){
    return pwm;
}
void DCMotor::set_pwm(int pwm_in){
    if (pwm_in<0){
        pwm_in=0;
    }else if (pwm_in>100){
        pwm_in=100;
    }
    pwm=pwm_in;
    if (pwm==0){
      setPins("Stop");
    }
}

void DCMotor::startMotor(int pwm_in, bool direction_in){
    if (direction_in!=direction){
        stopMotor();
    }
    set_direction(direction_in);
    set_pwm(pwm_in);
    analogWrite(pin_pwm, floatMap(pwm, 0,100,0,255));
}

void DCMotor::setupMotor(){
    pinMode(pin_dir1, OUTPUT);
    pinMode(pin_dir2, OUTPUT);
    pinMode(pin_pwm, OUTPUT);
}

float DCMotor::floatMap(float x, float in_min, float in_max, float out_min, float out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void DCMotor::display_direction(HardwareSerial *Serial){
    if (direction){
     Serial->println("CW");
     //(*Serial).println("CW");
  }else{
     Serial->println("CCW");
  }

}

int DCMotor::process_command(String *input_command, HardwareSerial * Serial){
        //change of motor direction
        if (input_command->charAt(input_command->indexOf("_")+1)=='+'){
            startMotor(get_pwm(),1);
            return 0;
         //change of motor direction
         }else if (input_command->charAt(input_command->indexOf("_")+1)=='-'){
            startMotor(get_pwm(),0);
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+4)=="off"){
            stopMotor();
            return 0;
         }else if (input_command->substring(input_command->indexOf("_")+1,input_command->indexOf("_")+4)=="dir"){
            display_direction(Serial);
            return 0;
         }else if ((input_command->substring(input_command->indexOf("_")+1).toInt()<=0) & (input_command->substring(input_command->indexOf("_")+1)!="0")){
          //unallowed command
            return 1;
            //set count at inf
         }else{
            startMotor(input_command->substring(input_command->indexOf("_")+1).toInt(),get_direction());
            return 0;
         }
}
