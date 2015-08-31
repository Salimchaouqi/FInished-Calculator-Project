/*
 * Fractions.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: nav
 */

#include "Fraction.h"
#include "Root.h"
#include "Integer.h"

//Constructors

Fraction::Fraction(Number* num, Number* den) {
    
    	if (Expression::debug) {
		cout << "Fraction(" << num->getRep() << " , " << den->getRep() << ") Called." << endl;
	}
        

        numInt = NULL;
	denInt = NULL;
	denRoot = NULL;
    
	Number* numCopy = Expression::copyNumber(num);
	Number* denCopy = Expression::copyNumber(den);



	//check if its dividng by zero
	//dyanamic cast denominator
	//cast into integer or root
	//check if either value is zero, then throw exception



	Integer* numInt = dynamic_cast<Integer*> (numCopy);
	Root* numRoot = dynamic_cast<Root*> (numCopy);

	Integer* denInt = dynamic_cast<Integer*> (denCopy);
	Root* denRoot = dynamic_cast<Root*> (denCopy);

        if(Expression::debug && denRoot) {
            cout << "Fraction denRoot value: " << denRoot->getRep() << endl;
        }
	if (denInt && (denInt->getValue() == 0)) {
		throw "sorry you cant divide by zero";
	} else if (numInt) {
		this->numInt = numInt;

	} else if (numRoot) {
		throw "Sorry Max, fraction class does not deal with root numerators, back to you";

	} else {
		throw "No valid Numerator";
	}
	if (denInt) {
		this->denInt = denInt;

	} else if (denRoot) {
		this->denRoot = denRoot;
	} else {
		throw "No valid Denominator";
	}

}

Fraction::Fraction(const Fraction& orig) {
	//copy Constructor
	//set everything to NULL
	numInt = NULL;
	denInt = NULL;
	denRoot = NULL;

	if (orig.numInt) {
		numInt = new Integer(*orig.numInt);
	}
	if (orig.denInt) {
		denInt = new Integer(*orig.denInt);
	}
	if (orig.denRoot) {
		denRoot = new Root(*orig.denRoot);
	}

	if (Expression::debug) {
		cout << "Copy Constructor Called. New Fraction: " << getRep() << endl;
	}

	//Copy Constructor
}

Fraction::~Fraction() {
	delete (numInt);
	delete(denInt);
	delete(denRoot);
}
//Output
//get methods

Number* Fraction::getNumInt() {
	return numInt;
}

Number* Fraction::getDenInt() {
	return denInt;
}

Number* Fraction::getDenRoot() {
	return denRoot;
}

string Fraction::getRep() {
	std::ostringstream rep;


	if (numInt) {
		rep << "(" << numInt->getRep() << "/";
	}

	if (denInt) {
		rep << denInt->getRep();

	} else if(denRoot){
		rep << denRoot->getRep();

	}
	rep << ")";

	return rep.str();
}

double Fraction::getEstimation() {
	double numApprox = 0;
	double denApprox = 0;

	numApprox = numInt->getEstimation();
	if (denInt) {
		denApprox = denInt->getEstimation();
	} else {//if(denRoot)
		denApprox = denRoot->getEstimation();
	}

	return numApprox / denApprox;
}

bool Fraction::isRational() {
	if (numInt && denInt) {
		return true;
	} else {
		return false;
	}

}

