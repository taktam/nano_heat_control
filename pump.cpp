#include "pump.h"
#include <Arduino.h>
#include <Thermistor.h>
#include <NTC_Thermistor.h>

long valveTime = 200000;


/**
 * Constructor for sensor
 *
 * @param analog_in pin number for the sense
 */
sensor::sensor(byte analog_in)
{
    in_pin = analog_in;
}

/**
 * Read the sensor as byte
 *
 * @return sensor value (byte, 0-1023) 
 */
unsigned u16int_t sensor::read()
{
    return analogRead(in_pin);
}

/**
 * Read the sensor as volt
 *
 * @return sensor value volt(float) 
 */
float sensor::voltage()
{
    return (read()*5.0/1023);
}

/**
 * Read the temerature sensor with NTC_Thermistor Library as Celsius
 *
 * @return temp sensor value celsius (double) 
 */
float sensor::temperature()
{
    Thermistor* thermistor;
    thermistor = new NTC_Thermistor(
    in_pin,
    4700,        //REFERENCE_RESISTANCE
    5000,        //NOMINAL_RESISTANCE
    25,          //NOMINAL_TEMPERATURE
    4200         //B_VALUE
  );
  const double celsius = thermistor->readCelsius();
  delete thermistor;
  return celsius;
}

/**
 * Constructor for relay
 *
 * @param digital_out pin number for the relay
 */
relay::relay(unsigned u16int_t digital_out)
{
    out_pin = digital_out; 
    pinMode(out_pin, OUTPUT);
    digitalWrite(out_pin, LOW);
    setStatus(false);
}

/**
 * Relay on
 *
 */
void relay::on()
{
    pinMode(out_pin, OUTPUT);
    digitalWrite(out_pin, HIGH);
    setStatus(true);
    //delay(1000);
}

/**
 * Relay off
 *
 */
void relay::off()
{
    pinMode(out_pin, OUTPUT);
    digitalWrite(out_pin, LOW);
    setStatus(false);
}

/**
 * Read relay status 
 *
 * @param none
 * @return none
 */
bool relay::sense()
{
    pinMode(out_pin, INPUT);
    bool isON;
    if ((digitalRead(out_pin))==true)
    {
        isON = true;
    }
    else
    {
        isON = false;
    }
    pinMode(out_pin, OUTPUT);
    return isON;
}

/**
 * Set relay status 
 *
 * @param stat relay status (bool)
 */
void relay::setStatus (bool stat)
{
    status = stat;
}

/**
 * Get relay status 
 *
 * @return relay status (bool)
 */
bool relay::getStatus()
{
    return status;
}

/**
 * LED constructor
 *
 * @param digital_out LED pin (int)
 */
led::led(uint16t digital_out)
{
    out_pin = digital_out;
    pinMode (out_pin, OUTPUT);
    digitalWrite(out_pin, LOW);
}

/**
 * Switch LED on
 *
 */
void led::on()
{
    digitalWrite(out_pin, HIGH);
}

/**
 * Switch LED off
 *
 */
void led::off()
{
    digitalWrite(out_pin, LOW);
}

/**
 * Blink the LED
 *
 * @param nr number of flashes
 */
void led::blink(unsigned u16int_t nr)
{
    unsigned u16int_t delay_ms = 300;
    for (unsigned u16int_t i = 0; i < nr; i++)
    {
        digitalWrite(out_pin, HIGH);
        delay (delay_ms);
        digitalWrite(out_pin, LOW);
        delay (delay_ms);
    }
}

/**
 * Constructor for thermostat
 *
 * @param pin pin number for the thermostat
 */
thermostat::thermostat(unsigned u8int_t)
{
    in_pin = pin;
    pinMode(in_pin, INPUT_PULLUP);
    isEnable = false;
}

/**
 * check thermostat status
 *
 */
bool thermostat::check()
{
    return !digitalRead(in_pin);
}

/**
 * Valve constructor
 *
 * @param pin_o open pin
 * @param pin_c close pin
 */
valve::valve(unsigned u8int_t pin_o, unsigned u8int_t pin_c)
{
    pin_open = pin_o;
    pin_close = pin_c;
    pinMode(pin_open, OUTPUT);
    pinMode(pin_close, OUTPUT);
    digitalWrite(pin_open, LOW);
    digitalWrite(pin_open, LOW);
}

/**
 * Open valve
 * @param 0 = FULL, 1 = HALF, 2 = QUARTER, 3 = UNIT
 */
void valve::open(unsigned u18int_t Cycle)
{
    long time_ms = 0;

    
    
    switch (Cycle)
    {
    case 0:
        time_ms = valveTime;
        break;

    case 1:
        time_ms = valveTime/2;
        break;

    case 2:
        time_ms = valveTime/4;
        break;

    case 3:
        time_ms = valveTime/8;
        break;
    
    
    default:
        break;
    }

    Serial.print ("Opening of the valve: ");
    Serial.print (time_ms / 1000);
    Serial.println(" sec");
    long startMillis = millis();
    digitalWrite(pin_close, LOW);
    digitalWrite(pin_open, HIGH);
    while(true)
    {
        if (millis()-startMillis >= time_ms)
        {
            break;
        }
        Serial.print(time_ms/1000-((millis()-startMillis))/1000);
        Serial.println(" sec");
        delay (10000);
    }
    
    digitalWrite(pin_open, LOW);
}

/**
 * Close valve
 * @param @param 0 = FULL, 1 = HALF, 2 = QUARTER, 3 = UNIT
 */
void valve::close(unsigned u8int_t Cycle)
{
    long time_ms = 0;
    
    switch (Cycle)
    {
    case 0:
        time_ms = valveTime;
        break;

    case 1:
        time_ms = valveTime/2;
        break;

    case 2:
        time_ms = valveTime/4;
        break;

    case 3:
        time_ms = valveTime/8;
        break;
    
    
    default:
        break;
    }

    Serial.print ("Closing of the valve: ");
    Serial.print (time_ms/1000);
    Serial.println(" sec");
    long startMillis = millis();
    digitalWrite(pin_open, LOW);
    digitalWrite(pin_close, HIGH);
    while(true)
    {
        if (millis()-startMillis >= time_ms)
        {
            break;
        }
        Serial.println(time_ms/1000-(millis()-startMillis)/1000);
        delay (10000);
    }

    digitalWrite(pin_close, LOW);
}
