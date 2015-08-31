/*
 * File:   Log.cpp
 * Author: Jeff
 *
 * Version: 4.1
 */

#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>

#include "Log.h"
using namespace std;


    //<< Constructors >>
Log::Log(Number* base, Number* value)
{
    if(base->getEstimation() <= 0)
        throw "Invalid base entered: base cannot be negative";
    if(value->getEstimation() <= 0)
        throw "Invalid value entered: value cannot be negative";
    this->base = Expression::copyNumber(base);
    this->value = Expression::copyNumber(value);
}

Log::~Log()
{
    delete(base);
    base = NULL;
    delete(value);
    value = NULL;
}

Log::Log(const Log& orig)
{
	//Copy Constructor
	this->value = Expression::copyNumber(orig.value);
	this->base = Expression::copyNumber(orig.base);

}

Number* Log::getBase()
{
    return base;
}

Number* Log::getValue()
{
    return value;
}

void Log::setBase(Number* base)
{
    this->base = base;
}

void Log::setValue(Number* value)
{
    this->value = value;
}

//<< Output Methods >>

string Log::getRep()
{
    string rep;
    Integer* testBaseInt = dynamic_cast<Integer*>(this->base);
    Integer* testValueInt = dynamic_cast<Integer*>(this->value);
    Constant* testBaseConst = dynamic_cast<Constant*>(this->base);
    Constant* testValueConst = dynamic_cast<Constant*>(this->value);
    Fraction* testBaseFrac = dynamic_cast<Fraction*>(this->base);
    Fraction* testValueFrac = dynamic_cast<Fraction*>(this->value);
    rep = "log_";
    if(testBaseInt || testBaseConst || testBaseFrac)
    {
            rep = rep + this->base->getRep();
    }
    else
        rep = rep + "(" + this->base->getRep() + ")";

    if(testValueInt || testValueConst || testValueFrac)
        rep = rep + ":" + this->value->getRep();
    else
        rep = rep + ":(" + this->value->getRep() + ")";

    return rep;
}
double Log::getEstimation()
{
    double approx = 0;
    for(int dig = 0; dig <= 5; dig++)
    {
        double guess;
        double exp = 0 + approx;
        for(guess = 0; exp <= this->value->getEstimation(); guess += 1/(pow(10,dig)))
        {
            exp = pow(this->base->getEstimation(), guess+approx);
        }
        guess -= 2/(pow(10, dig));
        approx += guess;
    }
    return approx;
}

//<< Calculation Methods >>
Expression* Log::add(Number* number) //add expression parameter to put final representaion into
{
    //only add the objects if they are both of Log type with the same base and value
    if(this->equals(number))
    {
        Number* two = new Integer(2);
        Expression* returnExp = new Expression(number);
        returnExp->mult(two);
        delete(two);
        return returnExp;
    }
    return NULL;
}

Expression* Log::sub(Number* number)//what happens if I want to return a single number instead of an expression?
{   //zero if Logs are identical
    if(this->equals(number))
    {
        Number* zero = new Integer(0);
        cout<<zero->getRep()<<endl;
        Expression* expression = new Expression(zero);
        delete(zero);
        delete[] number;
        return expression;
    }
    return NULL;
}

Expression* Log::mult(Number* number)
{   //There is no way to simplify the multiplication of Logs
    return NULL;
}

Expression* Log::div(Number* number)
{
    Log* testLog = dynamic_cast<Log*>(number);
    if(testLog)
    {
        if(this->base->equals(testLog->getBase()))
        {//base change law of Logs
            Log* newBase = new Log(testLog->getValue(), this->value);
            Expression* expression = new Expression(newBase);
            this->value = newBase->getValue();
            this->base = newBase->getBase();
            delete(newBase);
            return expression;
        }
    }
    return NULL;
}

