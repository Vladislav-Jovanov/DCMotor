#include <DCMotor.h>
#include <DCRPMDriver.h>

//here define pins for motor
#define M1_dir1 18
#define M1_dir2 19
#define M1_pwm 21

int pwm;

DCMotor my_motor(M1_dir1, M1_dir2, M1_pwm);

 // the setup routine runs once when you press reset:
 void setup() {
   my_motor.setup();
   my_motor.set_coils(1);
   Serial.begin(115200);
}

// // the loop routine runs over and over again forever:
 void loop() {
   pwm=analogRead(34);
   //Serial.println(pwm);
   my_motor.start(pwm);
 }

