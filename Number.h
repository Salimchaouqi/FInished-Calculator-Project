/* 
 * Author: max
 * 
 * Version 2.0
 */

#ifndef NUMBER_H
#define	NUMBER_H

using namespace std;
#include <iostream>
#include <sstream>

//#include "Expression.h"
class Expression;

class Number {
    
public:
    Number();
    virtual ~Number();

    //Output Methods
    virtual string getRep() = 0; //returns the best possible exact representation
    virtual double getEstimation() = 0; //returns estimated (or exact if rational) value.

    //standard operations
    virtual Expression* add(Number* number) = 0;
    virtual Expression* sub(Number* number) = 0;
    virtual Expression* mult(Number* number) = 0;
    virtual Expression* div(Number* number) = 0;
    virtual Expression* exp(Number* power) = 0;
    
    //virtual Expression* root(Number* root) = 0; I don't think we need this b/c I can just create a root Number and put any other Number inside that. 
    
    //special methods
    virtual Expression* simplify() = 0;  //expect this method to only be called directly after initialization
    virtual bool equals(Number* number) = 0;
    virtual bool isRational() = 0;
};

#endif
