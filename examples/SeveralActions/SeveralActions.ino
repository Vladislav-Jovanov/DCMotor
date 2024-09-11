#include <BouncyButton.h>
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
    #define PRG_start 4
    #define PRG_select 34
#endif

DCMotor my_motor(M1_dir1, M1_dir2, M1_pwm);
DCRPMDriver my_driver(&my_motor);
Bouncy_Button my_button(PRG_start,INPUT_PULLUP, FALLING, MYHARD);

int prg_nmbr;
bool start=false;
int counter=0;

void (*chosen_program)();

void IRS(){
    my_button.IRS();
}

int my_map(int input, int input_max, int input_min, int out_max, int out_min){
  return out_min + (input - input_min)*(out_max-out_min)/(input_max-input_min);
}

void select_program(){
  if(!start){
      int tmp=my_map(analogRead(PRG_select),4096,0,4,0);
      if (tmp!=prg_nmbr){
        delay(100);//to avoid glitches between numbers
        prg_nmbr=tmp;
        Serial.println(prg_nmbr);
      }
  }
}

void press_main(){
    if (start){
      chosen_program=&program_end;
      return;
    }else{
      start=true;
      counter=0;
    }
    if (start){
       switch(prg_nmbr){
          case 0:
            chosen_program=&program_null;
            break;
          case 1:
            chosen_program=&program_one;
            break;
          case 2:
            chosen_program=&program_two;
            break;
          case 3:
            chosen_program=&program_three;
            break;  
       }      
    }

}


void program_three(){
    my_driver.set_parameters(CW,50,5,500);
    my_driver.start();
    my_driver.update_speed(75);
    my_driver.update_direction(CCW);
    my_driver.update_time(10);
    my_driver.update_speed(NULL);
    start=false;
}

void program_null(){
    switch(counter){
      case 0:
        my_driver.set_parameters(CW,50,0,500);
        counter++;
        break;
      case 1:
        if (!my_driver.is_motor_turning()){
          my_driver.start();
        }
        if (my_driver.is_action_finished()){
          counter++;
        }
        break;
      case 2:
        my_driver.stop();
        start=false;
        break;
    }
}


void program_two(){
    my_driver.set_parameters(CW,50,5,500);
    my_driver.start();
    my_driver.update_speed(NULL);
    start=false;
}
void program_one(){
    my_driver.set_parameters(CW,50,5,500);
    my_driver.start();
    my_driver.update_time(10);
    my_driver.update_speed(NULL);
    start=false;
}

void program_end(){
  my_driver.stop();
  start=false;
}

void my_pass(){

}


// the setup routine runs once when you press reset:
void setup() {
    my_motor.setup();
    my_button.setup(&press_main,&IRS);
    Serial.begin(115200);    
    prg_nmbr=my_map(analogRead(PRG_select),4096,0,4,0);
    Serial.println(prg_nmbr);
    chosen_program=&my_pass;
}

// // the loop routine runs over and over again forever:
void loop() {
    select_program();
    my_button.main();
    my_driver.main();
    if (start){
      chosen_program();
    }
}

