#include "DCMotor.h"
#include "Arduino.h"
#include "HardwareSerial.h"

DCMotor::DCMotor(int dir1_pin, int dir2_pin, int pwm_pin)
{   
    pin_dir1=dir1_pin;
    pin_dir2=dir2_pin;
    pin_pwm=pwm_pin;
}

DCMotor::~DCMotor()
{
    //dtor
}

void DCMotor::set_coils(bool direction){
  switch(direction){
    case 0: //"CCW"
        digitalWrite(pin_dir1, LOW);
        digitalWrite(pin_dir2, HIGH);
        break;
    case 1://"CW"
        digitalWrite(pin_dir1, HIGH);
        digitalWrite(pin_dir2, LOW);
        break;
    }
}


void DCMotor::short_break(){
    digitalWrite(pin_dir1, HIGH);
    digitalWrite(pin_dir2, HIGH);
}

void DCMotor::stop_coils(){
    digitalWrite(pin_dir1, LOW);
    digitalWrite(pin_dir2, LOW);
}


void DCMotor::stop(){
    start(0);
}


void DCMotor::start(unsigned int pwm_duty){
    analogWrite(pin_pwm, pwm_duty);
}

void DCMotor::setup(){
    TCCR1B = TCCR1B & B11111000 | B00000010; //  3921.16 Hz for pin 9
    pinMode(pin_dir1, OUTPUT);
    pinMode(pin_dir2, OUTPUT);
    pinMode(pin_pwm, OUTPUT);
    stop_coils();
}

void DCMotor::check_coils(){
    start(255);    
    set_coils(1);
    delay(1000);
    set_coils(0);
    delay(1000);
    stop_coils();
    stop();
}


