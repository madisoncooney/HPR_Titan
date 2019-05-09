#include <NMEAGPS.h>
#include <Wire.h>

#include <LSM6.h>
#include <LIS3MDL.h>
#include <LPS.h>

LSM6 gyro_acc;
LIS3MDL mag;
LPS ps;

//print accelerometer data?
#define PRINT_ACC 1

float IMU[10];
//======================================================================
//  Program: NMEA.ino
//
//  Description:  This program uses the fix-oriented methods available() and
//    read() to handle complete fix structures.
//
//    When the last character of the LAST_SENTENCE_IN_INTERVAL (see NMEAGPS_cfg.h)
//    is decoded, a completed fix structure becomes available and is returned
//    from read().  The new fix is saved the 'fix' structure, and can be used
//    anywhere, at any time.
//
//    If no messages are enabled in NMEAGPS_cfg.h, or
//    no 'gps_fix' members are enabled in GPSfix_cfg.h, no information will be
//    parsed, copied or printed.
//
//  Prerequisites:
//     1) Your GPS device has been correctly powered.
//          Be careful when connecting 3.3V devices.
//     2) Your GPS device is correctly connected to an Arduino serial port.
//          See GPSport.h for the default connections.
//     3) You know the default baud rate of your GPS device.
//          If 9600 does not work, use NMEAdiagnostic.ino to
//          scan for the correct baud rate.
//     4) LAST_SENTENCE_IN_INTERVAL is defined to be the sentence that is
//          sent *last* in each update interval (usually once per second).
//          The default is NMEAGPS::NMEA_RMC (see NMEAGPS_cfg.h).  Other
//          programs may need to use the sentence identified by NMEAorder.ino.
//     5) NMEAGPS_RECOGNIZE_ALL is defined in NMEAGPS_cfg.h
//
//  'Serial' is for debug output to the Serial Monitor window.
//
//  License:
//    Copyright (C) 2014-2017, SlashDevin
//
//    This file is part of NeoGPS
//
//    NeoGPS is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    NeoGPS is distributed in the hope that it will be useful,
//    but WITHOUT IMUY WARRIMUTY; without even the implied warranty of
//    MERCHIMUTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with NeoGPS.  If not, see <http://www.gnu.org/licenses/>.
//
//======================================================================

//-------------------------------------------------------------------------
//  The GPSport.h include file tries to choose a default serial port
//  for the GPS device.  If you know which serial port you want to use,
//  edit the GPSport.h file.

#include <GPSport.h>

//------------------------------------------------------------
// For the NeoGPS example programs, "Streamers" is common set
//   of printing and formatting routines for GPS data, in a
//   Comma-Separated Values text format (aka CSV).  The CSV
//   data will be printed to the "debug output device".
// If you don't need these formatters, simply delete this section.

#include <Streamers.h>

//------------------------------------------------------------
// This object parses received characters
//   into the gps.fix() data structure

static NMEAGPS  gps;

//------------------------------------------------------------
//  Define a set of GPS fix information.  It will
//  hold on to the various pieces as they are received from
//  an RMC sentence.  It can be used anywhere in your sketch.

static gps_fix  fix;

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

  DEBUG_PORT.print("GPS:");
  trace_all( DEBUG_PORT, gps, fix );
} // doSomeWork

//------------------------------------
//  This is the main GPS parsing loop.

static void GPSloop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();


    doSomeWork();
  }
#if PRINT_ACC == 1
  Acc_Print();
#endif
} // GPSloop

//--------------------------

void setup()
{
  DEBUG_PORT.begin(115200);
  Wire.begin();

  gyro_acc.init();
  gyro_acc.enableDefault();

  mag.init();
  mag.enableDefault();

  ps.init();
  ps.enableDefault();

  gyro_acc.writeReg(LSM6::CTRL2_G, 0x4C); // 104 Hz, 2000 dps full scale

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


  gpsPort.begin( 9600 );
  delay(100);
  gpsPort.print("$PMTK251,38400*27\r\n");
  delay(100);
  gpsPort.begin( 38400 );
  delay(100);
  gpsPort.print("$PMTK220,100*2F\r\n");
  delay(100);
  gpsPort.print("PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n");
  delay(100);
}

//--------------------------

void loop()
{
  GPSloop();

}

void Acc_Print()
{
  gyro_acc.readGyro();
  IMU[0] = gyro_acc.g.x;
  IMU[1] = gyro_acc.g.y;
  IMU[2] = gyro_acc.g.z;
  gyro_acc.readAcc();
  IMU[3] = gyro_acc.a.x >> 4; // shift right 4 bits to use 12-bit representation (1 g = 256)
  IMU[4] = gyro_acc.a.y >> 4;
  IMU[5] = gyro_acc.a.z >> 4;
  IMU[6]  = ps.readPressureMillibars();
  IMU[7] = ps.readTemperatureC();
  mag.read();
  IMU[8] = mag.m.x;
  IMU[9] = mag.m.y;
  IMU[10] = mag.m.z;

  DEBUG_PORT.print("IMU:");
  DEBUG_PORT.print(IMU[0]);
  DEBUG_PORT.print(",");
  DEBUG_PORT.print(IMU[1]);
  DEBUG_PORT.print(",");
  DEBUG_PORT.print(IMU[2]);
  DEBUG_PORT.print(",");
  DEBUG_PORT.print(IMU[3]);
  DEBUG_PORT.print(",");
  DEBUG_PORT.print(IMU[4]);
  DEBUG_PORT.print(",");
  DEBUG_PORT.print(IMU[5]);
  DEBUG_PORT.print(",");
  DEBUG_PORT.print(IMU[6]);
  DEBUG_PORT.print(",");
  DEBUG_PORT.print(IMU[7]);
  DEBUG_PORT.print(",");
  DEBUG_PORT.print(IMU[8]);
  DEBUG_PORT.print(",");
  DEBUG_PORT.print(IMU[9]);
  DEBUG_PORT.print(",");
  DEBUG_PORT.print(IMU[10]);

  DEBUG_PORT.println();


}

void gyro_x() {

  return (static_cast<float>(gyro_acc.g.x));

}

void gyro_y() {

  return (static_cast<float>(gyro_acc.g.y));

}

void gyro_z() {

  return (static_cast<float>(gyro_acc.g.z));

}