Expression* Fraction::simplify() {
	if(numInt->getValue() == 0)
	{
		Expression* returnExp = new Expression(numInt);
		return returnExp;
	}

	if (Expression::debug) {
		cout << "Fraction.simplify() called." << endl;
	}

	// a = original numerator, b = original denominator, c = copy of original numerator, d = copy of original denominator
	if (isRational()) {

		if (Expression::debug) {
			cout << "Fraction is rational. Simplifying numerator and denominator" << endl;
		}

		int a = numInt->getValue();
		int b = denInt->getValue();
		bool isNegative = false;

		if (a < 0 && b > 0) {
			isNegative = true;
			a = a*-1;
		} else if (b < 0 && a > 0) {
			isNegative = true;
			b *= -1;
		} else if (a < 0 && b < 0) {
			isNegative = true;
			a = a * -1;
			b = b * -1;
		}

		int c = a;
		int d = b;
		while (c != d) //Euclidean algorithm for finding greatest common factor
		{
			if (c > d) {
				c = c - d;
			} else {
				d = d - c;
			}
		}
		if (isNegative) {
			a = a * -1;
		}

		//so if the negative was on the bottom before, this should switch it.
		if (c == 1 && a == numInt->getValue()) { //c == 1 then there was no greatest common factor and it means its already simplified
			if (Expression::debug) {
				cout << "Fraction could not be simplified. Returning NULL." << endl;
			}
			return NULL;
		} else { //divides by the greatest common factor

			delete(numInt);
			delete(denInt);
			numInt = NULL;
			denInt = NULL;

			a = a / c;
			b = b / c;

			//create the two Integer objects, but with Number Pointers
			numInt = new Integer(a);
			denInt = new Integer(b);
			if (b == 1) {
				Expression* returnExp = new Expression(numInt);

				return returnExp;
			} else {

				//put the Integer objects in a new Fraction
				Number* returnFrac = new Fraction(numInt, denInt);

				//put that fraction into an expression
				Expression* returnExp = new Expression(returnFrac);
				return returnExp;
			}
		}
		cout << "this should never happen";
		return NULL;
	}//if the den is a Root
	else if (denRoot) {
            
            
		//AND the numerator is an Integer...
		//a is a copy of the numerator, b is the number inside of the root , c = root value, d = c-1
		Integer* base = dynamic_cast<Integer*>(denRoot->getRoot());
		Number* a = new Integer(*numInt);
		Number* b = Expression::copyNumber(denRoot->getValue());
		Integer* c = new Integer(*base);

		int d = c->getValue() - 1;
		//turning d into a number object
		Number* e = new Integer(d);

                Number* fractionBase = Expression::copyNumber(b);
                Fraction* returnFrac = new Fraction(a,fractionBase);

		Expression* returnExp = new Expression(returnFrac);

                returnExp->mult(denRoot);
		
                
                if(Expression::debug) {
                    cout << "Fraction base root Expression: " << returnExp->getRep() << " (before multiplication)" << endl;
                }
                

		delete(returnFrac); returnFrac = NULL;
		//delete newRootValue; newRootValue = NULL;
		delete(a); a = NULL;
		delete(b); b = NULL;
		delete(c); c = NULL;
		delete(e); e = NULL;

		return returnExp;

	}

	return NULL;
}

bool Fraction::equals(Number* number) {
	Fraction* frac = dynamic_cast<Fraction*> (number);
	if (frac) {
		if (numInt->equals(frac->numInt)) {

			if (denInt && frac->denInt) {
				if (denInt->equals(frac->denInt)) {
					return true;
				}
			} else if (denRoot && frac->denRoot) {
				if (denRoot->equals(frac->denRoot)) {
					return true;
				}
			}
		}

	}
	return false;
}

Expression* Fraction::add(Number* number) {
	if (denRoot) {
		throw "Must simplify before performing operation";
	} else {
		Fraction* frac = dynamic_cast<Fraction*> (number);
		Integer* integer = dynamic_cast<Integer*> (number);

		//if what we're dealing with is a fraction..
		if (frac) {
			//and that fraction is rational (aka, it's an integer over an integer)
			if (isRational() && frac->isRational()) {
				//let's get the numerator and denominator out of frac
				//a = numInt(think of it as the numerator of fraction1, b = denominator of fraction1, c = numerator of fraction2, d= denominator of fraction 2
				int a = numInt->getValue();
				int b = denInt->getValue();
				int c = frac->numInt->getValue();
				int d = frac->denInt->getValue();

				bool isNegativeA = false;
				bool isNegativeC = false;

				//negative checks for a/b
				if(b < 0 && a > 0) {
					b = b * -1;
					isNegativeA = true;
				}
				else if(b > 0 && a < 0) {
					a= a * -1;
					isNegativeA = true;
				}
				else if(b < 0 && a < 0){
					a*=-1;
					b*=-1;
				}


				//negative checks for c/d
				if(d < 0 && c > 0) {
					d*=-1;
					isNegativeC = true;
				}
				else if(d > 0 && c < 0) {
					c*=-1;
					isNegativeC = true;
				}
				else if(b < 0 && a < 0){
					a*=-1;
					b*=-1;
				}

				/*a = a*d;
				b = b*d;
				c = c*b;
				d = d*b;
				 */
				int e = d * a;
				int f = d * b;
				int g = b * c;
				//int h = b * d;

				if(isNegativeA)
					e*=-1;

				if(isNegativeC)
					g*=-1;

				int l = e + g;

				delete(numInt);
				delete(denInt);
				numInt = NULL;
				denInt = NULL;

				//create the two Integer objects, but with Number Pointers
				numInt = new Integer(l);
				denInt = new Integer(f);

				//return simplify(); THIS DOES NOT WORK. JUST BECAUSE IT DIDN'T
				//SIMPLIFY DOESN'T MEAN THE ADD FAILED
				Expression* simpExp = simplify();

				if (simpExp) {
					return simpExp;
				} else { //if the simplify failed, that doesn't mean the add failed.
					//just return the current instance in an expression in that
					//case.

					Expression* returnExp = new Expression(this);

					return returnExp;
				}
			}

		} else if (integer) {
			Integer* denom = new Integer(1);
			Number* returnFrac = new Fraction(integer, denom);
			delete(denom);
			return add(returnFrac);

		}
	}
	return NULL;
}

