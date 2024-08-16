#include <DCMotor.h>
#include <DCRPMDriver.h>

//here define pins for motor
#define M1_dir1 7
#define M1_dir2 8
#define M1_pwm 9

#define COMMANDS "Motor controls:\n'm_<num>' - set motor to <num>% power\n'm_off' - turn motor off, same as m_0\n'm_+' - set direction clockwise\n'm_-' - set direction counterclockwise\n'm_dir' - get direction\nAny other command will stop the motor."



String computerdata;  // in the main
bool serial_event=false;//            in the main


DCMotor my_motor(M1_dir1, M1_dir2, M1_pwm);

DCRPMDriver my_driver(&my_motor);

// /*
//  * Created by ArduinoGetStarted.com
//  *
//  * This example code is in the public domain
//  *
//  * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-potentiometer
//  */


// //function serialEvent is recognizable by arduino in the main
 void serialEvent() {
   computerdata=Serial.readString(); //this interrupt will trigger when the data coming from the serial monitor(pc/mac/other) is received.
   serial_event = true; //set the serial event flag only if command is in proper format
 }

  int generate_switch(String *computerdata){
    
    if (computerdata->substring(0,computerdata->indexOf("_")+1)=="m_"){
      return 0;
    }else{
      return -1;
    }
  }

 void ProcessCommand(String *computerdata){
     int error;
     switch (generate_switch(computerdata)){
        case 0:
          error=my_driver.process_command(computerdata,&Serial);
          if(error){
            Serial.println("Command for motor doesn't exist. Halting everyting.");
            my_driver.stop();
          }
          break;
       default:
          Serial.println("Command doesn't exist. Halting everyting.");
          my_driver.stop();
          Serial.println(COMMANDS);
          break;
     }

 }

 // the setup routine runs once when you press reset:
 void setup() {
   my_motor.setup();
   Serial.begin(9600);
   Serial.println(COMMANDS);
}

// // the loop routine runs over and over again forever:
 void loop() {
   if (serial_event){
     ProcessCommand(&computerdata);
     serial_event=false;
   }
 }

