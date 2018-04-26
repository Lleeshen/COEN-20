/*
Implement Bin2Dec, Dec2Bin, and TwosComplement function from main
April 11 2018
Lyman Shen
*/

//Purpose of TwosComplement function
//Take the Twos Complement of the input as binary number and store it in output
void TwosComplement(const int input[8], int output[8]) {
    int i = 0;
    //From Least Significant bit, copy bits as is till the first 1
    while(i < 8 && input[i] == 0) {
            output[i] = 0;
            i++;
    }
    output[i] = 1;
    //For rest of the bits, the output is opposite the value of the input
    i++;
    while(i < 8) {
            if(input[i] == 0) {
                output[i] = 1;
            }
            else {
                output[i] = 0;
            }
            i++;
    }
}

//Purpose of Bin2Dec function
//Convert the binary number bin in 2's complement representation to a float, return the float
float Bin2Dec(const int bin[8])  {
    //Polynomial is initially the sign bit
    int polynomial = -bin[7];
    int i;
    //Polynomial Evaluation
    for(i=6; i >= 0; i--) {
        polynomial = 2 * polynomial + bin[i];
    }
    //Convert polynomial to decimal between -1 and 1
    return (float) polynomial / 128.0;
}

//Convert the float x to 2's complement binary representation, store the binary number in bin
void Dec2Bin(const float x, int bin[8]) {
    int i;
    int temp[8];
    //Get absolute value of float
    float fNum = (x > 0) ? x : -x;
    //Convert number to integer
    fNum = 128 * fNum;
    //The floor of the number + 0.5 is the number rounded to the nearest integer.
    if((int) fNum < 127) {
        fNum = fNum + 0.5;
    }
    //Binary representation of this number is the result if the original number is positive
    int num = (int) fNum;
    //Take care of overflow case: if the number is 128, repeated division does not work
    if(num == 128 ) {
        //The sign bit is negative and all other bits are 0
        bin[7] = 1;
        for(i=6; i>=0; i--) {
            bin[i] = 0;
        }
    }
    else {
    //Repeated division method to get bits
        i = 0;
        while (i < 7) {
            temp[i] = num % 2;
            num = num / 2;
            i++;
        }
        temp [7] = 0;
        //If negative, take 2's complement, else copy the bits
        if(x < 0) {
            TwosComplement(temp,bin);
        } else {
            i = 0;
            while (i < 8) {
                bin[i] = temp[i];
                i++;
            }
        }
    }
}
