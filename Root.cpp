





#include "Root.h"
Root::Root(Number* value, Number* numRoot){
	value = Expression::copyNumber(value);
	numRoot = Expression::copyNumber(numRoot);
	intVal = NULL;
	constVal = NULL;
	root = NULL;
	mult1 = 1;
	
	if(dynamic_cast<Constant*>(value) != 0){

		constVal = dynamic_cast<Constant*>(value);
	}
	if(dynamic_cast<Fraction*>(value)){
		fracVal = dynamic_cast<Fraction*>(value);
	}
	if(dynamic_cast<Integer*>(numRoot) != 0){
		root = dynamic_cast<Integer*>(numRoot);
	}
        if(dynamic_cast<Integer*>(value)){
            intVal = dynamic_cast<Integer*>(value);
        }

	if(intVal){
		if((intVal->getValue() < 0) && (root->getValue() % 2 == 0))
			throw "Can not have an imaginary root";
	}

	if(intVal||constVal){
		if(root->getValue() == 0){
			throw "root is not an integer";
		}
	}






}

Root::Root(const Root& orig)
{
	//Copy Constructor
	intVal = NULL;
	constVal = NULL;
	root = NULL;

	if(orig.intVal){
		intVal = new Integer(*orig.intVal);
	}
	if(orig.constVal){
		constVal = new Constant(*orig.constVal);
	}
	if(orig.root){
		root = new Integer(*orig.root);
	}
}
Root::~Root()
{
	delete (intVal);
	delete root;
	delete (constVal);

}


Number* Root::getValue(){
	if(intVal){

		return intVal;

	}
	else{

		return constVal;
	}

}


Number* Root::getRoot(){
	return root;
}





double Root::getEstimation(){
	double result = 0;

	if(constVal){
		result = pow(constVal->getEstimation(), 1/(root->getEstimation()));
	}
	else if(intVal){
		result = pow(intVal->getEstimation(), 1/(root->getEstimation()));
	}
	//cout << endl << result << endl;
	return result;
}


string Root::getRep(){

	std::ostringstream rep;

	if(root->getValue() == 2)
		rep << "sqrt:";
	else
		rep << root->getRep() << "rt:";

	if(constVal) {
		rep << constVal->getRep();

	}
	else {
		rep << intVal->getRep();

	}
	//cout << rep;

	return rep.str();
}


