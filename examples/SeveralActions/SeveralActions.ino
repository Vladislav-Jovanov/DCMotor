#include <BouncyButton.h>
#include <DCMotor.h>
#include <DCRPMDriver.h>
#include <LEDIndicator.h>


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
    //#define PRG_slider 34
    #define PRG_up 15 //check
    #define PRG_down 13 //check
    #define PIN_LED 5
#endif

int prg_nmbr=0;
bool start=false;
int counter=0;

void (*chosen_program)();

LED_Indicator my_indicator(PIN_LED);
DCMotor my_motor(M1_dir1, M1_dir2, M1_pwm);
DCRPMDriver my_driver(&my_motor);
Bouncy_Button my_button(PRG_start,INPUT_PULLUP, FALLING, MYHARD);
#if defined PRG_up
Bouncy_Button program_up(PRG_up, INPUT_PULLUP, FALLING, MYSOFT);

void IRS_up(){
    program_up.IRS();
}
//this only works when !start and when blinking is finished
void press_up(){
  if (!my_indicator.is_blink_on()){
    prg_nmbr++;
    if (prg_nmbr>=4){
        prg_nmbr=prg_nmbr - 4;
    }
    Serial.println(prg_nmbr+1);
    my_indicator.start_blink(prg_nmbr+1,250);
  }
}
#endif

#if defined PRG_down
Bouncy_Button program_down(PRG_down, INPUT_PULLUP, FALLING, MYSOFT);

void IRS_down(){
    program_down.IRS();
}

//this only works when !start and if blinking is finished
void press_down(){
  if (!my_indicator.is_blink_on()){
    prg_nmbr--;
    if (prg_nmbr<=-1){
        prg_nmbr=prg_nmbr + 4;
    }
    Serial.println(prg_nmbr+1);
    my_indicator.start_blink(prg_nmbr+1,250);
  }
}
#endif

#if defined PRG_slider
int my_map(int input, int input_max, int input_min, int out_max, int out_min){
  return out_min + (input - input_min)*(out_max-out_min)/(input_max-input_min);
}

//works if !start and blinking is finished
void select_program(){
  if (!my_indicator.is_blink_on()){
    int tmp=my_map(analogRead(PRG_slider),4096,0,4,0);
    if (tmp!=prg_nmbr){
      delay(100);//to avoid glitches between numbers
      prg_nmbr=tmp;
      Serial.println(prg_nmbr+1);
      my_indicator.start_blink(prg_nmbr+1,250);
    }
  }
}
#endif

void IRS(){
    my_button.IRS();
}
//this happens when my_button is pressed
void press_main(){
    if (start){
      chosen_program=&program_end;
    }else{
      start=true;
      counter=0;
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

void update_step(){
  counter++;
  my_indicator.start_blink(1,250);
}

void program_three(){
    switch(counter){
      case 0:
        if (!my_driver.is_motor_turning()){
          my_driver.set_parameters(CW,50,5,500);
          my_driver.start();
        }else{
          update_step();
        }
        break;
      case 1:
        if (my_driver.is_action_finished()){
          update_step();
          my_driver.update_speed(75);
        }
        break;
      case 2:
        if (my_driver.is_action_finished()){
          update_step();
          my_driver.update_direction(CCW);
        }
      case 3:
        if (my_driver.is_action_finished()){
          update_step();
          my_driver.update_time(10);
        }
        break;
      case 4:
        if (my_driver.is_action_finished()){
          update_step();
          my_driver.update_speed(NULL);
        }
        break;
      case 5:
        if (my_driver.is_action_finished()){
           my_driver.stop();
           start=false;
        }
        break;
    }
}

void program_null(){
    switch(counter){
      case 0:
        if (!my_driver.is_motor_turning()){
          my_driver.set_parameters(CW,50,0,500);
          my_driver.start();
        }else{ 
          update_step();
        }
        break;
      case 1:
        if (my_driver.is_action_finished()){
          update_step();
          my_driver.update_speed(NULL);
        }
        break;
      case 2:
        if (my_driver.is_action_finished()){
          my_driver.stop();
          start=false;
        }
        break;
    }
}


void program_two(){
      switch(counter){
      case 0:
        if (!my_driver.is_motor_turning()){
          my_driver.set_parameters(CW,50,5,0);
          my_driver.start();
        }else{ 
          update_step();
        }
        break;
      case 1:
        if (my_driver.is_action_finished()){
          update_step();
          my_driver.update_speed(NULL);
        }
        break;
      case 2:
        if (my_driver.is_action_finished()){
          my_driver.stop();
          start=false;
        }
        break;
    }
}
void program_one(){
        switch(counter){
      case 0:
        if (!my_driver.is_motor_turning()){
          my_driver.set_parameters(CW,50,5,0);
          my_driver.start();
        }else{ 
          update_step();
        }
        break;
      case 1:
        if (my_driver.is_action_finished()){
          update_step();
          my_driver.update_time(20);
        }
        break;
      case 2:
        if (my_driver.is_action_finished()){
          update_step();
          my_driver.update_speed(NULL);
        }
        break;
      case 3:
        if (my_driver.is_action_finished()){
          my_driver.stop();
          start=false;
        }
        break;
    }
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
    #if defined PRG_up
      program_up.setup(&press_up,&IRS_up);
    #endif
    #if defined PRG_down
      program_down.setup(&press_down,&IRS_down);
    #endif
    Serial.begin(115200); 
    my_indicator.setup();
    chosen_program=&my_pass;
    #if defined PRG_slider   
    prg_nmbr=my_map(analogRead(PRG_slider),4096,0,4,0);
    #endif
    Serial.println(prg_nmbr+1);
    my_indicator.start_blink(prg_nmbr+1, 250);
}

// // the loop routine runs over and over again forever:
void loop() {
    my_button.main();//sets-resets start
    my_indicator.main();//reacts to start/stop and button press

    //exclusevly if started
    if (start){
      my_indicator.start();
      chosen_program();
      my_driver.main();
    //exclusevly if stopped
    }else{
      my_indicator.stop();
      #if defined PRG_slider
        select_program();
      #endif
      #if defined PRG_up
        program_up.main();
      #endif
      #if defined PRG_down
        program_down.main();
      #endif
    }
}

