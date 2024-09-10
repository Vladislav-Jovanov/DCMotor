#include <DCMotor.h>
#include <DCRPMDriver.h>

//here define pins for motor
#if defined (__AVR__)
    #define M1_dir1 7
    #define M1_dir2 8
    #define M1_pwm 9
#endif
#if defined (ESP32)
    #define M1_dir1 18
    #define M1_dir2 19
    #define M1_pwm 21
    #define PRG_up 5
    #define PRG_down 4
    #define PRG_start 2
#endif

DCMotor my_motor(M1_dir1, M1_dir2, M1_pwm);
DCRPMDriver my_driver(&my_motor);
int prg_nmbr=0;
bool down;


void ARDUINO_ISR_ATTR IRS_up(){
    prg_nmbr++;
    if (prg_nmbr>=4){
        prg_nmbr=prg_nmbr - 4;
    }
    
}

void ARDUINO_ISR_ATTR IRS_down(){
    down=true;
}

void ARDUINO_ISR_ATTR IRS_program(){
    switch(prg_nmbr){
        case 0:
            program_null();
            break;
        case 1:
            //program_one();
            break;
        case 2:
            //program_two();
            break;
        case 3:
            program_three();
            break;
    }
}

void my_delay(int time){
    while (!my_driver.is_action_finished()){
        delay(1);
    }
}

void program_three(){
    my_driver.set_parameters(CW,50,5,500);
    my_driver.start();
    my_delay(1);
    my_driver.update_speed(75);
    my_delay(1);
    my_driver.update_direction(CCW);
    my_delay(1);
    my_driver.update_time(10);
    my_delay(1);
    my_driver.update_speed(NULL);
    my_delay(1);
}


void program_null(){
    my_driver.set_parameters(CW,50,0,500);
    my_driver.start();
    my_delay(1);
    my_driver.stop();
}





// the setup routine runs once when you press reset:
void setup() {
    Serial.begin(115200);    
    pinMode(PRG_down, INPUT_PULLUP);
    Serial.println(digitalRead(PRG_down));
    attachInterrupt(PRG_down, IRS_down, FALLING);    
    my_motor.setup();
}

// // the loop routine runs over and over again forever:
void loop() {
    if (down){
        down=false;
        prg_nmbr--;
        if (prg_nmbr<=-1){
            prg_nmbr=prg_nmbr + 4;
        }
        Serial.println(prg_nmbr);
    }

   my_driver.main();
}

