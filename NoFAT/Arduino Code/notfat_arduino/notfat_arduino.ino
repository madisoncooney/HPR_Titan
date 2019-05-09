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

  }

  //Joins two 4-bit nibbles
  byte NF_joinNibble(byte n1, byte n2) {

    byte newbyte;

    n1 = n1 << 4;

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

    byte newbyte;

    n1 = n1 << 3;

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

  }

  //Generates data frame from an NFfloat
  NFFrameData NF_createFrameData(int posneg,int exponent,int significand,int channel) {

    byte nySign = NF_decimalToNibble((10 + (2*channel) - posneg)-1);
    nySign = nySign << 4;
    byte nyExp = NF_decimalToNibble(exponent);

    uint8_t a1 = ((significand / 100000) % 10);
    uint8_t a2 = (significand / 10000) % 10;
    uint8_t b1 = ((significand / 1000) % 10);
    uint8_t b2 = (significand / 100) % 10;
    uint8_t c1 = ((significand / 10) % 10);
    uint8_t c2 = (significand / 1) % 10;

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

    byte feed = NF_decimalToNibble(feedN);

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

void loop() {

  float floatval = 123.456;
  NFfloat finalinput = NF_floatToComponents(floatval);

  NFFrameData dataframe = NF_createFrameData(finalinput.sign,finalinput.exponent,finalinput.significand,1);
  char *sensor = 0;
  NFFrameControl controlframe = NF_createFrameControl(sensor,0);

  NF_sendPacket(controlframe,dataframe);

}

/*
//Code for testing


}
*/
