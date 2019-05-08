//using namespace std;
//#include <iostream>
//#include <iomanip>
//#include <cmath>
//#include <string>
//#include <sstream>
//#include <bitset>
//#include <cstdlib>
//#include <cstring>
//#include <vector>



//FRAMES
//NF data frame
struct NFFrameData
{

  bitset<8> A;
  bitset<8> B;
  bitset<8> C;
  bitset<8> D;

};

//NF control frame
struct NFFrameControl
{

  bitset<8> stream;
  bitset<5> sensor;
  bitset<3> feed;

};

//DATATYPES
//NFfloat data type (decimal scientific notation)
struct NFfloat
{

  int sign;
  int exponent;
  int significand;

};

//TYPECAST OPERATORS
//Base 32 -> Decimal functions
int val(char c)
{
    if (c >= '0' && c <= '9')
        return (int)c - '0';
    else
        return (int)c - 'A' + 10;
}

int toDeci(char *str, int base)
{
    int len = strlen(str);
    int power = 1; // Initialize power of base
    int num = 0;  // Initialize result
    int i;

    // Decimal equivalent is str[len-1]*1 +
    // str[len-1]*base + str[len-1]*(base^2) + ...
    for (i = len - 1; i >= 0; i--)
    {
        // A digit in input number must be
        // less than number's base
        if (val(str[i]) >= base)
        {
           printf("Invalid Number");
           return -1;
        }

        num += val(str[i]) * power;
        power = power * base;
    }

    return num;
}

//NIBBLE OPERATORS
//Decimal to 4-bit nibble (VALUES: 0-15)
bitset<4> NF_decimalToNibble(int n) {

  std::bitset<4> nibble;
  unsigned int remainder, i = 1, step = 0, number_of_digits = 0;

  long binaryNumber = 0;


  if (n == 9) {

      nibble.set(0,true);
      nibble.set(1,false);
      nibble.set(2,false);
      nibble.set(3,true);

  } else {

    while (n!=0)
      {
      remainder = n%2;
      n /= 2;
      binaryNumber += remainder*i;
      nibble.set(step,static_cast<bool>(binaryNumber));
      i *= 10;
      step += 1;
     }

  }

 return nibble;

}

//Decimal to 5-bit nibble (VALUES: 0-31)
bitset<5> NF_decimalToNibble5(int n) {

  std::bitset<5> nibble;
  unsigned int remainder, i = 1, step = 0, number_of_digits = 0;

  long binaryNumber = 0;


  while (n!=0){
    remainder = n%2;
    n /= 2;
    binaryNumber += remainder*i;
    nibble.set(step,static_cast<bool>(binaryNumber));
    i *= 10;
    step += 1;
  }

 return nibble;

}

//Decimal to 3-bit nibble (VALUES: 0-7)
bitset<3> NF_decimalToNibble3(int n) {

  std::bitset<3> nibble;
  unsigned int remainder, i = 1, step = 0, number_of_digits = 0;

  long binaryNumber = 0;


  while (n!=0){
    remainder = n%2;
    n /= 2;
    binaryNumber += remainder*i;
    nibble.set(step,static_cast<bool>(binaryNumber));
    i *= 10;
    step += 1;
  }

 return nibble;

}

//Joins two 4-bit nibbles
bitset<8> NF_joinNibble(bitset<4> n1, bitset<4> n2) {

  std::bitset<8> newbyte;

  newbyte.set(7, n1.test(3));
  newbyte.set(6, n1.test(2));
  newbyte.set(5, n1.test(1));
  newbyte.set(4, n1.test(0));
  newbyte.set(3, n2.test(3));
  newbyte.set(2, n2.test(2));
  newbyte.set(1, n2.test(1));
  newbyte.set(0, n2.test(0));

  return newbyte;

}

//Joins a 5-bit and a 3-bit nibble, used for control frames
bitset<8> NF_joinNibbleStream(bitset<5> n1, bitset<3> n2) {

  std::bitset<8> newbyte;

  newbyte.set(7, n1.test(4));
  newbyte.set(6, n1.test(3));
  newbyte.set(5, n1.test(2));
  newbyte.set(4, n1.test(1));
  newbyte.set(3, n1.test(0));
  newbyte.set(2, n2.test(2));
  newbyte.set(1, n2.test(1));
  newbyte.set(0, n2.test(0));

  return newbyte;

}

//Converts a float to an NFfloat
NFfloat NF_floatToComponents(float f) {

 int posneg, exponent, significand;

  if (f >= 0)
    posneg = 0;
  else
    posneg = 1;

  exponent = (int)log10(fabs(f));
  significand = fabs((f / pow(10, exponent))* pow(10, 5));

  int outVal [3] = {posneg, exponent, significand};

  NFfloat buffer;
  buffer.sign = posneg;
  buffer.exponent = exponent;
  buffer.significand = significand;

  return buffer;

};