Expression* Log::exp(Number* number)
{   //There is no way to simplify the multiplication of Logs
    return NULL;
}
Expression* Log::simplify()
{
    Fraction* testFrac = dynamic_cast<Fraction*>(this->value);
    Integer* testInt = dynamic_cast<Integer*>(this->value);
    Root* testRoot = dynamic_cast<Root*>(this->value);
    if(isRational())//if the log simplifies to a rational number, an integer expression will be returned
    {
        Number* abs = new Integer(this->getEstimation());
        Expression* expression = new Expression(abs);
        delete(abs);
        return expression;
    }

    else if(testFrac)//if the log value is a fraction, the log will decompose into "log - log"
    {
        Number* newLog1 = new Log(this->base, testFrac->getNumInt());
        Number* newLog2 = new Log(this->base, testFrac->getDenInt());
        Expression* expression = new Expression(newLog1);
        expression->sub(newLog2);
        delete(newLog1);
        delete(newLog2);
        return expression;
    }
    else if(testInt)//if the value is an integer, the log will decomposed into the summation of the logs of it's factors
    {
        vector<int> factors;
        vector<Expression*> newLogs;
        Integer* testBase = dynamic_cast<Integer*>(this->base);
        int factorSize;
        testInt->getPrimeFactorization(factorSize);
        if(testBase->getValue() > testInt->getValue() && factorSize == 1)
        {
            int tempValue = testInt->getValue();
            int power = 1;
            while(tempValue < testBase->getValue())
            {
                power++;
                tempValue = pow(testInt->getValue(), power);
            }
            if(testBase->getValue() % tempValue == 0)
            {
                Integer* one = new Integer(1);
                Integer* den = new Integer(power);
                Fraction* newFrac = new Fraction(one, den);
                return new Expression(newFrac);
            }
        }
        int temp = testInt->getValue();
        int x;
        while(temp % testBase->getValue() == 0 && testBase)
            {
                x = testBase->getValue();
                factors.push_back(x);
                temp = temp/x;
            }
        //factorization code:
        for(x = 2; x < temp; x++)
        {
            if(temp % x == 0)
            {
                factors.push_back(x);
                temp = temp/x;
                x = 1;
            }
        }
        int multiple = 1;
        factors.push_back(x); //saves last factor
        if(factors.size() > 1) //no need to do this if the number is prime.  If the number is prime, no simplification needed.
        {
            //checking the factors and creating each new log expression
            for(int i = 0; i < factors.size(); i++)
            {
                if(factors[i] != factors[i+1] || i == factors.size()-1)
                {
                    Integer* newValue = new Integer(factors[i]);
                    Log* newLog = new Log(this->base, newValue);
                    if(multiple == 1)
                    {//if there was only one of that factor, a simple half expression will be created
                        newLogs.push_back(new Expression(newLog));
                    }
                    else
                    {//if there were multiples of a factor, a full expression that is an integer times a log will be created
                        Integer* newMult = new Integer(multiple);
                        Expression* returnExp = new Expression(newLog);
                        returnExp->mult(newMult);
                        newLogs.push_back(returnExp);
                        delete(newMult);
                    }
                    multiple = 1;
                    delete(newValue);
                }
                else
                    multiple++;
            }
            Expression* express = newLogs[0];
            for(int i = 1; i < newLogs.size(); i++)
            {//new log expressions will all be added together
                express->add(newLogs[i]);
            }
            return express;
        }
        return NULL;
    }
    else if(testRoot)
    {   //If the value is a root, the root will be taken out of the log and multiplied as a fraction.
        //i.e. log(sqrt(3)) = (1/2)log(3)
        Integer* one = new Integer(1);
        Number* denom = testRoot->getRoot();
        Fraction* multFrac = new Fraction(one, denom);
        Log* newLog = new Log(this->base, testRoot->getValue());
        if(newLog->simplify() == NULL)
        {//the new log may be further simplifyable, better check! If simplify() returns NULL, then newLog is in its simplest form

            Expression* expression = new Expression(newLog);
            expression->mult(multFrac);
            delete(one); delete(denom); delete(multFrac); delete(newLog);
            return expression;
        }
        else
        {
            Expression* expression = newLog->simplify();
            expression->mult(multFrac);
            delete(one); delete(denom); delete(multFrac); delete(newLog);
            return expression;
        }
    }
    else
        return NULL;
}

bool Log::equals(Number* number)
{
    Log* testLog = dynamic_cast<Log*>(number);
    if(testLog)
    {
        if(this->base->equals(testLog->getBase()) && this->value->equals(testLog->getValue()))
        {
            return true;
        }
    }
    return false;
}

bool Log::isRational()
{
    if(int(this->getEstimation()) == this->getEstimation())
        return true;
    return false;
}



