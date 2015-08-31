/* 
 * Author: max
 * 
 * Version 2.0
 */

#ifndef CONSTANT_H
#define	CONSTANT_H
#include "Number.h"
//#include "Constant.h"
#include "Integer.h"
#include "Expression.h"


class Constant : public Number {
public:

    //Constructors
    Constant();
    Constant(string symbol, double value);
    
    Constant(char commonConstant); //accepts 'e' or 'p' for e and pi respectively
    
    Constant(const Constant& orig);
    virtual ~Constant();
    

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
    double value;
    string symbol;
};

/*
 
class Constant : public Number {
public:
    
    Constant(string symbol, double value);
    void add(Number* number);
    
    //Methods that have been overriden
    string getRep();
    double getEstimation();
    
    bool mult(Number* number);
    
    bool isRational();
    bool isConstant();
    
    
private:
    double value;
    string symbol;
    
};*/

#endif	/* CONSTANT_H */