Expression* Fraction::sub(Number* number) {
	if (denRoot) {
		throw "Must simplify before performing operation";
	} else {
		Fraction* frac = dynamic_cast<Fraction*> (number);
		Integer* integer = dynamic_cast<Integer*> (number);

		//if what we're dealing with is a fraction..
		if (frac) {
			//and that fraction is rational (aka, it's an integer over an integer)
			if (isRational() && frac->isRational()) {
				//let's get the numerator and denominator out of frac
				//a = numInt(think of it as the numerator of fraction1, b = denominator of fraction1, c = numerator of fraction2, d= denominator of fraction 2
				int a = numInt->getValue();
				int b = denInt->getValue();
				int c = frac->numInt->getValue();
				int d = frac->denInt->getValue();

				bool isNegativeA = false;
				bool isNegativeC = false;

				//negative checks for a/b
				if(b < 0 && a > 0) {
					b = b * -1;
					isNegativeA = true;
				}
				else if(b > 0 && a < 0) {
					a= a * -1;
					isNegativeA = true;
				}
				else if(b < 0 && a < 0){
					a*=-1;
					b*=-1;
				}


				//negative checks for c/d
				if(d < 0 && c > 0) {
					d*=-1;
					isNegativeC = true;
				}
				else if(d > 0 && c < 0) {
					c*=-1;
					isNegativeC = true;
				}
				else if(b < 0 && a < 0){
					a*=-1;
					b*=-1;
				}

				/*a = a*d;
					b = b*d;
					c = c*b;
					d = d*b;
				 */
				int e = d * a;
				int f = d * b;
				int g = b * c;
				//int h = b * d;

				if(isNegativeA)
					e*=-1;

				if(isNegativeC)
					g*=-1;

				int l = e - g;

				delete(numInt);
				delete(denInt);
				numInt = NULL;
				denInt = NULL;

				//create the two Integer objects, but with Number Pointers
				numInt = new Integer(l);
				denInt = new Integer(f);

				//return simplify(); THIS DOES NOT WORK. JUST BECAUSE IT DIDN'T
				//SIMPLIFY DOESN'T MEAN THE ADD FAILED
				Expression* simpExp = simplify();

				if (simpExp) {
					return simpExp;
				} else { //if the simplify failed, that doesn't mean the add failed.
					//just return the current instance in an expression in that
					//case.

					Expression* returnExp = new Expression(this);

					return returnExp;
				}
			}

		} else if (integer) {
			Integer* denom = new Integer(1);
			Number* returnFrac = new Fraction(integer, denom);
			delete(denom);
			return add(returnFrac);

		}
	}
	return NULL;
}


