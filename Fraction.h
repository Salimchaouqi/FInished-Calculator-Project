/*
 * Fractions.h
 *
 *  Created on: Apr 10, 2014
 *      Author: nav
 */

#ifndef FRACTION_H_
#define FRACTION_H_
//#include <iostream>
#include <sstream>
#include <math.h>
#include <vector>
#include <string>


#include "Number.h"

using namespace std;

class Integer;
class Root;

class Fraction : public Number {
public:

    //Constructors
	Fraction();
    Fraction(Number* num, Number* den);
    Fraction(const Fraction& orig);
    virtual ~Fraction();

    //get methods
    Number* getNumInt();
    Number* getDenInt();
    Number* getDenRoot();


    //standard operations
    Expression* add(Number* number);
    Expression* sub(Number* number);
    Expression* mult(Number* number);
    Expression* div(Number* number);
    Expression* exp(Number* power);

    //Output Methods
    string getRep(); //returns the best possible exact representation
    double getEstimation(); //returns estimated (or exact if rational) value.

    //special methods
    Expression* simplify();
    bool equals(Number* number);
    bool isRational();
private:
    Integer *numInt, *denInt;
    Root *denRoot;

};

#endif /* FRACTION_H_ */
