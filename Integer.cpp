/* 
 * Author: max
 * 
 * Version 2.0
 */
#include <iostream>

#include "Integer.h"
#include "Fraction.h"
#include "Root.h"

Integer::Integer() {
    value = 1;
}

Integer::Integer(int value) {
    this->value = value;
}

Integer::Integer(const Integer& orig) {

    value = orig.value;
    if (Expression::debug) {
        std::cout << "Integer Copy constructor called.  New Value: " << getRep() << endl;
    }
}

Integer::~Integer() {

}

/*
 * output methods
 */

string Integer::getRep() {
    //turns the int into a string.
    ostringstream convert;
    convert << value;
    return convert.str();
}

double Integer::getEstimation() {
    return value;
}

int Integer::getValue() {
    return value;
}

//methods specific to Integer

//factors the integer into its prime roots in descending order

int* Integer::getPrimeFactorization(int& arraySize) {

    vector<int> factors;
    int temp = value;
    int x;
    for (x = 2; x < temp; x++) {
        if (temp % x == 0) {
            factors.push_back(x);
            temp = temp / x;
            x = 1;
        }
    }
    factors.push_back(x); //needed to include the last factor that x landed on

    //converts the vector into an int array, flipping the order so it becomes descending
    int* array = new int[factors.size()];

    int index = 0;
    for (int a = factors.size() - 1; a >= 0; a--) {
        array[index] = factors[a];
        index++;
    }

    int* tempSize = new int(factors.size());

    arraySize = *tempSize;
    return array;
}


//standard operations

Expression* Integer::add(Number* addNum) {
    if (Expression::debug) {
        cout << "add(" << addNum->getRep() << ") method of Integer Called!" << endl;
    }

    /* While most cases for add can be handled with  the "simpleOp" method, there
     is one special case when the integer equals zero that we should handle in a
     different manner. 
     
    /* 0 + any other number => just the number being returned*/

    //if the value of the current Integer instance is zero...
    if (value == 0) {
        if (Expression::debug) {
            cout << "special case for Integer add since the integer is zero! Returning added Number." << endl;
        }
        //we will immediately build a new expression containing only the added Number
        Expression* returnExp = new Expression(Expression::copyNumber(addNum));
        return returnExp;
    }

    return simpleOp('+', addNum);
}

Expression* Integer::sub(Number* number) {
    if (Expression::debug) {
        cout << "sub method of Integer Called!" << endl;
    }
    return simpleOp('-', number);
}

Expression* Integer::mult(Number* number) {
    //need to account for special case when the number taken in == 0
    Integer* num_int = dynamic_cast<Integer*>(number);
    if(num_int && (num_int->getValue()==0)) {
        Integer* zero = new Integer(0);
        Expression* retExp = new Expression(zero);
        delete(zero);
        zero = NULL;
        
        return retExp;
    }
    
    return simpleOp('*', number);
}

Expression* Integer::div(Number* div_num) {
    /*Division is the only tricky thing Integer has to deal with. Since the Expression class is 
     responsible for creating fractions when division is impossible, we must only return an Expression
     for .div when the number being divided divides cleanly into the current integer.
     
     In other words, we need to avoid any division that would result in a decimal value of any kind.*/
    if (Expression::debug) {
        cout << "Integer.div(" << div_num->getRep() << ") called. Current Value:  " << getRep() << endl;
    }

    Integer* integer = dynamic_cast<Integer*> (div_num);

    //if it's an integer...
    if (integer) {
        double testDouble = 0.0;
        int testInt = 0;

        //check and see if the double result of the division is the same as the
        //integer result. This will tell us if the values divided cleanly.
        testDouble = (double) value / (double) integer->value;
        testInt = (double) value / (double) integer->value;

        //if they are equal, return the simplified number!
        if (testDouble == (double) testInt) {
            Number* returnNum = new Integer(testInt);
            Expression* returnExp = new Expression(returnNum);
            return returnExp;
        }
    }
    Integer* integer_divNum = dynamic_cast<Integer*> (div_num);
    Root* root_divNum = dynamic_cast<Root*>(div_num);

    //okay, so this is our special case when we must invoke a fraction
    if (integer_divNum || root_divNum) {

        if (Expression::debug) {
            cout << "Integer creating a fraction! " << endl;
        }

        Fraction* frac = new Fraction(this, div_num);
        Expression* frac_simp_exp = frac->simplify();

        if (frac_simp_exp) {
            return frac_simp_exp;
        } else {
            Expression* retExp = new Expression(frac);
            return retExp;
        }

    }




    if (Expression::debug) {
        cout << "Integer could not be divided. Returning NULL." << endl;
    }

    return NULL;
}

