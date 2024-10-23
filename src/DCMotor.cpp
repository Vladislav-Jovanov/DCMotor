#include <DCMotor.h>
#include <Arduino.h>

//this is here because it is only used in this file
#if defined (ESP32)
    #include <driver/ledc.h>
    #define LEDC_TIMER              LEDC_TIMER_0
    #define LEDC_MODE               LEDC_HIGH_SPEED_MODE
    #define LEDC_CHANNEL            LEDC_CHANNEL_0
    #define LEDC_DUTY_RES           LEDC_TIMER_12_BIT // Set duty resolution to 13 bits
    #define LEDC_DUTY               0 // Set duty to 50%. (2 ** 8) * 50% = 4096
    #define LEDC_FREQUENCY          7000 // Frequency in Hertz. Set frequency at 4 kHz
#endif



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

void DCMotor::set_coils(int direction){
  switch(direction){
    case -1: //"CCW"
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
    #if defined (__AVR__)
        analogWrite(pin_pwm, pwm_duty);
    #endif
    #if defined (ESP32)
        ledc_set_duty(LEDC_MODE , LEDC_CHANNEL, pwm_duty);
        ledc_update_duty(LEDC_MODE , LEDC_CHANNEL);
    #endif
}

void DCMotor::setup(){
    #if defined (__AVR__)
        TCCR1B = TCCR1B & B11111000 | B00000010; //  3921.16 Hz for pin 9
        pinMode(pin_pwm, OUTPUT);
    #endif
    #if defined (ESP32)
        ledc_timer_config_t ledc_timer = {
            .speed_mode       = LEDC_MODE,
            .duty_resolution  = LEDC_DUTY_RES,
            .timer_num        = LEDC_TIMER,
            .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
            .clk_cfg          = LEDC_AUTO_CLK
        };

        ledc_channel_config_t ledc_channel = {
            .gpio_num       = pin_pwm,
            .speed_mode     = LEDC_MODE,
            .channel        = LEDC_CHANNEL,
            .intr_type      = LEDC_INTR_DISABLE,
            .timer_sel      = LEDC_TIMER,
            .duty           = 0, // Set duty to 0%
            .hpoint         = 0 //phase shift
        };
        ledc_timer_config(&ledc_timer);
        ledc_channel_config(&ledc_channel);
    #endif

    pinMode(pin_dir1, OUTPUT);
    pinMode(pin_dir2, OUTPUT);
    stop_coils();
}

float DCMotor::calc_pwm(float speed, float speed_min, float speed_max, float pwm_min){
    #if defined (__AVR__)
        return (speed-speed_min) * (255-pwm_min) / (speed_max-speed_min) + pwm_min;
    #endif
    #if defined (ESP32)
        int power=LEDC_DUTY_RES;
        return (speed-speed_min) * ( pow(2,power) -1 - pwm_min) / (speed_max-speed_min) + pwm_min;
    #endif
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


