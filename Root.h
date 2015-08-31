



/*

 * root.h

 *

 *  Created on: Apr 8, 2014

 *      Author: Cody Miller

 *

 *      Version 1.0

 */



#ifndef ROOT_H_

#define ROOT_H_





//root.h

#include "Integer.h"

#include "Fraction.h"

#include "Expression.h"

#include "Constant.h"

#include "Number.h"





#include <iostream>

#include <sstream>

#include <cmath>

#include <vector>

#include <exception>

#include <algorithm>

#include <string>

class Root: public Number{



private:


    Integer* intPow;
    Fraction* fracPow;
    
    Constant* constVal;

    Integer* intVal;
    Integer* root;
    Fraction* fracVal;


    int mult1;
    






public:



	Root();

	Root(Number* value, Number* numRoot);

	Root(const Root& orig);





	~Root();

	Number* getRoot();

	Number* getValue();





	double getEstimation();

	string getRep();

	bool equals(Number* number);

	bool isRational();



	Expression* simplify();

	Expression* add(Number* value);

	Expression* sub(Number* value);

	Expression* mult(Number* value);

	Expression* div(Number* value);

	Expression* exp(Number* power);





};





#endif /* ROOT_H_ */