Expression* Integer::exp(Number* power) {
    //create an expression for us to return
    Expression* returnExp = NULL;

    //cast the types we are capable of dealing with...
    Integer* integer = dynamic_cast<Integer*> (power);
    Fraction* fraction = dynamic_cast<Fraction*> (power);

    //if the power is an integer...
    if (integer) {

        //if the power is positive...
        if (integer->value > 0)
            //run a simple for loop to compute new value.
            for (int a = 0; a < integer->value; a++) {
                value *= value;

                returnExp = new Expression(this);
            }            //if the power == 0;
        else if (integer->value == 0) {
            //the integer equals zero.
            value = 1;

            returnExp = new Expression(this);
        } else {//if integer->value < 0
            int tempValue = value;
            int tempPower = integer->getValue() * -1;
            //compute the power as if it was positive...
            for (int a = 0; a < tempPower; a++) {
                tempValue *= tempValue;
            }

            //turn this temp value into 1/ a fraction.
            Number* one = new Integer(1);
            Number* den = new Integer(tempValue);

            //create the fraction
            Number* returnFrac = new Fraction(one, den);

            //delete the numbers used to create the fraction
            delete(one);
            delete(den);

            //create the expression, and set it equal to return Exp
            returnExp = new Expression(returnFrac);

            //delete the fraction used to create the expression
            delete(returnFrac);
        }
    }
    else if (fraction) {
        //extract values out of fraction
        Integer* denInt = dynamic_cast<Integer*> (fraction->getDenInt());
        Integer* numInt = dynamic_cast<Integer*> (fraction->getNumInt());

        //just to make sure both of the pointers are valid
        if (denInt && numInt) {
            
            
            
            returnExp = exp(numInt);
            
            returnExp->root(denInt);



        } else { //if one of the pointers are NOT valid
            throw "Raised to an invalid fraction.";
        }


        //
    } else { //we are not dealing with any compatible object, so return NULL
        return NULL;
    }

    //return the expression, no matter the case.
    return returnExp;
}

//special methods

Expression* Integer::simplify() {
    Expression* nullExp = NULL;
    return nullExp;
}

bool Integer::equals(Number* number) {

    Integer* integer = dynamic_cast<Integer*> (number);
    if (integer) {
        if (integer->value == value) {
            return true;
        }
    }

    //if it's not an Integer or the values are not equal,
    return false;
}

//performs every operation except division, since that is more complex.

Expression* Integer::simpleOp(char op, Number* number) {
    Expression* returnExp = NULL;

    //check if it is a workable type
    Integer* integer = dynamic_cast<Integer*> (number);

    //if so, simply perform the operation.
    if (integer) {

        switch (op) {
            case '+':
                value += integer->value;
                break;
            case '-':
                value -= integer->value;
                break;
            case '*':
                value *= integer->value;
                break;
            case '^':
                for (int a = 0; a < integer->value; a++) {
                    value *= value;
                }
                break;
            default:
                throw "Invalid operator";
        }

        Number* returnNum = new Integer(value);

        Expression* returnExp = new Expression(returnNum);
        return returnExp;
    }//if not, inform the program such a stunt is impossible

    else
        //what we want to do here is to return the OTHER value's add or multiply 
        //method if Integer fails to handle it.
        if(op == '+')
            return number->add(this);
        else if(op == '*')
            return number->mult(this);
        else
            return NULL;
}

bool Integer::isRational() {
    return true; //Integers are ALWAYS rational.
}