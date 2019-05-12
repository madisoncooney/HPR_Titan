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
#define PRINT_ACC 1


#define RFDPort Serial1
#define gpsPort Serial2

#define GPS_PORT_NAME "Serial1"
#define DEBUG_PORT Serial

#define TXGPS 18
#define RXGPS 19
#define TXRFD 16
#define RXRFD 17

static NMEAGPS  gps;
static gps_fix  fix;
int ledPin = 13;
int IMU[8];
float PressMb;
float tempC;

void setup() {
  // put your setup code here, to run once:
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

  delay(1000);

  xTaskCreate(
    gpsTask,
    "GPS TASK",
    10000,
    NULL,
    3,
    NULL);

  xTaskCreate(
    accTask,
    "ACC TASK",
    10000,
    NULL,
    2,
    NULL);

//  xTaskCreate(
//    printTask,
//    "PRINT TASK",
//    10000,
//    NULL,
//    1,
//    NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void gpsTask( void * parameter ) {
  while (gps.available( gpsPort )) {
    fix = gps.read();
    trace_all( RFDPort, gps, fix);
  }
}

void accTask( void * parameter ) {
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

  RFDPort.print("IMU:");

  //for (int i = 0; i < (sizeof(IMU) / sizeof(IMU[0])); i++) {
  for (int i = 0; i < 8; i++) {
    RFDPort.print(IMU[i]);
    RFDPort.print(",");
  }
  RFDPort.print(PressMb);
  RFDPort.print(",");
  RFDPort.print(tempC);
  RFDPort.println();
  
}