//Generates data frame from an NFfloat
NFFrameData NF_createFrameData(int posneg,int exponent,int significand,int channel) {

  bitset<4> nySign = NF_decimalToNibble((10 + (2*channel) - posneg)-1);
  bitset<4> nyExp = NF_decimalToNibble(exponent);

  std::string signifArray;
  std::stringstream ss;
  ss << significand;
  signifArray = ss.str();

  bitset<4> nyA1 = NF_decimalToNibble(std::stoi(signifArray.substr(0,1)));
  bitset<4> nyA2 = NF_decimalToNibble(std::stoi(signifArray.substr(1,1)));
  bitset<4> nyB1 = NF_decimalToNibble(std::stoi(signifArray.substr(2,1)));
  bitset<4> nyB2 = NF_decimalToNibble(std::stoi(signifArray.substr(3,1)));
  bitset<4> nyC1 = NF_decimalToNibble(std::stoi(signifArray.substr(4,1)));
  bitset<4> nyC2 = NF_decimalToNibble(std::stoi(signifArray.substr(5,1)));

  bitset<8> byA = NF_joinNibble(nySign,nyExp);
  bitset<8> byB = NF_joinNibble(nyA1,nyA2);
  bitset<8> byC = NF_joinNibble(nyB1,nyB2);
  bitset<8> byD = NF_joinNibble(nyC1,nyC2);

  NFFrameData buffer;

  buffer.A = byA;
  buffer.B = byB;
  buffer.C = byC;
  buffer.D = byD;

  return buffer;

}

//Generates control frame from stream ID
NFFrameControl NF_createFrameControl(char sensorN[], int feedN) {

  bitset<3> feed = NF_decimalToNibble3(feedN);

  int sensorInt = toDeci(sensorN,32);

  bitset<5> sensor = NF_decimalToNibble5(sensorInt);

  NFFrameControl buffer;

  buffer.stream = NF_joinNibbleStream(sensor,feed);
  buffer.sensor = sensor;
  buffer.feed = feed;

  return buffer;

}

//Frame -> char8, then send a 5 frame packet
void NF_sendPacket(NFFrameControl control,NFFrameData data) {


  NF_sendFrame(control.stream.to_ulong());

  NF_sendFrame(data.A.to_ulong());
  NF_sendFrame(data.B.to_ulong());
  NF_sendFrame(data.C.to_ulong());
  NF_sendFrameTerminator(data.D.to_ulong());

  /*
  cout << static_cast<char>(control.stream.to_ulong()) << endl;
  cout << static_cast<char>(data.A.to_ulong()) << endl;
  cout << static_cast<char>(data.B.to_ulong()) << endl;
  cout << static_cast<char>(data.C.to_ulong()) << endl;
  cout << static_cast<char>(data.D.to_ulong()) << endl;
  */
};

//Send frame
void NF_sendFrame(int byte) {

  Serial.print(byte);

};

void NF_sendFrameTerminator(char byte[]) {

  Serial.print(static_cast<char>(byte));

};

//Classes
class Sensor {

public:
  char *id;
  bool enabled;
  void (*apiRead)(); //Sensor.apiRead = &somefunction; ,, Function defined by sensor OEM that reads data from sensor
  int freq; //Hz
  int wait;
  int feeds;

  Sensor(char *identification, int desiredFeeds)
  {

    id = identification;
    enabled = true;
    freq = 50;
    wait = ((1 / freq) * 1000);

    feeds = desiredFeeds;

    //Add itself to the sensorlist
    SensorList.push_back(this);

  }
  addFeeds()
  {

    //Add feeds
    for (int i = 0; i < feeds; i++) {

      Feed i(this,i);

    }
  }

};

class Feed {

public:
  int id, channel;
  float (*source)(); //Feed.source = &somefunction;  ,, Function defined by us that returns a single float from an apiFeed read
  Sensor sensor;
  NFFrameControl control;

  Feed(Sensor parentSensor, int identification)
  {

    control = NF_createFrameControl(parentSensor.id,identification);

    id = identification;
    sensor = parentSensor;
    channel = 1;

  }

  void transmit() {

    float rawval = source();
    NFfloat nffloat = NF_floatToComponets(rawval);
    NFFrameData = NF_createFrameData(nffloat.sign,nffloat.exponent,nffloat.significand,channel);
    NF_sendPacket(control,data);

  }

};

/*
//Code for testing
int main() {

 float floatval = 1.23456;
 NFfloat finalinput = NF_floatToComponents(floatval);

 NFFrameData dataframe = NF_createFrameData(finalinput.sign,finalinput.exponent,finalinput.significand,1);
 char *sensor = "V";
 NFFrameControl controlframe = NF_createFrameControl(sensor,1);

 NF_sendPacket(controlframe,dataframe);

}
*/
