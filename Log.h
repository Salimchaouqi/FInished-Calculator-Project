/*
 * File:   Log.h
 * Author: Jeff
 *
 * Version: 4.0
 */

#ifndef LOG_H
#define	LOG_H


#include <iostream>
#include <sstream>

#include "Number.h"
#include "Expression.h"
#include "Integer.h"
#include "Fraction.h"
#include "Constant.h"
#include "Root.h"

using namespace std;

class Log: public Number
{
public:

    Log(Number* base, Number* value);
    ~Log();
    Log(const Log& orig);

    string getRep();
    double getEstimation(); //returns estimated (or exact if rational) value.

    Expression* add(Number* number);
    Expression* sub(Number* number);
    Expression* mult(Number* number);
    Expression* div(Number* number);
    Expression* exp(Number* power);

    Expression* simplify();
    bool equals(Number* number);
    bool isRational();

    Number* getBase();
    Number* getValue();
    void setBase(Number* base);
    void setValue(Number* value);
private:
    Number *base, *value;
};

#endif
