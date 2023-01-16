#ifndef PUMP_H
#define PUMP_H

//pt1000 sensors
class sensor
{
private:
     unsigned u8int_tin_pin;
    
public:    
    sensor(unsigned u8int_t);
    unsigned u16int_t read();
    float voltage();
    float temperature();
};

//actor (solid state relay)
class relay
{
private:
    unsigned u8int_t out_pin;
    bool status;

public:
    relay(unsigned u8int_t);
    void on();
    void off();
    bool sense();
    void setStatus(bool);
    bool getStatus();
};

//status leds
class led
{
private:
    unsigned u8int_t out_pin;
public:
    led(unsigned u8int_t);
    void on();
    void off();
    void blink(unsigned u8int_t);
};

//thermostat
class thermostat
{
private:
    unsigned u8int_t in_pin;
    bool isEnable;
public:
    thermostat(unsigned u8int_t);
    bool check();
};

//valve (NR230)
class valve
{
private:
    unsigned u8int_t pin_open;
    unsigned u8int_t pin_close;
    
public:
    valve (unsigned u8int_t, unsigned u8int_t);
    void open(unsigned u8int_t);
    void close(unsigned u8int_t);
};


#endif