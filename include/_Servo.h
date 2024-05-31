/**
   @file _Servo.h
   @author your name (you@domain.com)
   @brief
   @version 0.1
   @date 2024-04-03

   @copyright Copyright (c) 2024

*/

#include "_config.h"
#include <ESP32Servo.h>     //   Lib:  ESP32Servo@^1.2.0
#include <arduino-timer.h>          //  https://github.com/contrem/arduino-timer/blob/master/README.md       https://github.com/khoih-prog/TimerInterrupt

auto StepTimer = timer_create_default(); // create a timer with default settings

int pos = 0;    // variable to store the servo position
int Servo_LoadPwr_Pin       = 12;
int Servo_BatteryLevel_Pin  = 13;
int Servo_CurPvProd_Pin     = 14;
Servo Servo_LoadPwr;       // create servo object to control a servo
Servo Servo_BatteryLevel;  // create servo object to control a servo
Servo Servo_CurPvProd;     // create servo object to control a servo

//Servo test
int _lr = 544;      //< lower range
int _ur = 2500;     //< upper range
//int _TargetPos = 1500;    //< pointer position in range ur - lr
int _deg = 0;      //< pointer position in degrees
int _delay = 20;    //< delay to move 
int _StepSize = 1;        //< stepsize to get to the next position
int _swp = 0;       //< sweep dis/en-able

int _Load_Power_Deg;     //< global: last Load Power from MQTT in degree
int _Battery_level_Deg;  //< global: last Battery Power info from MQTT in degree
int _cur_pv_prod_Deg;    //< global: last cur_pv_prod from MQTT in degree
int _Load_Power_wayPoint    = 0;    //< global: last Load Power way point
int _Battery_level_wayPoint = 0;    //< global: last Battery Power way point
int _cur_pv_prod_wayPoint   = 0;    //< global: last cur_pv_prod way point

boolean boSweep = false;
bool TIMER_Servo_function_to_call(void *argument );
void vSweep(void);

void ServoSetup()
{   // standard 50 hz servo
    Servo_LoadPwr.setPeriodHertz(50);
    Servo_BatteryLevel.setPeriodHertz(50);
    Servo_CurPvProd.setPeriodHertz(50);
    // attaches the servo on configured pin to the servo object
    Servo_LoadPwr.attach(Servo_LoadPwr_Pin, _lr, _ur);
    Servo_BatteryLevel.attach(Servo_BatteryLevel_Pin, _lr, _ur);
    Servo_CurPvProd.attach(Servo_CurPvProd_Pin, _lr, _ur);
    // Allow allocation of all timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    // setup timer with callback function to call.
    StepTimer.every(_delay, TIMER_Servo_function_to_call);
}

void ServoNewLoadPower(int LoadPwr){
    if (LoadPwr < 0)
        _Load_Power_Deg = 0;
    if (LoadPwr <= 100)
        _Load_Power_Deg = map(LoadPwr, 0, 100, 0, 10);
    else if (LoadPwr <= 250)
        _Load_Power_Deg = map(LoadPwr, 100, 250, 10, 40);
    else if (LoadPwr <= 800)
        _Load_Power_Deg = map(LoadPwr, 250, 800, 40, 60);
    else if (LoadPwr <= 1400)
        _Load_Power_Deg = map(LoadPwr, 800, 1400, 60, 90);
    else if (LoadPwr <= 4000)
        _Load_Power_Deg = map(LoadPwr, 1400, 4000, 90, 120);
    else
        _Load_Power_Deg = 120;

#if (ENABLE_UART == 1)
      Serial.print("new LoadPower in deg: ");
      Serial.println(_Load_Power_Deg);
#endif

}

void ServoNewCurPvPower(int PvPwr){
    if (PvPwr < 0)
        _cur_pv_prod_Deg = 120;
    else if (PvPwr <= 100)
        _cur_pv_prod_Deg = map(PvPwr, 0, 100, 120, 87);
    else if (PvPwr <= 500)
        _cur_pv_prod_Deg = map(PvPwr, 100, 500, 86, 66); 
    else if (PvPwr <= 1500)
        _cur_pv_prod_Deg = map(PvPwr, 500, 1500, 65, 45);
    else if (PvPwr <= 1500)
        _cur_pv_prod_Deg = map(PvPwr, 1500, 4000, 44, 25);
    else if (PvPwr <= 11000)
        _cur_pv_prod_Deg = map(PvPwr, 4000, 11000, 24, 0);
    else
        _cur_pv_prod_Deg = 0;
    
#if (ENABLE_UART == 1)
      Serial.print("new PV Power in deg: ");
      Serial.println(_cur_pv_prod_Deg);
#endif
}

