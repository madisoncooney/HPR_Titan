#include <NMEAGPS.h>
#include <Wire.h>
#include <LSM6.h>
#include <LIS3MDL.h>
#include <LPS.h>
#include <Streamers.h>

LSM6 gyro_acc;
LIS3MDL mag;
LPS ps;

//print accelerometer data?
#define PRINT_ACC 0

int IMU[8];
float PressMb;
float tempC;
#define RFDPort Serial1
#define gpsPort Serial2

#define GPS_PORT_NAME "Serial1"
#define DEBUG_PORT Serial

#define TXGPS 18
#define RXGPS 19
#define TXRFD 16
#define RXRFD 17




//------------------------------------------------------------
// This object parses received characters
//   into the gps.fix() data structure

static NMEAGPS  gps;

//------------------------------------------------------------
//  Define a set of GPS fix information.  It will
//  hold on to the various pieces as they are received from
//  an RMC sentence.  It can be used anywhere in your sketch.

static gps_fix  fix;


int ledPin = 13;

void setup()
{
  delay(2000);
  DEBUG_PORT.begin(115200);


  i2cSetup();
  enableGyroAcc();
  enableMag();
  enableBaro();

  pinMode(ledPin, OUTPUT);    //define LED pin
  digitalWrite(ledPin, HIGH); // Turn on LED pin

  while (!DEBUG_PORT)
    ;

  DEBUG_PORT.print( F("NMEA.INO: started\n") );
  DEBUG_PORT.print( F("  fix object size = ") );
  DEBUG_PORT.println( sizeof(gps.fix()) );
  DEBUG_PORT.print( F("  gps object size = ") );
  DEBUG_PORT.println( sizeof(gps) );
  DEBUG_PORT.println( F("Looking for GPS device on " GPS_PORT_NAME) );


#ifndef NMEAGPS_RECOGNIZE_ALL
#error You must define NMEAGPS_RECOGNIZE_ALL in NMEAGPS_cfg.h!
#endif

#ifdef NMEAGPS_INTERRUPT_PROCESSING
#error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

#if !defined( NMEAGPS_PARSE_GGA ) & !defined( NMEAGPS_PARSE_GLL ) & \
      !defined( NMEAGPS_PARSE_GSA ) & !defined( NMEAGPS_PARSE_GSV ) & \
      !defined( NMEAGPS_PARSE_RMC ) & !defined( NMEAGPS_PARSE_VTG ) & \
      !defined( NMEAGPS_PARSE_ZDA ) & !defined( NMEAGPS_PARSE_GST )

  DEBUG_PORT.println( F("\nWARNING: No NMEA sentences are enabled: no fix data will be displayed.") );

#else
  if (gps.merging == NMEAGPS::NO_MERGING) {
    DEBUG_PORT.print  ( F("\nWARNING: displaying data from ") );
    DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
    DEBUG_PORT.print  ( F(" sentences ONLY, and only if ") );
    DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
    DEBUG_PORT.println( F(" is enabled.\n"
                          "  Other sentences may be parsed, but their data will not be displayed.") );
  }
#endif

  DEBUG_PORT.print  ( F("\nGPS quiet time is assumed to begin after a ") );
  DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
  DEBUG_PORT.println( F(" sentence is received.\n"
                        "  You should confirm this with NMEAorder.ino\n") );

  trace_header( DEBUG_PORT );
  DEBUG_PORT.flush();

  RFDPort.begin( 115200, SERIAL_8N1, TXRFD, RXRFD );
  delay(100);

  gpsSetup();
}

void loop()
{
  GPSloop();
}


//----------------------------------------------------------------
//  This function gets called about once per second, during the GPS
//  quiet time.  It's the best place to do anything that might take
//  a while: print a bunch of things, write to SD, send an SMS, etc.
//
//  By doing the "hard" work during the quiet time, the CPU can get back to
//  reading the GPS chars as they come in, so that no chars are lost.
static void doSomeWork()
{
  // Print all the things!

  trace_all( DEBUG_PORT, gps, fix );
  gpsprint();
}


//------------------------------------
//  This is the main GPS parsing loop.

static void GPSloop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
    doSomeWork();
  }
  if (PRINT_ACC == 1) {
    //if (Wire.available()) {
    Acc_Print();
    //}
  }


} // GPSloop


void Acc_Print()
{

  gyro_acc.readGyro();
  gyro_acc.readAcc();
  mag.read();

  PressMb  = ps.readPressureMillibars();
  tempC = ps.readTemperatureC();

  IMU[0] = gyro_acc.g.x;
  IMU[1] = gyro_acc.g.y;
  IMU[2] = gyro_acc.g.z;
  IMU[3] = gyro_acc.a.x; //>> 4; // shift right 4 bits to use 12-bit representation (1 g = 256)
  IMU[4] = gyro_acc.a.y; //>> 4;
  IMU[5] = gyro_acc.a.z; //>> 4;
  IMU[6] = mag.m.x;
  IMU[7] = mag.m.y;
  IMU[8] = mag.m.z;
  //

  //RFDPort.print("IMU:");
  //RFDPort.println(IMU[0]);

  printdata(RFDPort);


}

void printdata(Stream &myPort ) {

  myPort.write("IMU:");

  //for (int i = 0; i < (sizeof(IMU) / sizeof(IMU[0])); i++) {
  for (int i = 0; i < 8; i++) {
    myPort.print(IMU[i]);
    myPort.print(",");
  }
  myPort.print(PressMb);
  myPort.print(",");
  myPort.print(tempC);
  myPort.println();

}

void gpsprint() {
  if (fix.valid.location) {
  RFDPort.print("GPS:");
  //trace_all( RFDPort, gps, fix);
  RFDPort.print(fix.dateTime.date);
  RFDPort.print(fix.dateTime.month);
  RFDPort.print(fix.dateTime.year);
  RFDPort.print("-");
  RFDPort.print(fix.dateTime.hours);
  RFDPort.print(fix.dateTime.minutes);
  RFDPort.print(fix.dateTime.seconds);
  RFDPort.print(fix.dateTime_ms());
  RFDPort.print(",");
  RFDPort.print(fix.altitude_cm());
  RFDPort.print(",");
  RFDPort.print(fix.latitudeL());
  RFDPort.print(",");
  RFDPort.print(fix.longitudeL());
  RFDPort.print(",");
  RFDPort.print(fix.speed_mkn());
  RFDPort.print(",");
  RFDPort.print(fix.heading_cd());
  RFDPort.println();
  }
}
