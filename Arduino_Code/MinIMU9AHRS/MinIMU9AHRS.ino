  //FRAMES
  //NF data frame
  struct NFFrameData
  {

    byte A;
    byte B;
    byte C;
    byte D;

  };

  MinIMU-9-Arduino-AHRS
  Pololu MinIMU-9 + Arduino AHRS (Attitude and Heading Reference System)

  Copyright (c) 2011-2016 Pololu Corporation.
  http://www.pololu.com/

  MinIMU-9-Arduino-AHRS is based on sf9domahrs by Doug Weibel and Jose Julio:
  http://code.google.com/p/sf9domahrs/

  sf9domahrs is based on ArduIMU v1.5 by Jordi Munoz and William Premerlani, Jose
  Julio and Doug Weibel:
  http://code.google.com/p/ardu-imu/

  MinIMU-9-Arduino-AHRS is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your option)
  any later version.

  MinIMU-9-Arduino-AHRS is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
  more details.

  You should have received a copy of the GNU Lesser General Public License along
  with MinIMU-9-Arduino-AHRS. If not, see <http://www.gnu.org/licenses/>.

  int toDeci(char *str,int base) {

    int len = strlen(str);
    int power = 1; // Initialize power of base
    int num = 0;  // Initialize result
    int i;

// Uncomment the below line to use this axis definition:
// X axis pointing forward
// Y axis pointing to the right
// and Z axis pointing down.
// Positive pitch : nose up
// Positive roll : right wing down
// Positive yaw : clockwise
int SENSOR_SIGN[9] = {1, 1, 1, -1, -1, -1, 1, 1, 1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer
// Uncomment the below line to use this axis definition:
// X axis pointing forward
// Y axis pointing to the left
// and Z axis pointing up.
// Positive pitch : nose down
// Positive roll : right wing down
// Positive yaw : counterclockwise
//int SENSOR_SIGN[9] = {1,-1,-1,-1,1,1,1,-1,-1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer

      num += val(str[i]) * power;
      power = power * base;

#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

    return num;

  }

  //NIBBLE OPERATORS
  //Decimal to 4-bit nibble (VALUES: 0-15)
  byte NF_decimalToNibble(int n) {

    byte nibble;

    long long binaryNumber = 0;
    int remainder = 0, i = 1, step = 0, number_of_digits = 0;

    while (n!=0)
    {
      remainder = n%2;
      n /= 2;
      binaryNumber += remainder*i;
      bitWrite(nibble,step,remainder);
      i *= 10;
      step += 1;
    }

    return nibble;

#define PRINT_DCM 1    //Will print the whole direction cosine matrix
#define PRINT_ANALOGS 1 //Will print the analog raw data
#define PRINT_EULER 1   //Will print the Euler angles Roll, Pitch and Yaw
#define PRINT_PS 1      //Will print the pressure, tempurature and altitude
#define PRINT_GPS 1      //Will print the GPS

  //Joins two 4-bit nibbles
  byte NF_joinNibble(byte n1, byte n2) {

float G_Dt = 0.02;  // Integration time (DCM algorithm)  We will run the integration loop at 50Hz if possible

long timer = 0; //general purpuse timer
long timer_old;
long timer24 = 0; //Second timer used to print values
int AN[6]; //array that stores the gyro and accelerometer data
int AN_OFFSET[6] = {0, 0, 0, 0, 0, 0}; //Array that stores the Offset of the sensors

    bitWrite(newbyte,7,bitRead(n1,3));
    bitWrite(newbyte,6,bitRead(n1,2));
    bitWrite(newbyte,5,bitRead(n1,1));
    bitWrite(newbyte,4,bitRead(n1,0));
    bitWrite(newbyte,3,bitRead(n2,3));
    bitWrite(newbyte,2,bitRead(n2,2));
    bitWrite(newbyte,1,bitRead(n2,1));
    bitWrite(newbyte,0,bitRead(n2,0));

    return newbyte;

float Accel_Vector[3] = {0, 0, 0}; //Store the acceleration in a vector
float Gyro_Vector[3] = {0, 0, 0}; //Store the gyros turn rate in a vector
float Omega_Vector[3] = {0, 0, 0}; //Corrected Gyro_Vector data
float Omega_P[3] = {0, 0, 0}; //Omega Proportional correction
float Omega_I[3] = {0, 0, 0}; //Omega Integrator
float Omega[3] = {0, 0, 0};

int gpsSats;
double gpsLat;
double gpsLng;
double gpsSpeed;
double gpsCourse;
double gpsAlt;

// Euler angles
float roll;
float pitch;
float yaw;

float errorRollPitch[3] = {0, 0, 0};
float errorYaw[3] = {0, 0, 0};

unsigned int counter = 0;
byte gyro_sat = 0;

float DCM_Matrix[3][3] = {
  {
    1, 0, 0
  }
  , {
    0, 1, 0
  }
  , {
    0, 0, 1
  }
};
float Update_Matrix[3][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}}; //Gyros here

    return newbyte;

float Temporary_Matrix[3][3] = {
  {
    0, 0, 0
  }
  , {
    0, 0, 0
  }
  , {
    0, 0, 0
  }
};

TinyGPSPlus GPS;
SoftwareSerial mySerial(8, 9);

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Pololu MinIMU-9 + Arduino AHRS");
  delay(1500);

  I2C_Init();
  Accel_Init();
  Compass_Init();
  Gyro_Init();
  Baro_Init();
  GPS_Init();

  pinMode (STATUS_LED, OUTPUT); // Status LED
  digitalWrite(STATUS_LED, LOW);

  delay(20);

  for (int i = 0; i < 32; i++) // We take some readings...
  {
    Read_Gyro();
    Read_Accel();
    for (int y = 0; y < 6; y++) // Cumulate values
      AN_OFFSET[y] += AN[y];
    delay(20);
  }

  for (int y = 0; y < 6; y++)
    AN_OFFSET[y] = AN_OFFSET[y] / 32;

  AN_OFFSET[5] -= GRAVITY * SENSOR_SIGN[5];

  //Serial.println("Offset:");
  for (int y = 0; y < 6; y++)
    Serial.println(AN_OFFSET[y]);

  delay(2000);
  digitalWrite(STATUS_LED, HIGH);

  timer = millis();
  delay(20);
  counter = 0;
}


uint32_t GPStimer = millis();
void loop() //Main Loop
{
  // This sketch displays information every time a new sentence is correctly encoded.

  while (mySerial.available() > 0)
    if (GPS.encode(mySerial.read()))
      Read_myGPS();

  if (millis() > 5000 && GPS.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
  printGPS();

  if ((millis() - timer) >= 20) // Main loop runs at 50Hz
  {
    counter++;
    timer_old = timer;
    timer = millis();



    if (timer > timer_old)
    {
      G_Dt = (timer - timer_old) / 1000.0; // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
      if (G_Dt > 0.2)
        G_Dt = 0; // ignore integration times over 200 ms
    }
    else
      G_Dt = 0;

    // if a sentence is received, we can check the checksum, parse it...
    // *** DCM algorithm
    // Data adquisition

    Read_Gyro();   // This read gyro data
    Read_Accel();     // Read I2C accelerometer
    Read_Baro();

    if (counter > 5)  // Read compass data at 10Hz... (5 loop runs)
    {
      counter = 0;
      Read_Compass();    // Read I2C magnetometer
      Compass_Heading(); // Calculate magnetic heading

    }

    // Calculations...
    Matrix_update();
    Normalize();
    Drift_correction();
    Euler_angles();
    // ***
    printdata();

  }

  //Send frame
  void NF_sendFrame(byte byte) {

    Serial.println(byte);

  }

  void NF_sendFrameTerminator() {

    Serial.println(255);

  }

  //Frame -> char8, then send a 6 frame packet
  void NF_sendPacket(NFFrameControl control,NFFrameData data) {


    NF_sendFrame(control.stream);

    NF_sendFrame(data.A);
    NF_sendFrame(data.B);
    NF_sendFrame(data.C);
    NF_sendFrame(data.D);

    NF_sendFrameTerminator();

    /*
    cout << (control.stream.to_ulong()) << endl;
    cout << (data.A.to_ulong()) << endl;
    cout << (data.B.to_ulong()) << endl;
    cout << (data.C.to_ulong()) << endl;
    cout << (data.D.to_ulong()) << endl;
    */
  }


void setup() {

  Serial.begin(115200);

}

