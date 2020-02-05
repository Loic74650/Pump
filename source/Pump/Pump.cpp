#include "Arduino.h"
#include "Pump.h"

//Constructor
//PumpPin is the Arduino relay output pin number to be switched to start/stop the pump
//TankLevelPin is the Arduino digital input pin number connected to the tank level switch
//Interlockpin is the Arduino digital input number connected to an "interlock". 
//If this input is LOW, pump is stopped and/or cannot start. This is used for instance to stop
//the Orp or pH pumps in case filtration pump is not running
//IsRunningSensorPin is the pin which is checked to know whether the pump is running or not. 
//It can be the same pin as "PumpPin" in case there is no sensor on the pump (pressure, current, etc) which is not as robust. 
//This option is especially useful in the case where the filtration pump is not managed by the Arduino. 
Pump::Pump(uint8_t PumpPin, uint8_t IsRunningSensorPin, uint8_t TankLevelPin=NO_TANK, uint8_t Interlockpin=NO_INTERLOCK)
{
  pumppin = PumpPin;
  isrunningsensorpin = IsRunningSensorPin;
  tanklevelpin = TankLevelPin;
  interlockpin = Interlockpin;
  StartTime = 0;
  LastStartTime = 0;
  StopTime = 0;
  UpTime = 0;        
  UpTimeError = 0;
  MaxUpTime = DefaultMaxUpTime;
}

//Call this in the main loop, for every loop, as often as possible
void Pump::loop()
{
  if(digitalRead(isrunningsensorpin) == PUMP_ON)
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

  if(interlockpin != NO_INTERLOCK)
  {
    if(digitalRead(interlockpin) == INTERLOCK_NOK)
       Stop();
  }
}

//Switch pump ON (if over time was not reached)
bool Pump::Start()
{
  if((digitalRead(isrunningsensorpin) == PUMP_OFF) && !UpTimeError && ((tanklevelpin == NO_TANK) || (digitalRead(tanklevelpin) != TANK_EMPTY)) && ((interlockpin == NO_INTERLOCK) || (digitalRead(interlockpin) == INTERLOCK_OK)))//if((digitalRead(pumppin) == false))
  {
    digitalWrite(pumppin, PUMP_ON);
    StartTime = LastStartTime = millis(); 
    return true; 
  }
  else return false;
}

//Switch pump OFF
bool Pump::Stop()
{
  if(digitalRead(isrunningsensorpin) == PUMP_ON)
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
    MaxUpTime += DefaultMaxUpTime;
    UpTimeError = false;
  }
}

//tank level status
bool Pump::TankLevel()
{
  return digitalRead(tanklevelpin);
}

//interlock status
bool Pump::Interlock()
{
  return digitalRead(interlockpin);
}

//pump status
bool Pump::IsRunning()
{
  return digitalRead(isrunningsensorpin);
}