Expression* Root::simplify(){

	double result = 0;//stores the value of the estimated root
	double simpRoot = 0;//stores the value of whatever the value is raised to e.g. 3rt:8, it would store 3
	double revIntVal = 0;//holds the value of the "value" e.g. 3rt:8 it would hold 8.
	int mult = 1; // holds the value of the multiplier for simplifying later
	int modResult = 1;

	if(constVal){
		return NULL;//roots with constants as their values can't be simplified
	}


	else if(intVal){//checks if the value in the root is an integer
		if(root->getValue() < 0){//if the value of whatever the root is is negative, ie -3rt:8 this loop executes
			simpRoot = root->getEstimation()*-1;//converts the value of the root to a positive for a time being
			revIntVal = intVal->getEstimation();//stores the value of the value
			result = pow(revIntVal, 1/(simpRoot));//evaluates the root and stores it as a double

			if(result == floor(result)){//checks to see if the evaluation of the root is a whole number
				Number* denNum = new Integer(result);//since the root is negative to begin with, have to create a fraction as the answer. 
				Number* numNum = new Integer(1);//this line holds the numerator, which will always be one
				Number* frac = new Fraction(numNum, denNum);//creates the fraction here
				Expression* expr = new Expression(frac);//create expression to hold the fraction
				return expr;//return expression
				delete(denNum);
				delete(numNum);
				delete(frac);
			}
                       /* else if(result-floor(result) <= .00001){
                                Number* denNum = new Integer(floor(result));//since the root is negative to begin with, have to create a fraction as the answer. 
				Number* numNum = new Integer(1);//this line holds the numerator, which will always be one
				Number* frac = new Fraction(numNum, denNum);//creates the fraction here
				Expression* expr = new Expression(frac);//create expression to hold the fraction
				return expr;//return expression
				delete(denNum);
				delete(numNum);
				delete(frac);
                        }
                        else if(result-floor(result) > .999){
                                Number* denNum = new Integer(floor(result)+1);//since the root is negative to begin with, have to create a fraction as the answer. 
				Number* numNum = new Integer(1);//this line holds the numerator, which will always be one
				Number* frac = new Fraction(numNum, denNum);//creates the fraction here
				Expression* expr = new Expression(frac);//create expression to hold the fraction
				return expr;//return expression
				delete(denNum);
				delete(numNum);
				delete(frac);
                        }
                        */
			else{//if the root does not simplify to a whole number

				vector<int> factors;
				int temp = revIntVal;
				for(int x = 2; x <= temp; x++){
					if(temp % x == 0){
						factors.push_back(x);
						temp = temp/x;
						x = 1;

					}
				}




				vector<int> vars(factors);



				vars.erase(unique(vars.begin(), vars.end()),vars.end());

				vector<int> varCount;//keeps track of the number of times each factor occurs


				for(int i = 0; i < vars.size(); i++){//counts the number of times each factor occurs
					varCount.push_back(count(factors.begin(), factors.end(), vars.at(i)));
				}

				for(int i = 0; i < vars.size(); i++){
					if(varCount[i] >= simpRoot){
						modResult = varCount[i]%(int)simpRoot;
						mult *= (pow(vars[i], (varCount[i]-modResult)));
					}
				}

				if(mult == 1 ){
                                    return NULL;
				}

				else{
					revIntVal = revIntVal/mult;
                                        
					double result1;
					result1 = pow((double)mult, (1/simpRoot))*-1;
                                        int i;
                                        if(result1 == floor(result1) || (result1-floor(result1)< .001)){
                                            i = (int)result1;
                                        }
                                        else if(result1-floor(result1) >.998){
                                            i = (int)result1+1;
                                        }
					Number* newMult = new Integer(i);
                                        if(revIntVal == 1){
                                            Number* int1 = new Integer(1);
                                            Number* frac = new Fraction(int1,newMult);
                                            Expression* expr = new Expression(frac);
                                            return expr;
                                        }
                                        else{
					Number* newBase = new Integer(revIntVal);
					Number* newPow = new Integer(simpRoot);
					Number* newRoot = new Root(newBase, newPow);
					int uno = 1;
					Number* one = new Integer(uno);


					Number* frac = new Fraction(one, newRoot);
					Number* multFrac = new Fraction(one, newMult);
					//cout <<endl <<"madeithere"<<endl;
					Expression* expr = new Expression(multFrac);
                                        expr->mult(frac);
                                        cout << expr->getRep();
					return expr;
					delete(newMult);
					delete(newBase);
					delete(newPow);
					delete(newRoot);
					delete(one);
					delete(frac);
					delete(multFrac);  
                                        }
				}
			}

		}
		else if(intVal->getValue() < 0){//runs if the value inside the root is less than zero
			revIntVal = intVal->getEstimation()*-1;//converts the value inside the root to a positive for the time being
			simpRoot = root->getEstimation();
			result = pow(revIntVal, 1/(root->getEstimation()));
			result *= -1;
			if(floor(result) == result){

				Number* newNum = new Integer(floor(result));
				Expression* expr = new Expression(newNum);
				//cout << endl << result<< endl;
				return expr;
				delete(newNum);
			}
			/*else if(result-floor(result) > .999){
				Number* newNum = new Integer(floor(result)+1);
				Expression* expr = new Expression(newNum);
				//cout << endl << result<< endl;
				return expr;
				delete(newNum);
			}
                        else if(floor(result) == result){
                                Number* newNum = new Integer(result);
				Expression* expr = new Expression(newNum);
				//cout << endl << result<< endl;
				return expr;
				delete(newNum);
                        }
                         * */

			else{

				vector<int> factors;
				int temp = revIntVal;
				int x;
				for(x = 2; x <= temp; x++){
					if(temp % x == 0){
						factors.push_back(x);
						temp = temp/x;
						x = 1;

					}
				}

				vector<int> vars(factors);

				vars.erase(unique(vars.begin(), vars.end()),vars.end());

				vector<int> varCount;

				for(int i = 0; i < vars.size(); i++){
					varCount.push_back(count(factors.begin(), factors.end(), vars.at(i)));
				}

				for(int i = 0; i < vars.size(); i++){

					if(varCount[i] >= simpRoot){
						modResult = varCount[i]%(int)simpRoot;
						mult *= (pow(vars[i], (varCount[i]-modResult)));
					}

				}
				if(mult == 1 ){
                                    return NULL;
				}
				else{

					revIntVal = revIntVal/mult;

					double result1;
					result1 = pow((double)mult, (1/simpRoot))*-1;
                                        int i;
                                        if(result1 == floor(result1) || (result1-floor(result1)< .001)){
                                            i = (int)result1;
                                        }
                                        else if(result1-floor(result1) >.998){
                                            i = (int)result1+1;
                                        }
					Number* newMult = new Integer(i);
                                        if(revIntVal == 1){
                                            Expression* expr = new Expression(newMult);
                                            return expr;
                                        }
					Number* newBase = new Integer(revIntVal);
					Number* newPow = new Integer(simpRoot);
					Number* newRoot = new Root(newBase, newPow);
					//cout << newRoot->getRep();
					Expression* expr = new Expression(newMult);
					expr->mult(newRoot);
					//cout << expr->getRep();
					delete(newMult);
					delete(newBase);
					delete(newPow);
					delete(newRoot);
					return expr;
				}
			}

		}
		else{//no negatives in either case
			revIntVal = intVal->getEstimation();
			//cout << endl << revIntVal << endl;
			simpRoot = root->getEstimation();
			//cout << endl <<simpRoot << endl;
			result = pow(revIntVal, 1/(simpRoot));
			if(floor(result) == result){

				Number* newNum = new Integer(result);
				Expression* expr = new Expression(newNum);
				//cout << endl << result<< endl;
				return expr;
				delete(newNum);
			}
			/*else if(result-floor(result) > .999){
				Number* newNum = new Integer(floor(result)+1);
				Expression* expr = new Expression(newNum);
				//cout << endl << result<< endl;
				return expr;
				delete(newNum);
			}
                        else if(floor(result) == result){
                            Number* newNum = new Integer(result);
				Expression* expr = new Expression(newNum);
				//cout << endl << result<< endl;
				return expr;
				delete(newNum);
                        }
*/
			else{





				vector<int> factors;
				int temp = revIntVal;

				for(int x = 2; x <= temp; x++){
					if(temp % x == 0){
						factors.push_back(x);
						temp = temp/x;
						x = 1;

					}
				}



				vector<int> vars(factors);



				vars.erase(unique(vars.begin(), vars.end()),vars.end());

				vector<int> varCount;


				for(int i = 0; i < vars.size(); i++){
					varCount.push_back(count(factors.begin(), factors.end(), vars.at(i)));
				}





				for(int i = 0; i < vars.size(); i++){

					if(varCount[i] >= simpRoot){
						modResult = varCount[i]%(int)simpRoot;
						mult *= (pow(vars[i], (varCount[i]-modResult)));
					}

				}
				if(mult == 1 ){
                                    return NULL;
				}
				else{
					revIntVal = revIntVal/mult;

					double result1;
					result1 = pow((double)mult, (1/simpRoot));
					
                                        int i;
                                        if(result1 == floor(result1) || (result1-floor(result1)< .001)){
                                            i = (int)result1;
                                        }
                                        else if(result1-floor(result1) >.998){
                                            i = (int)result1+1;
                                        }
                                        Number* newMult = new Integer(i);
                                         if(revIntVal == 1){
                                            Expression* expr = new Expression(newMult);
                                            return expr;
                                        }
					Number* newBase = new Integer(revIntVal);
					Number* newPow = new Integer(simpRoot);
					Number* newRoot = new Root(newBase, newPow);
					Expression* expr = new Expression(newMult);
					expr->mult(newRoot);
					//cout << expr->getRep();
					delete(newMult);
					delete(newBase);
					delete(newPow);
					delete(newRoot);
					return expr;
				}

			}





		}     
        }
                else return NULL;

        
	return NULL;
}

