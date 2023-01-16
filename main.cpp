/*
 *
 - This project uses an Arduino Nano controller to analyze the behavior of water heating elements.
 - The data is collected, processed and documented to observe the performance of the heater and the changes in water temperature. 
 - The loop() function contains a simulation of the heating process. 
 - This project can be used to measure, document and understand the behavior of the heating element.
  *
 /* by Taki 2023
 */

#include <Arduino.h>
#include "pump.h"

//Flags & times, global variables
bool valveIsOpen = false;
bool pumpIsOn = false;
bool thermostatEnable = false;
unsigned long initMS = 0;
float preTemp = 0;
byte valveCloseStatus = 0;


//******************************* HARDWARE INIT ***********************************

sensor *sen0 = new sensor(A2);    
thermostat *therm = new thermostat(2);
relay *pump = new relay (3);
valve *nr230 = new valve(5,4);
relay *onS0 = new relay(9);
led *LED = new led(6);


//******************************* BASIC FUNCTIONS ***********************************

/**
 * Write the temperature out the serial port
 *
 */
void showTemp()
{
  Serial.print("Temp:\t");
  Serial.print(sen0->temperature());
  Serial.println(" °C"); 
}

/**
 * Switch on the pump
 *
 */
void pumpON()
{
  Serial.println ("Turning on the pump...");
  pump->on();
  if ((pump->getStatus())==true)
  {
    Serial.println("The pump is on");
  }
  else
  {
    Serial.println("Pump switch-on error");
  }
}

/**
 * Switch off the pump
 *
 */
void pumpOFF()
{
  Serial.println ("Turning off the pump...");
  pump->off();
  if ((pump->getStatus())==false)
  {
    Serial.println("The pump is off");
  }
  else
  {
    Serial.println("Pump switch-off error");
  }
}

/**
 * Check the temperature difference
 *
 */
void chkChange(float ref)
{
  if ((sen0->temperature())>(ref+0.3))
  {
    Serial.println("Rising temperature");
  }

  if ((sen0->temperature())<(ref-0.3))
  {
    Serial.println("Falling temperature");
  }
  
}


//******************************* S E T U P ***********************************

void setup() {
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2));
  ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);
  ADMUX = bit (REFS0) | bit (REFS1);
  Serial.begin(9600);
  onS0->on();
  

  Serial.println("Setup run...");
  nr230->open(0);
  valveIsOpen = true;
  Serial.println("The valve is open");
  pump->on();
  pumpIsOn = true;
  Serial.println("The pump is on");
  Serial.println("Pump test run... (5sec)");
  delay(5000);
  showTemp();
  Serial.println("-----------------------------------");
  Serial.println("Test completed, controller start...");
  Serial.println("-----------------------------------");
  
}

//******************************** L O O P ************************************

void loop() {

  if (valveCloseStatus>7)
    {
      Serial.println("The valve is completely closed, the pump turns off...");
      valveIsOpen = false;
      pump->off();
      pumpIsOn = false;
    }

  chkChange(preTemp);
  float refTemp = 0;

  if (sen0->temperature()>=40)
  {
    refTemp = sen0->temperature();

    //The water is too hot
    if (sen0->temperature()>65.0)
    {
      //pump->off();
      //pumpIsOn = false;
      if (valveIsOpen == true)
      {
        if (valveCloseStatus < 6)
        {
          nr230->close(3);
          valveCloseStatus = valveCloseStatus + 1;
        }
        delay (60000);
      }

    }

    if (valveIsOpen==false)
    {
      if (sen0->temperature()>65.0)
      {
        Serial.println("Water temperature above 65°, valve opening to 45°");
        nr230->open(1);
        valveIsOpen = true;
        valveCloseStatus = 4;
      }
      else
      {
        Serial.println("Valve opening to 90°");
        nr230->open(0);
        valveIsOpen = true;
        valveCloseStatus = 0;
      }
      
      
      //The pump starts...
      pump->on();
      pumpIsOn = true;
    }
    
    
    //Watching the thermostat
    
    //The thermostat is OFF
    if (therm->check() == false)
    {
      if (valveCloseStatus>5)
      {
        if (pumpIsOn == true)
        {
          Serial.println("The valve is 6/8 unit closed, turn the pump off...");
          pump->off();
          pumpIsOn = false;
        }
        else
        {
          Serial.println("The valve is closed (6/8), the is pump off");
          showTemp();
          Serial.println("-----------------------------------");
          delay(1000);
        }
        
      }

      else
      {
        Serial.println("The thermostat is off, close the valve with one unit...");
        nr230->close(3);
        valveCloseStatus = valveCloseStatus + 1;
        showTemp();
        Serial.println("Wait... (35 sec)");
        delay(35000); 

      }
      
      
    }

    //the thermostat is ON
    else
    {
      if (valveCloseStatus != 0)
      {
        if (pumpIsOn==false)
        {
          pump->on();
        }

        if (valveCloseStatus >= 2)
        {
          Serial.println("The thermostat is on, open the valve with two unit...");
          nr230->open(2);
          valveCloseStatus = valveCloseStatus -2;
        }

        if (valveCloseStatus == 1)
        {
          Serial.println("The thermostat is on, open the valve with one unit...");
          nr230->open(3);
          valveCloseStatus = 0;
        }
      
      }
      else
      {
        showTemp();
        Serial.println("Wait... (35 sec)");
        delay(35000);
      }
      
      showTemp();
      Serial.println("Wait... (35 sec)");
      delay(35000);   
    }

  chkChange(refTemp);
    
  }

  //Too low water temperature...

  else
  {
    if(therm->check()==true)
    {
      nr230->open(2);
      pump->on();
    }
    /*

TEST

    //it is switched off
    if ((pumpIsOn==false)&&(valveIsOpen==false))
    {
      Serial.println("The water temperature is too low.");
      Serial.println("The valve is closed and the pump is switched off.");
      initMS = millis();
      refTemp = sen0->temperature();
      while (true)
      {
        unsigned long delayTime = 30000;  
        if ((millis()-initMS>delayTime))  
        {
          break;
        }
        showTemp();
        chkChange(refTemp);
        delay(1000);
      }    
    }
    //is not switched off
    else
    {
      refTemp = sen0->temperature();
      Serial.println("Too low water temperature");
      pumpOFF();
      pumpIsOn = false;
      Serial.println("Closing of the valve...");
      nr230->close(0);
      valveIsOpen = false;
      initMS = millis();
      while (true)
      {
        unsigned long delayTime = 30000; 
        if ((millis()-initMS>delayTime))  
        {
          break;
        }
        showTemp();
        chkChange(refTemp);
        delay(1000);
        }   
    }  
    */
  }

preTemp = sen0->temperature();
}