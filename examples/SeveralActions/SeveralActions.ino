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
    //#define PRG_slider 34
    #define PRG_up 15 //check
    #define PRG_down 13 //check
    #define PIN_LED 5
#endif

int prg_nmbr=0;
int tmp;
bool blink_on=false;
bool start=false;
bool led_on=false;
bool time_passed=false;//for LED
long int current_time;// for LED
int counter=0;
int old_counter=0;

void (*chosen_program)();

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
  if (!blink_on){
    prg_nmbr++;
    if (prg_nmbr>=4){
        prg_nmbr=prg_nmbr - 4;
    }
    Serial.println(prg_nmbr);
    init_blink();
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
  if (!blink_on){
    prg_nmbr--;
    if (prg_nmbr<=-1){
        prg_nmbr=prg_nmbr + 4;
    }
    Serial.println(prg_nmbr);
    init_blink();
  }
}
#endif

#if defined PRG_slider
int my_map(int input, int input_max, int input_min, int out_max, int out_min){
  return out_min + (input - input_min)*(out_max-out_min)/(input_max-input_min);
}

//works if !start and blinking is finished
void select_program(){
  if (!blink_on){
    int tmp=my_map(analogRead(PRG_slider),4096,0,4,0);
    if (tmp!=prg_nmbr){
      delay(100);//to avoid glitches between numbers
      prg_nmbr=tmp;
      Serial.println(prg_nmbr);
      init_blink();
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
      current_time=millis();
      counter=0;
      old_counter=0;
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

void init_blink(){
    tmp=prg_nmbr+1;
    blink_on=true;
    led_on=true;
    digitalWrite(PIN_LED,HIGH);
    time_passed=false;
    current_time=millis();
}

void blink(){
    if (tmp>0){
      if(millis()-current_time>=250){
        time_passed=true;
        current_time=millis();
      }

      if (!led_on && time_passed){
          digitalWrite(PIN_LED,HIGH);
          led_on=true;
          time_passed=false;
      }else if (led_on && time_passed){
          led_on=false;
          digitalWrite(PIN_LED,LOW);
          time_passed=false;
          tmp--;
      }
    }else{
      blink_on=false;
    }
}

//see to turn it into a class it only needs a pointer to counter and LED pin
void init_indicator(){

}

void led_indicator(){
  if(start){
    if (!led_on && !time_passed){
      digitalWrite(PIN_LED,HIGH);
      led_on=true;
    }else if (led_on && time_passed){
      led_on=false;
      digitalWrite(PIN_LED,LOW);
    }
    if (old_counter!=counter){
      if(millis()-current_time>=500){
        current_time=millis();
        old_counter=counter;
        time_passed=true;
      }
    }else{
      if(millis()-current_time>=500){
        current_time=millis();
        time_passed=false;
      }
    }
  }else if (!start && !blink_on){
    if(led_on){
      led_on=false;
      digitalWrite(PIN_LED,LOW);
      time_passed=false;
    }
  }

}


void program_three(){
    switch(counter){
      case 0:
        if (!my_driver.is_motor_turning()){
          my_driver.set_parameters(CW,50,5,500);
          my_driver.start();
        }else{
          counter++;  
        }
        break;
      case 1:
        if (my_driver.is_action_finished()){
          counter++;
          my_driver.update_speed(75);
        }
        break;
      case 2:
        if (my_driver.is_action_finished()){
          counter++;
          my_driver.update_direction(CCW);
        }
      case 3:
        if (my_driver.is_action_finished()){
          counter++;
          my_driver.update_time(10);
        }
        break;
      case 4:
        if (my_driver.is_action_finished()){
          counter++;
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
          counter++;  
        }
        break;
      case 1:
        if (my_driver.is_action_finished()){
          counter++;
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
          counter++;  
        }
        break;
      case 1:
        if (my_driver.is_action_finished()){
          counter++;
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
          counter++;  
        }
        break;
      case 1:
        if (my_driver.is_action_finished()){
          counter++;
          my_driver.update_time(20);
        }
        break;
      case 2:
        if (my_driver.is_action_finished()){
          counter++;
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
    pinMode(PIN_LED,OUTPUT);
    chosen_program=&my_pass;
    #if defined PRG_slider   
    prg_nmbr=my_map(analogRead(PRG_slider),4096,0,4,0);
    #endif
    Serial.println(prg_nmbr);
    init_blink();
}

// // the loop routine runs over and over again forever:
void loop() {
    my_button.main();//sets-resets start
    led_indicator();

    //exclusevly if started
    if (start){
      chosen_program();
      my_driver.main();
    //exclusevly if stopped
    }else{
      blink();
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