bool Root::equals(Number* num){

	Root* obj = dynamic_cast<Root*>(num);
	if(obj){
		if(root->equals(obj->root)){

			if(intVal && obj->intVal) {
				if(intVal->equals(obj->intVal)){
					return true;
				}
			}
			else if(constVal && obj->constVal){
				if(constVal->equals(obj->constVal)){
					return true;
				}
			}
		}
	}
	return false;
}



Expression* Root::add(Number* number){
	Root* obj = dynamic_cast<Root*>(number);



	if(obj){
		if(constVal && obj->constVal){

			if(root->getValue() == obj->root->getValue() && constVal->getEstimation() == obj->constVal->getEstimation()){
				Number* two = new Integer(2);
				Number* newRoot = new Root(constVal, root);
				Expression* expr = new Expression(two);
				expr->mult(newRoot);
				delete(two);
				delete(newRoot);
				return expr;
			}
		}
		else if(intVal && obj->intVal){

			if(root->getValue() == obj->root->getValue() && intVal->getValue() == obj->intVal->getValue()){
				Number* two = new Integer(2);
                                
                                int res = (intVal->getValue()*((int)pow(two->getEstimation(),root->getEstimation())));
                                Number* rest = new Integer(res);
                                intVal = dynamic_cast<Integer*>(rest);
				//cout << endl << "two" << endl;
				Number* newRoot = new Root(intVal,root);
				simplify();

				
			}
		}
		else return NULL;

	}
	return NULL;
}

