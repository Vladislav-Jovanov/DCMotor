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
#endif

DCMotor my_motor(M1_dir1, M1_dir2, M1_pwm);

DCRPMDriver my_driver(&my_motor);

// the setup routine runs once when you press reset:
void setup() {
    Serial.begin();
    my_motor.setup();
}
//ramp up, const, ramp down 30 seconds
void first_step(){
    my_driver.set_direction(CW);
    my_driver.set_speed(50);
    my_driver.set_accl(5);
    my_driver.set_turns(1000);//this should be called after the speed and accl are set
    Serial.prinln(my_driver.get_turns());
    my_driver.start_turns(1000);
}
//ramp up, ramp down 20 seconds
void second_step(){
    my_driver.set_direction(CW);
    my_driver.set_speed(50);
    my_driver.set_accl(5);
    Serial.prinln(my_driver.get_turns());
    my_driver.start_turns(500);
}
//no ramp, const, no ramp 10 seconds
void third_step(){
    my_driver.set_direction(CCW);
    my_driver.set_speed(50);
    my_driver.set_accl(0);
    my_driver.start_turns(500);
}

// // the loop routine runs over and over again forever:
void loop() {
   my_driver.main();
}

