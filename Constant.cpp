/* 
 * Author: max
 * 
 * Version 2.0
 */

#include "Constant.h"

Constant::Constant() { //default constructor
    symbol = "pi";
    value = 3.141592653589793;
}

Constant::Constant(const Constant& orig) {
    value = orig.value;
    symbol = orig.symbol;
}

Constant::~Constant() {

}

//used for constants that are very common, like pi and e.

Constant::Constant(char commonConstant) {
    switch (commonConstant) {
        case 'p':
        case 'P':
            symbol = "pi";
            value = 3.141592653589793;
            break;
        case 'e':
        case 'E':
            symbol = "e";
            value = 2.718281828459045;
            break;
        default:
            throw "Invalid Common Constant passed into Constant class.";
    }
}

Constant::Constant(string symbol, double value) {

    this->value = value;
    this->symbol = symbol;
}

string Constant::getRep() { //returns the symbolic representation of the number.
    string output;
    ostringstream convert;

    convert << "(" << symbol << ")";

    output = convert.str();

    return output;
}

double Constant::getEstimation() {
    return value;
}

bool Constant::equals(Number* number) {
    bool equal = false;
    
    //essentially, if the symbols are the same, they are the same 'constant'
    Constant* constant = dynamic_cast<Constant*> (number);
    if (constant) {
        if(0 == symbol.compare(constant->symbol))
            equal = true;
    }
    
    return equal;
}

//most of the below return false because this class is a CONSTANT.

Expression* Constant::add(Number* number) { //a constant plus itself == 2* the constant
    if(Expression::debug) {
        cout << "Constant add(" << number->getRep() <<") called!" << endl;
    }
    //if the number taken in is a Constant...
    Constant* constant = dynamic_cast<Constant*>(number);
    if(constant)
        //and that Constant equals the current number...
        if(equals(constant)) {
            //return 2 * the constant within an expression.
            Number* two = new Integer(2);
            Number* constant = new Constant(symbol, value);
            Expression* returnExp = new Expression(constant);
            returnExp->mult(two);
            
            if(Expression::debug) {
                cout << "Constant added to itself! Returning: " << returnExp->getRep() << endl;
            }
            
            return returnExp;
        }
    //else 
    if(Expression::debug) {
        cout << "Cannot add " << number->getRep() << " to Constant! Returning NULL." << endl;
    }
    return NULL;
}

Expression* Constant::sub(Number* number) { //subtracting a constant by itself yields zero!
    //if the number taken in is a Constant...
    Constant* constant = dynamic_cast<Constant*>(number);
    if(constant)
        //and that Constant equals the current number...
        if(equals(constant)) {
            //return 0 within an expression.
            Number* zero = new Integer(0);
            Expression* returnExp = new Expression(zero);
            return returnExp;
        }
    //else    
    return NULL;
}

Expression* Constant::mult(Number* number) { //you cannot multiply a constant. You must add a RationalNumber number to the appropriate vector instead.
    return NULL;
}

Expression* Constant::div(Number* number) { //you CAN divide by a constant! you can divide by itself!
    //if the number taken in is a Constant...
    Constant* constant = dynamic_cast<Constant*>(number);
    if(constant)
        //and that Constant equals the current number...
        if(equals(constant)) {
            //return 1 within an expression.
            Number* one = new Integer(1);
            Expression* returnExp = new Expression(one);
            return returnExp;
        }
        
    return NULL;
}

Expression* Constant::exp(Number* power) {
    return NULL;
}

Expression* Constant::simplify() {
    return NULL;
}

bool Constant::isRational() {
    return false; //constants are ALWAYS irrational for our purposes.
}