void ServoNewBatLvl(int BatLvl){
    if( BatLvl < 0 )
        _Battery_level_Deg = 0;
    else if ( BatLvl > 100 )
        _Battery_level_Deg = 100;
    else
        _Battery_level_Deg = map(BatLvl, 0, 100, 0, 100); // scale it to use it with the servo (value between 0 and 100)
    
#if (ENABLE_UART == 1)
      Serial.print("new BatLvl: ");
      Serial.println(_Battery_level_Deg);
#endif
}

int _iUpdateServoPos(int _TargetPos, int &wayPoint){
    if (wayPoint != _TargetPos){
        int16_t i16Diff = _TargetPos - wayPoint;

        if (i16Diff >= _StepSize)
            wayPoint += _StepSize;
        else if (i16Diff <= -_StepSize)
            wayPoint -= _StepSize;
        else
            wayPoint = _TargetPos;
    }
    return wayPoint;
}

bool TIMER_Servo_function_to_call(void *argument ) {

    bool boRetval = false;

    if (_Load_Power_wayPoint != _Load_Power_Deg){
        Servo_LoadPwr.write(_iUpdateServoPos(_Load_Power_Deg, _Load_Power_wayPoint)); // sets the servo position according to the scaled value
        delay(15);  // waits for the servo to get there
        boRetval = true; // continue
    }
    if (_Battery_level_wayPoint != _Battery_level_Deg){
        Servo_BatteryLevel.write(_iUpdateServoPos(_Battery_level_Deg, _Battery_level_wayPoint)); // sets the servo position according to the scaled value
        delay(15);  // waits for the servo to get there
        boRetval = true; // continue
    }
    if (_cur_pv_prod_wayPoint != _cur_pv_prod_Deg){
        Servo_CurPvProd.write(_iUpdateServoPos(_cur_pv_prod_Deg, _cur_pv_prod_wayPoint)); // sets the servo position according to the scaled value
        delay(15);  // waits for the servo to get there
        Serial.print("PvPwr wayPnt: ");
        Serial.println(_cur_pv_prod_wayPoint);
        boRetval = true; // continue
    }

    return boRetval;    // repeat ? true : false
}

void vServoCyclicTick() {

  //timer2.tick();
  StepTimer.tick<void>(); // avoids ticks() calculation
}


void vNewMqttData(void) {

    // if StepTimer not Started !!! otherwise multiple might be started
    if (StepTimer.empty()) { /* no active tasks */
        StepTimer.every(_delay, TIMER_Servo_function_to_call);
    }
}

void vSweep(void)
{
    for (pos = _lr; pos <= _ur; pos += _StepSize) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        Servo_LoadPwr.write(pos);    // tell servo to go to position in variable 'pos'
        delay(_delay);             // waits 15ms for the servo to reach the position
    }
    for (pos = _ur; pos >= _lr; pos -= _StepSize) { // goes from 180 degrees to 0 degrees
        Servo_LoadPwr.write(pos);    // tell servo to go to position in variable 'pos'
        delay(_delay);             // waits 15ms for the servo to reach the position
    }    
}

#ifdef SERVO_TEST
void Servotest(tenTstServo msgType, int iVal)
{
    switch(msgType){
        case enServo_lr:
            if (iVal > 500){
                _lr = iVal;
                myservo.attach(servoPin, _lr , _ur); // attaches the servo on configured pin to the servo object
            }
            break;
        case enServo_ur:
            if (iVal > 500){
                _ur = iVal;
                myservo.attach(servoPin, _lr , _ur); // attaches the servo on configured pin to the servo object
            }
            break;
        case enServo_pos:
            _pos = iVal;
            break;
        case enServo_deg:
            _deg = iVal;
            break;
        case enServo_delay:
            _delay = iVal;
            break;
        case enServo_sz:
            _sz = iVal;
            break;
        case enServo_swp:
            boSweep = iVal ? true: false;
        default:
            break;
    };
    vNewMqttData();
}
#endif
