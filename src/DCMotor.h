#ifndef DCMOTOR_H
#define DCMOTOR_H


class DCMotor
{
    public:
        DCMotor(int dir1_pin, int dir2_pin, int pwm_pin);
        virtual ~DCMotor();
        void start(unsigned int pwm_duty);
        void stop();
        void setup();
        void set_coils(bool direction);        
        void short_break();
        void stop_coils();
        void check_coils();
    protected:
        int pin_dir1;
        int pin_dir2;
        int pin_pwm;

    private:
        
};

#endif // DCMOTOR_H