/*
//new add method for Salim
Expression* Root::add(Number* number){
	Root* obj = dynamic_cast<Root*>(number);



	if(obj){
		if(constVal && obj->constVal){

			if(root->getValue() == obj->root->getValue() && constVal->getEstimation() == obj->constVal->getEstimation()){


                            Number* newRoot = new Root(constVal, root);
                            mult1 = 2;//this will be used to multiply the leading number if newRoot is simplified
                            newRoot->simplify();
			}
		}
		else if(intVal && obj->intVal){

			if(root->getValue() == obj->root->getValue() && intVal->getValue() == obj->intVal->getValue()){

				Number* newRoot = new Root(intVal,root);
				mult1 = 2;
                                newRoot->simplify();


			}
		}
		else return NULL;

	}
	return NULL;
}

 */

Expression* Root::sub(Number* value){
	Root* obj = dynamic_cast<Root*>(value);
	if(obj){
		if(constVal && obj->constVal){

			if(root->getValue() == obj->root->getValue() && constVal->getEstimation() == obj->constVal->getEstimation()){

				Expression* expr = new Expression();

				return expr;

			}
		}
		else if(intVal && obj->intVal){

			if(root->getValue() == obj->root->getValue() && intVal->getValue() == obj->intVal->getValue()){
				Number* zero = new Integer(0);
				Expression* expr = new Expression(zero);
				//cout << "0" << endl;
				return expr;
                                delete(intVal);
                                delete(root);
                                delete(zero);
			}
		}
		else return NULL;

	}
	return NULL;

}


Expression* Root::mult(Number* value){
	Root* obj = dynamic_cast<Root*>(value);
	if(obj){
		if(constVal && obj->constVal){
			if((root->getValue() == 2) && root->getValue() == obj->root->getValue() && constVal->getEstimation() == obj->constVal->getEstimation()){
				Number* val = new Constant(*obj->constVal);
				Expression* expr = new Expression(val);
				delete(val);
				return expr;
			}
			else{
				return NULL;
			}
		}
		else if (intVal && obj->intVal){
			if((root->getValue() == 2) && root->getValue() == obj->root->getValue() && intVal->getValue() == obj->intVal->getValue()){
				Number* val = new Integer(obj->intVal->getValue());
				Expression* expr = new Expression(val);
				delete(val);
				return expr;
			}
			else if(root->getValue() == obj->root->getValue()){
				Number* inte = new Integer((intVal->getValue())*(obj->intVal->getValue()));
				intVal = dynamic_cast<Integer*>(inte);
				Number* newRoot = new Root(intVal, root);
				Expression* retExpr = simplify();
                                if(retExpr){
                                    return retExpr;
                                }
                                
                                else{
                                    Expression* expr = new Expression(newRoot);
                                    return expr;
                                    delete(newRoot);
                                }
			}
			else{
				return NULL;
			}
		}
	}
	return NULL;
}