Expression* Fraction::mult(Number* number) {
    
    number = Expression::copyNumber(number);
    
	if (denRoot) {
		throw "Must simplify before performing operation";
	} else {

		Fraction* frac = dynamic_cast<Fraction*> (number);
		Integer* integer = dynamic_cast<Integer*> (number);

		//if what we're dealing with is a fraction..
		if (frac) {
			//and that fraction is rational (aka, it's an integer over an integer)
			if (isRational() && frac->isRational()) {
				//let's get the numerator and denominator out of frac
				//a = numInt(think of it as the numerator of fraction1, b = denominator of fraction1, c = numerator of fraction2, d= denominator of fraction 2
				int a = numInt->getValue();
				int b = denInt->getValue();
				int c = frac->numInt->getValue();
				int d = frac->denInt->getValue();


				a = a * c;
				b = b* d;

				//if the denominator is negative instead of the num,
				//or both the values are negative,
				if((a >0 && b< 0) || (a<0 && b<0)) {
					//make it the other way around.
					a*=-1;
					b*=-1;
				}


				delete(numInt);
				delete(denInt);
				//create the two Integer objects, but with Number Pointers
				numInt = new Integer(a);
				denInt = new Integer(b);

				//return simplify(); THIS DOES NOT WORK. JUST BECAUSE IT DIDN'T
				//SIMPLIFY DOESN'T MEAN THE ADD FAILED
				Expression* simpExp = simplify();

				if (simpExp) {
					return simpExp;
				} else { //if the simplify failed, that doesn't mean the add failed.
					//just return the current instance in an expression in that
					//case.

					Expression* returnExp = new Expression(this);

					return returnExp;
				}
			}

		} else if (integer) {
			Integer* denom = new Integer(1);
			Number* returnFrac = new Fraction(integer, denom);
			delete denom;
			return mult(returnFrac);

		}
	}
	return NULL;
}

Expression* Fraction::div(Number* number) {
    
    number = Expression::copyNumber(number);
	if (denRoot) {
		throw "Must simplify Fraction before performing operation";
                
	} else {
		Fraction* frac = dynamic_cast<Fraction*> (number);
		Integer* integer = dynamic_cast<Integer*> (number);

		//if what we're dealing with is a fraction..
		if (frac) {
			//and that fraction is rational (aka, it's an integer over an integer)
			if (isRational() && frac->isRational()) {
				//let's get the numerator and denominator out of frac
				//a = numInt(think of it as the numerator of fraction1, b = denominator of fraction1, c = numerator of fraction2, d= denominator of fraction 2
				int a = numInt->getValue();
				int b = denInt->getValue();
				int c = frac->numInt->getValue();
				int d = frac->denInt->getValue();


				a = a * d;
				b = b* c;

				//if the denominator is negative instead of the num,
				//or both the values are negative,
				if((a >0 && b< 0) || (a<0 && b<0)) {
					//make it the other way around.
					a*=-1;
					b*=-1;
				}


				delete(numInt);
				delete(denInt);
				//create the two Integer objects, but with Number Pointers
				numInt = new Integer(a);
				denInt = new Integer(b);

				//return simplify(); THIS DOES NOT WORK. JUST BECAUSE IT DIDN'T
				//SIMPLIFY DOESN'T MEAN THE ADD FAILED
				Expression* simpExp = simplify();

				if (simpExp) {
					return simpExp;
				} else { //if the simplify failed, that doesn't mean the add failed.
					//just return the current instance in an expression in that
					//case.

					Expression* returnExp = new Expression(this);

					return returnExp;
				}
			}

		} else if (integer) {
			Integer* denom = new Integer(1);
			Number* returnFrac = new Fraction(integer, denom);
			delete denom;
			return add(returnFrac);

		}
	}
    
    //making root case
    Root* div_root = dynamic_cast<Root*>(number);
    if(div_root) {
        Integer* one = new Integer(1);
        
        Fraction* rootFrac = new Fraction(one, div_root);
        delete(one);
        one = NULL;
        
        Expression* rootExp = new Expression(rootFrac);
        rootExp->mult(this);
        
        return rootExp;
    }
    
	return NULL;
}

Expression* Fraction::exp(Number* number) {
	Fraction* frac = dynamic_cast<Fraction*> (number);
	Integer* integer = dynamic_cast<Integer*> (number);
	//Root* root = dynamic_cast<Root*>(number);
	/* integer class will take care of this
    //if what we are dealing with is an integer
    if(integer) {
            int a = numInt->getValue();
            int b = denInt->getValue();
            for(int i = 0; integer.value()>i;i++){
                    a = a * a;
                    b = b * b;

            }
            delete(numInt);
            delete(denInt);
            //create the two Integer objects, but with Number Pointers
            numInt = new Integer(a);
            denInt = new Integer(b);

            return simplify();

    }
	 */
	if (frac || integer) {

		Expression* exp1 = numInt->exp(number);
		Expression* exp2 = denInt->exp(number);
		exp1->div(exp2);
		delete exp2;

		return exp1;
	}
	return NULL;


}
