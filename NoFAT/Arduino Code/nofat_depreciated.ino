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
