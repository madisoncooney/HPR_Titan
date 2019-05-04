/*

  NoFAT Multiplexer
  Adapted from Pololu MinIMU-9 + Arduino AHRS (Attitude and Heading Reference System)

  Contains functions handling the processing and transmission of sensor data recieved by the microcontroller
  to a data transmission shield or computer

*/
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <sstream>
#include <bitset>
#include <cstdlib>
#include <cstring>
#include <vector>
using namespace std;

//List of sensors
std::vector<Sensor> SensorList = {};

void NF_tx_loop() //Main Loop
{

  long timer = 0;   //general purpose timer
  long timer_old;
  unsigned int counter = 0;

  if((millis()-timer)>=BCLK)  // Main loop runs at 50Hz, BCLK = 20
  {
    counter++;
    timer_old = timer;
    timer=millis();
    if (timer>timer_old)
    NF_tx_DCMtime(timer_old,timer);

    //Reads the clock for each sensor and checks whether it should send data
    for (auto sensor = SensorList.begin(); sensor != SensorList.end(); ++sensor)
    {
      if (timer % (sensor.wait) == 0)
      {
        //Extract data from sensor
        void sensor.apiRead();

        //Transmit from all feeds
        for (int i = 0; i < sensor.feeds; i++) {

          void sensor.i.transmit();

        }
      }
    }
  }
}

void NF_tx_DCMtime(int t1, int t2)
{
  if (t2>t1)
  {
    G_Dt = (t2-t1)/1000.0;    // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
    if (G_Dt > 0.2)
      G_Dt = 0; // ignore integration times over 200 ms
  }
  else
    G_Dt = 0;
};
