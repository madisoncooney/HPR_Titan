/*

  Monash High Performance Rocketry SPI Multiplexer
  Adapted from Pololu MinIMU-9 + Arduino AHRS (Attitude and Heading Reference System)

  Contains functions handling the processing and transmission of sensor data recieved by the microcontroller
  to a data transmission shield or computer

  datarate = 115.2 kbd

  //Sensor data types
  T: temperature
  M: magnetometer
  G: gyroscope
  C: gps
  S: strain gauge
  B: barometer
  A: Altitude

*/

// setprecision code
#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision
#include <string>
using namespace std;

//Define classes
class Sensor {

private:
  char SensorType;
  char SensorID;
  int SensorRate;
  string SensorData;
  long SensorTime;

public:
  Sensor (char,char,int);
  void sensorPull (char);

};

//Methods
void Sensor::Sensor (char type,char id,id baud) {

  SensorType = type;
  SensorID = id;
  SensorRate = baud;
  SensorData = "0";
  SensorTime = millis();

};

string Sensor::sensorPull (pullData,pullTime) {

  //Temp sensor
  if (SensorType = 'T') {

    return output;

  };

  //magnetometer
  if (SensorType = 'M') {

  };

  //gyroscope
  if (SensorType = 'G') {

    return output;

  };

  //gps
  if (SensorType = 'C') {

    return output;

  };

  //strain
  if (SensorType = 'S') {

    return output;

  };

  //barometer
  if (SensorType = 'B') {

    return output;

  };

  //altitude
  if (SensorType = 'A') {

    return output;

  };

};

//
//
//
