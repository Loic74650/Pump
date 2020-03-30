/*
            Pump - a simple library to handle home-pool filtration and persitaltic pumps
                 (c) Loic74 <loic74650@gmail.com> 2017-2020
Features: 

- keeps track of running time
- keeps track of Tank Levels
- set max running time limit

NB: all timings are in milliseconds
*/

#ifndef PUMP_h
#define PUMP_h
#define PUMP_VERSION "1.0.0"

//Constants used in some of the functions below
#define PUMP_ON  1
#define PUMP_OFF 0
#define TANK_FULL  1
#define TANK_EMPTY 0
#define INTERLOCK_OK  1
#define INTERLOCK_NOK 0
#define NO_TANK 255 
#define NO_INTERLOCK 255  

#define DefaultMaxUpTime 30*60*1000 //default value is 30mins  
 
class Pump{
  public:

    Pump(uint8_t, uint8_t, uint8_t, uint8_t, double, double);
    void loop();
    bool Start();
    bool Stop();
    bool IsRunning();
    bool TankLevel();
    double GetTankUsage();    
    void SetTankVolume(double Volume);
    void SetFlowRate(double FlowRate);
    bool Interlock();
    void SetMaxUpTime(unsigned long Max);
    void ResetUpTime();

    void ClearErrors();
    
    unsigned long UpTime;
    unsigned long MaxUpTime;
    bool UpTimeError;
    unsigned long StartTime;
    unsigned long LastStartTime;
    unsigned long StopTime; 
    double flowrate, tankvolume;          
  private:
     
    uint8_t pumppin; 
    uint8_t isrunningsensorpin;
    uint8_t tanklevelpin;
    uint8_t interlockpin;

};
#endif