Expression* Root::div(Number* value){
	Root* obj = dynamic_cast<Root*>(value);
	if(obj){
		if(constVal && obj->constVal){

			if(root == obj-> root && constVal == obj->constVal){
				Number* one = new Integer(1);
				Expression* Expr = new Expression(one);
				return Expr;
                                delete(one);
			}
			else if (root == obj->root && constVal != obj->constVal){
                            return NULL;
			}
		}
		else if(intVal && obj->intVal){
			if(root->getValue() == obj->root->getValue() && intVal == obj->intVal){
				Number* one = new Integer(1);
				Expression* Expr = new Expression(one);
				delete(one);
				return Expr;
			}
			else if (root == obj->root && intVal != obj->intVal){
				Number* frac = new Fraction(intVal, obj->intVal);
				Number* newRoot = new Root(frac, root);
				Expression* expr = new Expression(newRoot);
				delete(frac);
				delete(newRoot);

				return expr;

			}

		}
		else return NULL;
	}
	return NULL;

}


Expression* Root::exp(Number* power){
	if(dynamic_cast<Integer*>(power)!= 0){
		intPow = dynamic_cast<Integer*>(power);
	}
        else
            intPow = NULL;
	if(dynamic_cast<Fraction*>(power)!=0){
		fracPow = dynamic_cast<Fraction*>(power);
	}
        else
            fracPow = NULL;
	if(intPow){
           // cout <<"Root Exponentiate called"<<endl;
		if(constVal){
			
		}
		else if(intVal){
                    double i = pow(intVal->getEstimation(),intPow->getEstimation());
                    if(i-floor(i) < .001 || i == floor(i)){
                        int j = (int)i;
                        Number* int1 = new Integer(j);
                        intVal = dynamic_cast<Integer*>(int1);
                        Number* newRoot = new Root(intVal,root);
                        
                        Expression* retExpr = simplify();
                        if(retExpr){
                            return retExpr;
                        }
                        else{
                        
                            Expression* expr = new Expression(newRoot);
                            return expr;
                            delete(newRoot);
                       }
                    }
                    else if(i-floor(i) > .998){
                        int j = (int)i + 1;
                        Number* int1 = new Integer(j);
                        intVal = dynamic_cast<Integer*>(int1);
                        Number* newRoot = new Root(intVal,root);
                        Expression* retExpr = simplify();
                    
                        if(retExpr){
                            retExpr;
                        }
                        else{
                        
                            Expression* expr = new Expression(newRoot);
                            return expr;
                            delete(newRoot);
                       }
                         
                        
                    }
                }
		
		else return NULL;
        }

	
	else if(fracPow){
		

		if(intVal){
                    int j;
                    int h;
                    double i = pow(intVal->getEstimation(),fracPow->getNumInt()->getEstimation());
                    if(i - floor(i) < .001 || i == floor(i)){
                        j = (int)i;
                        
                    }
                    else if(i-floor(i) > .998)
                        j = (int)i + 1;
                    Number* int1 = new Integer(j);
                    intVal = dynamic_cast<Integer*>(int1);
                    double newBase = ((root->getEstimation())*fracPow->getDenInt()->getEstimation());
                    if(newBase-floor(newBase) < .001 || newBase == floor(newBase))
                        h = (int)newBase;
                    else if(newBase-floor(newBase) > .998)
                        h = (int)newBase+1;
                    Integer* int2 = new Integer(h);
                    root = dynamic_cast<Integer*>(int2);
                    Number* newRoot = new Root(intVal, root);
                    Expression* retExpr = simplify();
                    if(retExpr)
                        return retExpr;
                    else{
                        Expression* expr = new Expression(newRoot);
                        return expr;
                    }
		}
		else if (constVal){
			Number* newRoot = new Root(constVal,fracPow);
			Expression* expr = new Expression(newRoot);
			delete(newRoot);
			return expr;
		}





	}


	//}
	return NULL;
}
bool Root::isRational(){
	return false;
}



