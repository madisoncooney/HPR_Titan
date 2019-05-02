#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <sstream>
using namespace std;

int appendDigit(int base, int append) {
   std::string sBase = std::to_string(base);
   std::string sAppend = std::to_string(append);
   std::string result = sBase + sAppend;
   return std::stoi(result);
}

//NF data frame
struct NFFrameData
{

  unsigned int A;
  unsigned int B;
  unsigned int C;
  unsigned int D;

};

struct NFfloat
{

  int sign;
  int exponent;
  int significand;

};

//Typecast operators
//Binary -> decimal
int NF_decimalToBinary(int n) {

  long binaryNumber = 0;
  unsigned int remainder, i = 1, step = 1, number_of_digits = 0;

  while (n!=0)
  {
      remainder = n%2;
      n /= 2;
      binaryNumber += remainder*i;
      i *= 10;
  }

 return binaryNumber;

}

int NF_binaryToDecimal(int n) {
    int decimalNumber = 0, i = 0, remainder;
    while (n!=0)
    {
        remainder = n%10;
        n /= 10;
        decimalNumber += remainder * pow(2,i);
        ++i;
    }
    return decimalNumber;
}

//Float -> components
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

//components -> NF bytes
NFFrameData NF_componentsToNF(int posneg,int exponent,int significand,int channel) {

  unsigned int nySign = NF_decimalToBinary((10 + (2*channel) - posneg)-1);
  unsigned int nyExp = NF_decimalToBinary(exponent);

  std::string signifArray;
  std::stringstream ss;
  ss << significand;
  signifArray = ss.str();

  unsigned int nyA1 = NF_decimalToBinary(std::stoi(signifArray.substr(0,1)));
  unsigned int nyA2 = NF_decimalToBinary(std::stoi(signifArray.substr(1,1)));
  unsigned int nyB1 = NF_decimalToBinary(std::stoi(signifArray.substr(2,1)));
  unsigned int nyB2 = NF_decimalToBinary(std::stoi(signifArray.substr(3,1)));
  unsigned int nyC1 = NF_decimalToBinary(std::stoi(signifArray.substr(4,1)));
  unsigned int nyC2 = NF_decimalToBinary(std::stoi(signifArray.substr(5,1)));

  unsigned int byA = appendDigit(nyA1,nyA2);
  unsigned int byB = appendDigit(nyA1,nyA2);
  unsigned int byC = appendDigit(nyA1,nyA2);
  unsigned int byD = appendDigit(nyA1,nyA2);

  NFFrameData buffer;

  buffer.A = byA;
  buffer.B = byB;
  buffer.C = byC;
  buffer.D = byD;

  return buffer;

}


//Code for testing
int main() {

  /*
 int outputvar = NF_decimalToBinary(9);
 int finalvar = NF_binaryToDecimal(outputvar);
 cout << finalvar;
 */

 float floatval = 1.23456;
 NFfloat finalinput = NF_floatToComponents(floatval);

 NFFrameData finalresultforreal = NF_componentsToNF(finalinput.sign,finalinput.exponent,finalinput.significand,1);

 cout << setfill('0') << setw(4) << finalresultforreal.A;

}
