#include "Arduino.h"
#include "Pump.h"

//Constructor
//PumpPin is the Arduino relay output pin number to be switched to start/stop the pump
//TankLevelPin is the Arduino digital input pin number connected to the tank level switch
Pump::Pump(uint8_t PumpPin, uint8_t TankLevelPin=NO_TANK)
//Pump::Pump(uint8_t PumpPin, uint8_t TankLevelPin)
{
  pumppin = PumpPin;
  tanklevelpin = TankLevelPin;
  StartTime = 0;
  StopTime = 0;
  UpTime = 0;
  MaxUpTime = 30*60*1000; //default value is 30mins        
  UpTimeError = 0;
}

//Call this in the main loop, for every loop, as often as possible
void Pump::loop()
{
  if(digitalRead(pumppin) == PUMP_ON)
  {
    UpTime += millis() - StartTime;
    StartTime = millis();
  }

  if((MaxUpTime > 0) && (UpTime >= MaxUpTime))
  {
    Stop();
    UpTimeError = true;
  }

  if(tanklevelpin != NO_TANK)
  {
    if(digitalRead(tanklevelpin) == TANK_EMPTY)
       Stop();
  }
}

//Switch pump ON (if over time was not reached)
bool Pump::Start()
{
  if((digitalRead(pumppin) == PUMP_OFF) && !UpTimeError && ((tanklevelpin == NO_TANK) || (digitalRead(tanklevelpin) != TANK_EMPTY)))//if((digitalRead(pumppin) == false))
  {
    digitalWrite(pumppin, PUMP_ON);
    StartTime = millis(); 
    return true; 
  }
  else return false;
}

//Switch pump OFF
bool Pump::Stop()
{
  if(digitalRead(pumppin) == PUMP_ON)
  {
    digitalWrite(pumppin, PUMP_OFF);
    UpTime += millis() - StartTime; 
    return true;
  }
  else return false;
}

//Reset the tracking of running time
//This is typically called every day at midnight
void Pump::ResetUpTime()
{
  StartTime = 0;
  StopTime = 0;
  UpTime = 0;
}

//Set a maximum running time (in millisecs) per day (in case ResetUpTime() is called once per day)
//Once reached, pump is stopped and "UpTimeError" error flag is raised
//Set "Max" to 0 to disable limit
void Pump::SetMaxUpTime(unsigned long Max)
{
  MaxUpTime = Max;
}

//Clear "UpTimeError" error flag and allow the pump to run for an extra 30mins
void Pump::ClearErrors()
{
  if(UpTimeError)
  {
    MaxUpTime += 30*60*1000;
    UpTimeError = false;
  }
}

//tank level status
bool Pump::TankLevel()
{
  return digitalRead(tanklevelpin);
}

//pump status
bool Pump::IsRunning()
{
  return digitalRead(pumppin);
}
