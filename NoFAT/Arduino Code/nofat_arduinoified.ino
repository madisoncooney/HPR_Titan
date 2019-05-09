//FRAMES
//NF data frame
struct NFFrameData
{

  byte A;
  byte B;
  byte C;
  byte D;

};

//NF control frame
struct NFFrameControl
{

  byte stream;
  byte sensor; //actually 5 bits
  byte feed; //actually 3 bits

};

//DATATYPES
//NFfloat data type (decimal scientific notation)
struct NFfloat
{

  int sign;
  int exponent;
  int significand1;
  int significand2;
  int significand3;
  int significand4;
  int significand5;
  int significand6;

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

int toDeci(char *str,int base) {

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
byte NF_decimalToNibble(int n) {

  byte nibble = 0b00000000;
  byte rawnum = n;

  bitWrite(nibble,7,bitRead(rawnum,7));
  bitWrite(nibble,6,bitRead(rawnum,6));
  bitWrite(nibble,5,bitRead(rawnum,5));
  bitWrite(nibble,4,bitRead(rawnum,4));
  bitWrite(nibble,3,bitRead(rawnum,3));
  bitWrite(nibble,2,bitRead(rawnum,2));
  bitWrite(nibble,1,bitRead(rawnum,1));
  bitWrite(nibble,0,bitRead(rawnum,0));

  return nibble;

}

//Joins two 4-bit nibbles
byte NF_joinNibble(byte n1, byte n2) {

  byte newbyte = 0b00000000;

  bitWrite(newbyte,7,bitRead(n1,3));
  bitWrite(newbyte,6,bitRead(n1,2));
  bitWrite(newbyte,5,bitRead(n1,1));
  bitWrite(newbyte,4,bitRead(n1,0));
  bitWrite(newbyte,3,bitRead(n2,3));
  bitWrite(newbyte,2,bitRead(n2,2));
  bitWrite(newbyte,1,bitRead(n2,1));
  bitWrite(newbyte,0,bitRead(n2,0));

  return newbyte;

}

//Joins a 5-bit and a 3-bit nibble, used for control frames
byte NF_joinNibbleStream(byte n1, byte n2) {

  byte newbyte = 0b00000000;

  bitWrite(newbyte,7,bitRead(n1,4));
  bitWrite(newbyte,6,bitRead(n1,3));
  bitWrite(newbyte,5,bitRead(n1,2));
  bitWrite(newbyte,4,bitRead(n1,1));
  bitWrite(newbyte,3,bitRead(n1,0));
  bitWrite(newbyte,2,bitRead(n2,2));
  bitWrite(newbyte,1,bitRead(n2,1));
  bitWrite(newbyte,0,bitRead(n2,0));

  return newbyte;

}

//Converts a float to an NFfloat
NFfloat NF_floatToComponents(float f) {

  int posneg, exponent, significand1, significand2, significand3, significand4, significand5, significand6;

  if (f >= 0)
  posneg = 1;
  else
  posneg = 0;

  exponent = (int)log10(abs(f));
  significand1 = (static_cast<int>((abs((f / pow(10, exponent))* pow(10, 5)))/100000)) % 10;
  significand2 = (static_cast<int>((abs((f / pow(10, exponent))* pow(10, 5)))/10000)) % 10;
  significand3 = (static_cast<int>((abs((f / pow(10, exponent))* pow(10, 5)))/1000)) % 10;
  significand4 = (static_cast<int>((abs((f / pow(10, exponent))* pow(10, 5)))/100)) % 10;
  significand5 = (static_cast<int>((abs((f / pow(10, exponent))* pow(10, 5)))/10)) % 10;
  significand6 = ((static_cast<int>((abs((f / pow(10, exponent))* pow(10, 5)))/1)) % 10)-1;



  NFfloat buffer;
  buffer.sign = posneg;
  buffer.significand1 = significand1;
  buffer.significand2 = significand2;
  buffer.significand3 = significand3;
  buffer.significand4 = significand4;
  buffer.significand5 = significand5;
  buffer.significand6 = significand6;

  //Fix exponentiation overflow
  exponent = (exponent);

  if (exponent < -7) {

    buffer.exponent = -7;

  } else if (exponent > 8) {

    buffer.exponent = 8;

  } else {

   buffer.exponent = exponent;

  }

  return buffer;

}

//Generates data frame from an NFfloat
NFFrameData NF_createFrameData(int posneg,int exponent,int significand1,int significand2,int significand3,int significand4,int significand5,int significand6,int channel) {

  byte nySign = NF_decimalToNibble(((10 + (2*channel) - posneg)-1));
  byte nyExp = NF_decimalToNibble(exponent);

  byte a1 = NF_decimalToNibble(significand1);
  byte a2 = NF_decimalToNibble(significand2);
  byte b1 = NF_decimalToNibble(significand3);
  byte b2 = NF_decimalToNibble(significand4);
  byte c1 = NF_decimalToNibble(significand5);
  byte c2 = NF_decimalToNibble(significand6);

  byte byA = NF_joinNibble(nySign,nyExp);
  byte byB = NF_joinNibble(a1,a2);
  byte byC = NF_joinNibble(b1,b2);
  byte byD = NF_joinNibble(c1,c2);

  NFFrameData buffer;

  buffer.A = byA;
  buffer.B = byB;
  buffer.C = byC;
  buffer.D = byD;

  return buffer;

}

//Generates control frame from stream ID
NFFrameControl NF_createFrameControl(char sensorN[], int feedN) {

  byte feed = static_cast<byte>(feedN);

  int sensorInt = toDeci(sensorN,32);

  byte sensor = NF_decimalToNibble(sensorInt);

  NFFrameControl buffer;

  buffer.stream = NF_joinNibbleStream(sensor,feed);
  buffer.sensor = sensor;
  buffer.feed = feed;

  return buffer;

}

//Send frame
void NF_sendFrame(byte byte) {

  Serial.flush();
  Serial.write((byte));

}

void NF_sendFrameTerminator() {

  Serial.flush();
  Serial.write((255));

}

//Frame -> char8, then send a 6 frame packet
void NF_sendPacket(NFFrameControl control,NFFrameData data) {


  NF_sendFrame(control.stream);

  NF_sendFrame(data.A);
  NF_sendFrame(data.B);
  NF_sendFrame(data.C);
  NF_sendFrame(data.D);

  NF_sendFrameTerminator();



}

void setup() {

  Serial.begin(9600);

}

void loop() {

  float floatval = 12.3456;
  NFfloat finalinput = NF_floatToComponents(floatval);

  NFFrameData dataframe = NF_createFrameData(finalinput.sign,finalinput.exponent,finalinput.significand1,finalinput.significand2,finalinput.significand3,finalinput.significand4,finalinput.significand5,finalinput.significand6,1);
  char *sensor = "V";
  NFFrameControl controlframe = NF_createFrameControl(sensor,2);

  NF_sendPacket(controlframe,dataframe);

}

/*
//Code for testing


}
*/
