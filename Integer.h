/* 
 * Author: max
 * 
 * Version 2.0
 */

#ifndef INTEGER_H
#define	INTEGER_H

#include <vector>

#include "Number.h"
#include "Integer.h"
#include "Expression.h"

class Integer : public Number {
public:

    //default constructor is = 1
    Integer();
    Integer(const Integer& orig);
    virtual ~Integer();

    Integer(int value);

    //operations specific to Integer
    int* getPrimeFactorization(int& arraySize);

    //Output Methods
    string getRep(); //returns the best possible exact representation
    double getEstimation(); //returns estimated (or exact if rational) value.
    int getValue();

    //standard operations
    Expression* add(Number* number);
    Expression* sub(Number* number);
    Expression* mult(Number* number);
    Expression* div(Number* number);
    Expression* exp(Number* power);

    //special methods
    Expression* simplify();
    bool equals(Number* number);
    
    bool isRational();

private:
    int value;
    Expression* simpleOp(char op, Number* number); //used internally to simplify the different methods above

    

};

#endif	/* INTEGER_H */

