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

//List of sensors
std::vector<Sensor> SensorList = {};

Sensor s_Magnetometer("0",3);
