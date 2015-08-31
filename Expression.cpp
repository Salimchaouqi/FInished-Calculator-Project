/* 
 * Author: max
 * 
 * Version 2.0
 */

#include "Expression.h"
#include "Fraction.h"
#include "Number.h"
#include "Constant.h"
#include "Root.h"
#include "Integer.h"
#include "Log.h"

bool Expression::debug = false; // Definition of our Debug Variable

//default constructor makes it a half exp with a value of zero

Expression::Expression() {
    num1 = NULL;
    num2 = NULL;
    exp1 = NULL;
    exp2 = NULL;

    num1 = new Integer(0);
}

Expression::Expression(const Expression& orig) {

    //check if each of the variables in the orig expression
    //If they are, create a copy of the number. Otherwise set the pointer to NULL.

    if (orig.num1)
        num1 = copyNumber(orig.num1);
    else
        num1 = NULL;

    if (orig.num2)
        num2 = copyNumber(orig.num2);
    else
        num2 = NULL;

    if (orig.exp1)
        exp1 = new Expression(*orig.exp1);
    else
        exp1 = NULL;

    if (orig.exp2)
        exp2 = new Expression(*orig.exp2);
    else
        exp2 = NULL;

    op = orig.op;

}

Expression::~Expression() {
    delete(num1);
    delete(num2);
    delete(exp1);
    delete(exp2);

    num1 = NULL;
    num2 = NULL;
    exp1 = NULL;
    exp2 = NULL;
}

Expression::Expression(Number* num) {

    //Expression::debug = false;

    num = copyNumber(num);

    num1 = NULL;
    num2 = NULL;
    exp1 = NULL;
    exp2 = NULL;
    op = ' ';



    if (debug)
        cout << "Expression(Number* " << num->getRep() << ") constructor called." << endl;

    //Try to simplify the number first
    if (debug)
        cout << "Exp: Trying to simplify " << num->getRep() << " before doing anything." << endl;

    Expression* simplifiedExp = num->simplify();

    //if the Number could simplify to an Expression...
    if (simplifiedExp) {
        if (debug)
            cout << simplifiedExp->getRep() << "simplified into an Expression. Dealing with returned Expression." << endl;
        //try to simplify that expression into a single Number
        Number* simplifiedNum = simplifiedExp->simplify();

        //if it could simplify into a single number,
        if (simplifiedNum) {
            if (debug)
                cout << simplifiedNum->getRep() << " simplified into a single Number. Setting up half Expression." << endl;
            num1 = simplifiedNum;

        } else { //if it could NOT simplify into a single number
            //become the expression Given
            becomeExpression(simplifiedExp);
        }
    } else {//if(!simplifiedExp) { //if the given number could not simplify,

        if (Expression::debug)
            cout << "Number could not simplify. Assigning input to num1." << endl;

        //assign our input to the first slot.
        num1 = num;
        //tell the program we are a halfExp
    }

}

bool Expression::isHalfExp() {
    if (num1 && !num2 && !exp1 && !exp2)
        return true;
    else
        return false;
}

/* =============Number* methods============== */


void Expression::add(Number* addNum) {

    //create a local copy of the number
    addNum = copyNumber(addNum);

    if (debug) {
        cout << "Expression " << getRep() << " add(Number* " << addNum->getRep() << ") called!" << endl;
    }

    //TRY TO SIMPLIFY NUMBER FIRST
    Expression* addExpSimp = addNum->simplify();
    Number* addNumSimp;

    //if the Number simplifies to another expression...
    if (addExpSimp) {
        //check if we can simplify it further to a lone number...
        addNumSimp = addExpSimp->simplify();
        //if it simplifies to a Number...
        if (addNumSimp) {
            delete(addNum);
            addNum = NULL;
            //set addNum equal to that, and continue on with the method.
            addNum = addNumSimp;
        } else { //if we could not get the simplified expression down to a lone number... 
            //call the add(Expression*) method to deal with the number.
            delete(addNum);
            addNum = NULL;
            add(addExpSimp);
        }
    }
    //simplification code over! perform one last check to make sure we are still
    //dealing with a Number addition
    if (addNum) {

        //check if it is a half expression first, since these are our simplest case 
        //(and will never need to access the addHelper method in the case)
        if (isHalfExp()) {

            //is "slot 1" (the only slot filled) a number or an expression?

            //if slot 1 is a number...
            if (num1) {
                //try to just add it directly
                Expression* returnedExp;
                returnedExp = num1->add(addNum);

                //if the add was successful
                if (returnedExp) {
                    //try to get a simplified NUMBER
                    Number* returnedNum = returnedExp->simplify();
                    if (returnedNum) {
                        //if we get a simplified number, delete our old num1 object
                        //and replace it with our new, simplified, one.
                        delete(num1);
                        num1 = returnedNum;
                    } else {//if our returned expression could not be simplified to a single Number,
                        //then we want to extract the variables out of that expression and put it
                        //into this expression. (only because this is the case of the halfExpression,
                        //and we do not want half expressions containing other expressions), as that
                        //would be redundant.

                        becomeExpression(returnedExp);
                    }
                } else {//if the addition was not possible for the lone Number Object,
                    //use the current expression class to represent the added numbers.
                    op = '+';
                    num2 = addNum;
                }

            } else {//if "slot 1" is an expression
                //this should NEVER happen b/c we are dealing with a half expression.
                throw "Expression is a half expression containing only another expression.";
            }

        }//if the current expression is NOT a half Expression...

        else {

            //use the addHelper method to do the legwork, as it is designed to
            if (!addHelper(true, addNum)) {

                //move current instance into the first slot...
                moveInstanceToExp1();

                //set operator
                op = '+';

                //set num1 equal to the thing we attempted to add
                num1 = addNum;

                //done!
            }

        }

    }
}

/* addHelper's job is to tell the add method whether a particular addition simplified 
 or not. From there, the add method will take care of rearranging anything that needs
 rearranging.*/

bool Expression::addHelper(bool firstRun, Number* addNum) {

    if (debug) {
        cout << "addHelper Called(" << addNum->getRep() << ") " << endl;
    }
    //if we are an expression of two Numbers in each "slot", 
    if (num1 && num2) {
        //we are going to try to work with these numbers directly.
        //but first, we must check what operation is going on between the two Numbers,
        //that way, we know what is mathematically appropriate.
        switch (op) {
            case '+':
            {
                //this is the easy one. Just try to add the Number to both Numbers.
                //if that fails, create a new expression with your current two numbers
                //PLUS the number you tried to add. num1 + num2 => exp1 + num1;

                //start with num1

                //attempt to num1 + addNum
                Expression* num1ReturnExp = num1->add(addNum);
                //if the addition was successful...
                if (num1ReturnExp) {

                    //try to get a single Number...
                    Number* num1ReturnNum = num1ReturnExp->simplify();
                    //if the Expression simp was successful...
                    if (num1ReturnNum) {
                        //we're done! woo hoo!
                        //set our num1 slot = to what was just returned.
                        delete(num1);
                        num1 = num1ReturnNum;
                        return true;
                    } else { //if the addition was sucessful but did not return a 
                        //lone Number object...

                        //delete num1, as it is no longer necessary...
                        delete(num1);
                        num1 = NULL;

                        //put the expression in the first "slot"
                        exp1 = num1ReturnExp;

                        //move num2 into the num1 slot...
                        num1 = num2;
                        //blank out num2
                        num2 = NULL;

                        //we are done! Return true!
                        return true;
                    }
                } else if (!num1ReturnExp) {

                    //if num1.add failed, attempt to addNum + num1

                    Expression* addNumReturnExp = addNum->add(num1);
                    //if the addition was successful...
                    if (addNumReturnExp) {

                        //try to get a single Number...
                        Number* addNumReturnNum = addNumReturnExp->simplify();
                        //if the Expression simp was successful...
                        if (addNumReturnNum) {
                            //we're done! woo hoo!
                            //set our num1 slot = to what was just returned.
                            delete(num1);
                            num1 = NULL;
                            num1 = addNumReturnNum;
                            return true;
                        } else { //if the addition was sucessful but did not return a 
                            //lone Number object...

                            //delete num1, as it is no longer necessary...
                            delete(num1);
                            num1 = NULL;

                            //put the expression in the first "slot"
                            exp1 = addNumReturnExp;

                            //move num2 into the num1 slot...
                            num1 = num2;
                            //blank out num2
                            num2 = NULL;

                            //we are done! Return true!
                            return true;
                        }
                    }//if addNum + num1 failed, go ahead and try addNum + num2
                    else {
                        addNumReturnExp = addNum->add(num2);
                        //if the addition was successful...
                        if (addNumReturnExp) {

                            //try to get a single Number...
                            Number* addNumReturnNum = addNumReturnExp->simplify();
                            //if the Expression simp was successful...
                            if (addNumReturnNum) {
                                //we're done! woo hoo!
                                //set our num2 slot = to what was just returned.
                                delete(num2);
                                num2 = NULL;
                                num2 = addNumReturnNum;
                                return true;
                            } else { //if the addition was successful but did not return a 
                                //lone Number object...

                                //delete num2, as it is no longer necessary...
                                delete(num2);
                                num2 = NULL;

                                //put the expression in the first "slot"
                                exp1 = addNumReturnExp;

                                //we are done! Return true!
                                return true;
                            }
                        }
                    }
                } else { //since num1 + addNum, addNum + num1, and addNum + num2 all failed, it's
                    //time to try the last possible combination, num2 + addNum;


                    //attempt to add num2 + addNum2
                    Expression* num2ReturnExp = num2->add(addNum);
                    //if the addition to num2 was successful...
                    if (num2ReturnExp) {
                        //try to get a single Number...
                        Number* num2ReturnNum = num2ReturnExp->simplify();
                        //if the Expression simp was successful...
                        if (num2ReturnNum) {
                            //we're done! woo hoo!
                            //set our num2 slot = to what was just returned.
                            delete(num2);
                            num2 = num2ReturnNum;
                            return true;
                        } else { //if the addition was successful but did not return a 
                            //lone Number object...

                            //delete num2, as it is no longer necessary...
                            delete(num2);
                            num2 = NULL;

                            //put the expression in the first "slot"
                            //since we always want to use exp1 and num1 first.
                            exp1 = num2ReturnExp;
                            return true;

                            //and we're done!
                        }
                    }//if the addition of addNum was unsuccessful for all possible combinations,
                    else {

                        //return FALSE! Because we need to let the base know if such a stunt was impossibe within this particular expression.
                        return false;


                        //and we are done handling addition when the current Expression is added!
                    }
                }
                break;
            }
            case '-':
            {//will I ever have to deal with this? No. I am going to assume I will NOT.
                throw "Operand was subtraction in Expression.";
            }
            case '*':
            {
                //if our current Expression has two Numbers multiplied together
                //and we are trying to add a number to it...

                /* Let's break this down a bit...
                 An expression would only multiply two things together if they could not
                 be represented any other way. Therefore, at LEAST one of the items MUST 
                 be irrational, but it is also possible that they are BOTH irrational.
                 
                 We can then assume immediately, that if addNum is rational, this
                 expression will NOT simplify. */

                //if the number we are trying to add is rational, we can assume
                //this will NOT simplify. 
                //(because the Expression class would only multiply two numbers 
                //if they could not be multiplied any other way)



                //if the number we are adding is rational OR BOTH of the num1 && num2 are irrational,
                if (addNum->isRational() || (!num1->isRational() && !num2->isRational())) {
                    //we know there is no way, in these cases, that this expression can be simplified.
                    //So, just immediately return false, as addition is impossible in this circumstance.

                    return false;

                    //and we're done for this case!
                }//if the number we are trying to add is irrational, AND one of
                    //the numbers in the current, multiplied expression is ALSO irrational,
                else {
                    //there is a chance we can still simplify (i.e. actually add)
                    //this expression, but it only occurs when the irrational number
                    //in the current instance equals the irrational number we are
                    //trying to add. Ex. 2*pi + pi = 3*pi.

                    //so we check if addNum is equal to either num1 || num2 

                    //start with num1..
                    if (num1->equals(addNum)) {
                        //then we need to add one to num2 (because we already know it has to be rational)

                        //create integer with value of 1
                        Integer* one = new Integer(1);

                        //add it to num2
                        Expression* num2AddExp = num2->add(one);
                        delete(one);
                        one = NULL;

                        //get a Number* ready to be extracted from the simplifed num2...
                        Number* num2AddNum = NULL;

                        //make sure we didn't mess up...
                        if (num2AddExp) {
                            //get out our number from the expression...
                            num2AddNum = num2AddExp->simplify();
                            //make sure we didn't mess up again...
                            if (num2AddNum) {
                                //yes! our logic worked! we can now delete num2
                                //and set this returned number equal to that
                                delete(num2);
                                num2 = num2AddNum;
                                return true;

                            } else //if we messed up our logic and the returned expression does not simplify to a number...
                                throw "Rational*Irrational + Irrational Logic Error. Did not return simplified Number.";
                        } else //if we messed up the logic and our returned expression is invalid...
                            throw "Rational*Irrational + Irrational Logic Error. Did not return simplified Expression.";

                    }//now we check num2 applying the same logic if it's true (copy/pasted code)
                    else if (num2->equals(addNum)) {
                        //then we need to add one to num1 (because we already know it has to be rational)

                        //create integer with value of 1
                        Integer* one = new Integer(1);

                        //add it to num1
                        Expression* num1AddExp = num1->add(one);

                        //get a Number* ready to be extracted from the simplified num2...
                        Number* num1AddNum = NULL;

                        //make sure we didn't mess up...
                        if (num1AddExp) {
                            //get out our number from the expression...
                            num1AddNum = num1AddExp->simplify();
                            //make sure we didn't mess up again...
                            if (num1AddNum) {
                                //yes! our logic worked! we can now delete num1
                                //and set this returned number equal to that
                                delete(num1);
                                num1 = num1AddNum;
                                return true;
                            } else //if we messed up our logic and the returned expression does not simplify to a number...
                                throw "Rational*Irrational + Irrational Logic Error. Did not return simplified Number.";
                        } else //if we messed up the logic and our returned expression is invalid...
                            throw "Rational*Irrational + Irrational Logic Error. Did not return simplified Expression.";

                    } else { //if all else fails, we must throw an exception,
                        //because every case has been accounted for and therefore,
                        //something went wrong. Perhaps not even in the Expression class.

                        return false;
                    }
                }

                break;
            }
            case '/':
            case '|':
            {
                //There is no case where a SINGLE NUMBER TYPE added to an expression
                //that has been divided will EVER return a simpler form.

                //This, of course, assumes that all of the logic dealing with factions
                //and their creation has been done correctly.

                //In light of this, lets return false to let outside expressions know that we will not return a simpler form of this.
                return false;
            }
            case '>':
            case '<':
            case 'r':
            case 'R':
            case 'l':
            case 'L':
            {
                /*In every case of rasing a Number to another Number power, if the power is Rational,
                 Every single number type will simplify, including the expression itself.
                 In other words, this operator would not be used unless the POWER the 
                 Number was being raised to was irrational, OR, a NUMBER being RAISED
                 to a power, was irrational. Or, of course, both could be true.
                 
                 Ex. 2^pi, pi^2, e^pi   
                 
                 In any case, this situation plus another number will also NOT simplify.
                 
                 Therefore, let's return false so that outside Expressions know if the add worked */

                return false;
            }
            default:
                throw "Invalid Operator.";
        }
    } else if (exp1 && num1) {
        //this is when things get interesting....

        switch (op) {

                //if a number and expression are added, try to add it to num1 first.
                //if not, run the addHelper method and see if it can be taken care of
                //further down the line.
            case '+':
            {
                /*ATTEMPT TO ADD TO num1 FIRST*/

                Expression* num1ReturnExp = num1->add(addNum);
                //if the addition was successful...
                if (num1ReturnExp) {
                    //try to get a single Number...
                    Number* num1ReturnNum = num1ReturnExp->simplify();
                    //if the Expression simp was successful...
                    if (num1ReturnNum) {
                        //we're done! woo hoo!
                        //set our num1 slot = to what was just returned.
                        delete(num1);
                        num1 = num1ReturnNum;
                        return true;
                    } else { //if the addition was sucessful but did not return a 
                        //lone Number object...
                        //
                        //                        //delete num1, as it is no longer necessary...
                        //                        delete(num1);
                        //                        num1 = NULL;
                        //
                        //                        //put the expression in the first "slot"
                        //                        exp1 = num1ReturnExp;
                        //
                        //                        //move num2 into the num1 slot...
                        //                        num1 = num2;
                        //                        //blank out num2
                        //                        num2 = NULL;
                        //COMPLETELY botched that one! I performed the completely incorrect set of operations!

                        //where were we? Oh yes! 
                        //if the addition was successful but returned an expression that could not be simplified...

                        //delete num1 since it's no longer valid after the addition...
                        delete(num1);
                        num1 = NULL;

                        //fill in the returned expression with exp2
                        exp2 = num1ReturnExp;

                        //set the operator to '+' (just for good measure);
                        op = '+';

                        //addition success! return true to the main add method.
                        return true;
                    }
                } else { //if the addition of num1 w/ addNum was unsuccessful,
                    //try adding the number to exp1;

                    /*ATTEMPT TO ADD TO exp1 !!*/

                    //return the result of whether adding to exp1 worked!
                    return exp1->addHelper(true, addNum);
                }


                break;
            }
            case '-':
                throw "Expression: op was subtraction.";
            case '*':

                /* In the case of addition by a lone number, this would be impossible, 
                 since an expression times a number means that all that stuff would not multiply any
                 other way. */

                return false;

            case '/': //division exp/num1
            case '|'://division num1/exp1                case 'r':
            case 'R':
            case 'l':
            case 'L':
                return false;

            default:
                throw "Expression: Invalid operator exception.";
        }


        /* The reason the actually fails is because I originally never thought two expressions would be ADDED together.
            Turns out, that this actually happens OFTEN, such as in the case 2*pi + 2*e. Soooooooo what I really need to
         *  do is run add helper on both of these expressions.*/

    } else if (exp1 && exp2) {
        switch (op) {
            case '+':
            {
                //this is quite simple. If two expressions are added together, try to add to each of them individually first. 
                //if they both fail, the main add method will take care of rearranging the expression appropriately.

                if (exp1->addHelper(true, addNum)) {
                    return true;
                } else if (exp2->addHelper(true, addNum)) {
                    return true;
                } else {
                    return false;
                }

            }
            case '-':
            {
                throw "Subtracted Expression Expception";
            }//make sure returns are always put so the code does not bleed through
            case '/': //+,-,*,/,|,>,<,r,R,l,L
            case '|':
            case '>':
            case '<':
            case 'r':
            case 'l':
            case 'L':
            {
                //in these cases, no single number can be added to either expression
            }
        }
    } else {
        //if something got screwed up, as this should never happen. So, we throw an exception.v
        throw "Invalid slots configuration while adding";
    }

    return false;

}

bool Expression::addHelper(Expression* addExp) {
    cout << "TODO: Implement addHelper(Expression* addExp)";
    return false;
}

void Expression::sub(Number* sub_num) {
    //COPY Number ZEROth
    sub_num = copyNumber(sub_num);

    //TRY TO SIMPLIFY NUMBER FIRST
    Expression* subSimpExp = sub_num->simplify();
    Number* subSimpNum = NULL;

    //check if it simplified to an expression
    if (subSimpExp) {
        //check if it simplified to a Number
        subSimpNum = subSimpExp->simplify();
        if (subSimpNum) {
            //if it simplified to a single number, change subNum so that's what
            //we are dealing with.
            delete(sub_num);
            sub_num = NULL;
            sub_num = subSimpNum;
        }//if the number simplified to an expression
        else {
            //delete subNum since we Don't need it.
            delete(sub_num);
            sub_num = NULL;
            //pass it onto the expression subtraction method.
            sub(subSimpExp);
        }
    }
    //if we are still dealing with a Number subtraction...
    if (sub_num) {
        //if we are a half exp or full exp with two nums, just try to .sub the other numbers first.

        /* What 1 - (1/2) ? 1.sub will fail */

        //lets multiply (sub_num) by negOne and use our already implemented add methods.
        Integer* neg_one = new Integer(-1);
        
        Expression* neg_sub_exp = new Expression(sub_num);
        
        neg_sub_exp->mult(neg_one);
        
        add(neg_sub_exp);
        
    }


    /*  */
}

void Expression::mult(Number* multNum) {
    //COPY Number ZEROth
    multNum = copyNumber(multNum);

    //TRY TO SIMPLIFY NUMBER FIRST
    if (debug) {
        cout << "Expression " << getRep() << " mult(Number* " << multNum->getRep() << ") called!" << endl;
    }

    //TRY TO SIMPLIFY NUMBER FIRST
    Expression* multExpSimp = multNum->simplify();
    Number* multNumSimp = NULL;

    //if the Number simplifies to another expression...
    if (multExpSimp) {
        //check if we can simplify it further to a lone number...
        multNumSimp = multExpSimp->simplify();
        //if it simplifies to a single Number*
        if (multNumSimp) {
            //set multNum equal to that, and continue on with the method.
            delete(multNum);
            multNum = NULL;
            multNum = multNumSimp;
        } else { //if we could not get the simplified expression down to a lone number... 
            //delete multNum
            delete(multNum);
            multNum = NULL;

            //call the mult(Expression*) method to deal with the number.
            mult(multExpSimp);
        }
    }

    //we need to check if we are multiplied by zero
    Integer* zero_test = dynamic_cast<Integer*> (multNum);
    Integer* zero = new Integer(0);

    if (zero_test && zero_test->equals(zero)) {
        //the entire expression equals zero.
        Expression* zero_exp = new Expression(zero);
        becomeExpression(zero_exp);
    } else


        //if we are still dealing with multiplying a Number...
        if (multNum) {

        if (isHalfExp()) {
            Expression* multSimpExp = num1->mult(multNum);

            //if it returns a simplified expression
            if (multSimpExp) {
                Number* multSimpNum = multSimpExp->simplify();
                //and that turns into a single Number...
                if (multSimpNum) {
                    //delete num1 and replace.
                    delete(multNum);
                    multNum = NULL;
                    multNum = multSimpNum;
                    delete(multSimpExp);
                    multSimpExp = NULL;
                    num1 = multSimpNum;
                }//and if you cannot simplify that expression into a single Number,
                else {
                    //make that the current instance.
                    becomeExpression(multSimpExp);
                }
            } else { //if it could not simplify, just use the number
                op = '*';
                num2 = multNum;
            }
        } else

            if (multNum) {

            multHelper(false, multNum);

            if(op == '*' && num1 && num2 && !exp1 && !exp2) {
                Expression* retExp = num1->mult(num2);

                if (retExp) {
                    Number* retNum = retExp->simplify();
                    if (retNum) {
                        delete(num1);
                        delete(num2);
                        num2 = NULL;
                        num1 = retNum;
                    }
                    else {
                        becomeExpression(retExp);
                    }
                }
            }

            
            /* After all is said and done, try to add back together any of the variables,
             as they may now add together after being multiplied by something.*/

        }

    }

}

void Expression::multHelper(bool has_multed_exp, Number* mult_num) {

    //we can asume mult_num is entirely simplified due to the fact the regular 
    //"mult" method does this as a first step. If it is not simplified before this
    //point, there could be issues.

    //Half Expression Case
    //I believe the half exression case should be handled by the regular mult 
    //method, as it is the method that will first be called. Perhpas we should check
    //if it is a half expression and throw an exception if the value is true.

    if (isHalfExp()) {
        throw "Expression: multHelper called on half Expression.";
    } else
        if (mult_num) { //if we are dealing with a regular expresion...

        //check the state of the current expression
        switch (op) {
                // Valid operators: +,-,*,/,|,>,<,r,R,l,L
            case '+':
                //since in the case of addition, everything must be multiplied
                //no matter what, we can check each variable individually instead
                //of in groups like addition.
            {
                if (exp1 && exp2) {
                    /*Case exp1*/
                    exp1->multHelper(false, mult_num);
                    //try to simplify exp1
                    Number* exp1_num = exp1->simplify();
                    if (exp1_num) {
                        //if it simplifies, exp1 is invalid.
                        delete(exp1);
                        exp1 = NULL;
                        exp1 = exp2;
                        exp2 = NULL;

                        //since it would only be one valid expresion
                        becomeExpression(exp1);
                        //try to add our number using the add method.
                        add(exp1_num);
                    }//if exp1 could not be simplified...ds
                    else {
                        //We are done. We just leave exp1 as is.
                    }

                    /*Case exp2*/
                    exp2->multHelper(false, mult_num);
                    //try to simplify exp2
                    Number* exp2Num = exp2->simplify();
                    if (exp2Num) {
                        //if it simplifies, exp2 is invalid.
                        delete(exp2);
                        exp2 = NULL;
                        //since it would only be one valid expression
                        becomeExpression(exp1);
                        //try to add our number using the add method.
                        add(exp2Num);
                    }//if exp1 could not be simplified...
                    else {
                        //We are done.
                    }
                } else if (exp1 && num1) { //exp1 + num1

                    //try to multiply num1 FIRST
                    Expression* num1_ret_exp = num1->mult(mult_num);

                    //if the multiplication returned a valid expression
                    if (num1_ret_exp) {
                        //we need to check first if can be simplified further into a number
                        Number* num1_ret_num = num1_ret_exp->simplify();
                        //if the num1 return ultimately turns into a number...
                        if (num1_ret_num) {
                            //num1 is invalid
                            delete(num1);
                            num1 = NULL;
                            //multpily expression 1 it to the expression
                            if (exp1->addHelper(true, num1_ret_num)) {
                                //if it simplifies, become exp1
                                becomeExpression(exp1);
                            } else { //it cannot be absorbed into the expression, represent it with this one
                                num1 = num1_ret_num;
                            }
                        } else { //if num1 simplifies but to another expression...
                            delete(num1);
                            num1 = NULL;
                            op = '+';
                            exp2 = num1_ret_exp;
                        }
                    } else { //if num1 did not simplify
                        //create a new expression to represent it.
                        Expression* newExp2 = new Expression(num1);
                        newExp2->mult(mult_num);
                        exp2 = newExp2;
                        //delete and null num1
                        delete(num1);
                        num1 = NULL;
                    }

                    /*Simply multiply exp1 just like above*/
                    exp1->multHelper(false, mult_num);
                    //try to simplify exp1
                    Number* exp1_num = exp1->simplify();
                    //if exp1 simplifies...
                    if (exp1_num) {
                        //if it simplifies, exp1 is invalid.
                        delete(exp1);
                        exp1 = NULL;

                        if (num1 && !exp2 && !num2) {
                            num2 = exp1_num;
                        } else if (exp2 && !num1) {
                            //we must move over exp2 to exp1
                            exp1 = exp2;
                            exp2 = NULL;

                            num1 = exp1_num;
                        } else
                            throw "Expression Multiply Logic Exception.";

                    } else {//if exp1 does not simplify to a SINGLE NUMBER,
                        /* Keep it as is if it does not simplify */
                    }



                } else if (num1 && num2) { //num1 + num2
                    //we are dealing with two nice numbers... :)

                    //Going to initialize some expressions to 
                    //do the work of simplifying for us.
                    // (that way we don't have to type it AGAIN and AGAIN.)
                    Expression* temp1Exp = new Expression(num1);
                    temp1Exp->mult(mult_num);
                    Number* temp1Num = temp1Exp->simplify();

                    Expression* temp2Exp = new Expression(num2);
                    temp2Exp->mult(mult_num);
                    Number* temp2Num = temp2Exp->simplify();

                    if (temp1Num) {
                        delete(num1);
                        delete(temp1Exp);
                        temp1Exp = NULL;
                        num1 = NULL;
                        num1 = temp1Num;
                    } else { //if temp1 is an expression
                        delete(num1);
                        num1 = NULL;
                        exp1 = temp1Exp;
                    }

                    if (temp2Num) {
                        delete(num2);
                        num2 = NULL;

                        if (exp1 && !num1) {
                            num1 = temp2Num;
                        } else if (num1 && !exp1) {
                            num2 = temp2Num;
                        } else {
                            throw "Invalid Expression. You probably forgot to NULL.";
                        }

                    } else { //if temp2 is an Expression
                        delete(num2);
                        num2 = NULL;
                        if (exp1)
                            exp2 = temp2Exp;
                        else {
                            exp1 = temp2Exp;
                        }
                    }
                    //multiply num2

                } else
                    throw "InvAlid Expression. nums and exps mismatch.";
                break;
            }
            case '-':
            {
                throw "Expression Subtraction Exception.";
                break;
            }
            case '*':
            {
                /*OKAY! Here's where the real power of the multHELPER comes into play.
                 We must pay special attention to the has_multed_exp flag that is passed
                 into this method, as that determines whether we should try to even multiply
                 this particular expression.*/

                //num1 * num2
                if (num1 && num2 && !exp1 && !exp2) {

                    //since this is the 'base case' , only multiply if it hasn't
                    //been multiplied yet. Remember: true means it has already
                    //gotten to an expression earlier down the line.
                    if (!has_multed_exp) {
                        //try to multiply num1
                        Expression* num1_exp = num1->mult(mult_num);
                        if (num1_exp) {
                            Number* num1_num = num1_exp->simplify();
                            if (num1_num) {
                                delete(num1);
                                num1 = num1_num;
                            } else {
                                delete(num1);
                                num1 = NULL;
                                exp1 = num1_exp;
                            }
                        } else {
                            if(Expression::debug) {
                                cout << "num2: " << num2->getRep() << " being multiplied by " << mult_num->getRep() <<endl;
                            }
                            //try to multiply num2
                            Expression* num2_exp = num2->mult(mult_num);
                            if (num2_exp) {
                                Number* num2_num = num2_exp->simplify();
                                if (num2_num) {
                                    delete(num2);
                                    num2 = num2_num;
                                } else {
                                    delete(num2);
                                    num2 = NULL;
                                    exp1 = num2_exp;
                                }
                            } else {
                                //all else fails, moveInstanceToExp1, and mult
                                moveInstanceToExp1();
                                num1 = mult_num;
                            }
                        }
                    }
                }//exp1 * num1 
                else if (exp1 && num1 && !num2 && !exp2) {

                    /* This is our first real, "special case" where we take advantage
                     of the has_multed_exp variable. If num1 multiplication works,
                     must must continue to send it down the line for exp1, but with
                     that flag set as false.*/

                    //try to multiply num1, but ONLY if the has_multed is set to false.
                    if (!has_multed_exp) {
                        Expression* num1_ret_exp = num1->mult(mult_num);
                        if (num1_ret_exp) {
                            Number* num1_ret_num = num1_ret_exp->simplify();
                            if (num1_ret_num) {
                                delete(num1);
                                num1 = num1_ret_num;
                                has_multed_exp = true;
                            } else {
                                delete(num1);
                                num1 = NULL;
                                //not sure if this is the best way to do this.
                                exp2 = num1_ret_exp;
                                has_multed_exp = true;
                            }
                        }
                    }

                    //check if num1 mult worked, if it did, multiply exp1 with "true";
                    exp1->multHelper(has_multed_exp, mult_num);

                } else

                    //exp1 * exp2
                    if (exp1 && exp2 && !num1 && !num2) {

                    //we need a mult helper in this instance, that way we know
                    //if something was successfully multiplied or not.

                    //I THINK this case should NEVER happen!
                    if (debug) {
                        cout << "Expression: ERROR! exp1 * exp2 case! Defaulting to exp1.";
                    }

                    exp1->multHelper(has_multed_exp, mult_num);

                } else
                    throw "Invalid Expression. Please check NULL values.";

                break;
            }
            case '/':
            {



                //num1/num2
                if (num1 && num2 && !exp1 && !exp2) {

                    //if what they are trying to multiply equals the bottom,
                    //the expression becomes what was on top.
                    if (mult_num->equals(num2)) {
                        delete(num2);
                        num2 = NULL;
                        delete(mult_num);
                        mult_num = NULL;
                        op = ' ';
                    } else { //every other case
                        //try to multiply the top
                        Expression* num1_ret_exp = num1->mult(mult_num);
                        if (num1_ret_exp) {
                            Number* num1_ret_num = num1_ret_exp->simplify();
                            if (num1_ret_num) {
                                delete(num1);
                                num1 = num1_ret_num;
                            } else {
                                Number* temp = num2;
                                num2 = NULL;
                                becomeExpression(num1_ret_exp);
                                div(temp);
                            }
                        } else { //if it does not simplify, represent it with the current expression.
                            exp1 = new Expression(num1);
                            delete(num1);
                            num1 = num2;
                            num2 = NULL;
                            exp1->mult(mult_num);
                        }
                    }

                }//exp1/num1
                else if (exp1 && num1 && !num2 && !exp2) {

                    //if what they are trying to multiply equals the bottom,
                    //the expression becomes what was on top.
                    if (mult_num->equals(num1)) {
                        delete(num1);
                        num1 = NULL;
                        delete(mult_num);
                        mult_num = NULL;
                        becomeExpression(exp1);
                    } else {
                        //every other case
                        //multiply the top
                        exp1->mult(mult_num);

                        //attempt to simplify
                        Number* exp1_ret_num = exp1->simplify();
                        if (exp1_ret_num) {
                            delete(exp1);
                            exp1 = NULL;
                            Number* tempNum = num1;
                            Expression* newExp = new Expression(exp1_ret_num);
                            becomeExpression(newExp);
                            div(tempNum);
                        }
                    }


                }//exp1/exp2
                else if (exp1 && exp2 && !num1 && !num2) {
                    mult(exp1);

                    //attempt to simplify
                    Number* exp1_ret_num = exp1-> simplify();
                    if (exp1_ret_num) {
                        delete(exp1);
                        exp1 = NULL;
                        Expression* temp_exp = exp2;
                        Expression* newExp = new Expression(exp1_ret_num);
                        becomeExpression(newExp);
                        div(temp_exp);
                    }

                } else
                    throw "Invalid Expression Exception.";



                break;
            }
            case '|':
            {
                //num2/num1 (not sure if this should ever happen)
                //if (num1 && num2 && !exp1 && !exp2) {

                //should not happen.

                //  }//num1/exp1 (this will DEFINITELY happen.)
                if (exp1 && num1 && !num2 && !exp2) {


                    //try to multiply the top
                    Expression* num1_ret_exp = num1->mult(mult_num);
                    if (num1_ret_exp) {
                        Number* num1_ret_num = num1_ret_exp->simplify();
                        if (num1_ret_num) {
                            delete(num1);
                            num1 = num1_ret_num;
                        } else { //if the number "simplifies" to another expression
                            exp2 = exp1;
                            exp1 = NULL;
                            op = '/';
                            exp1 = num1_ret_exp;
                        }
                    } else { //if it does not simplify, represent it with the current expression.
                        exp1 = new Expression(num1);
                        delete(num1);
                        num1 = num2;
                        num2 = NULL;
                        exp1->mult(mult_num);
                    }


                    //    }//exp2/exp1 (not sure if this will happen, either.)
                    //      else if (exp1 && exp2 && !num1 && !num2) {



                }

            }

            case '>':
            case 'l':
            case 'L':
            case 'R':
            case 'r':
            case '<':
            {
                if (num1 && num2 && !exp1 && !exp2) {
                    //num1^num2
                    //I don't think this is ever a case
                    moveInstanceToExp1();
                    op = '*';
                    num1 = mult_num;
                } else if (num1 && !num2 && exp1 && !exp2) {
                    moveInstanceToExp1();
                    op = '*';
                    num1 = mult_num;

                } else if (!num1 && !num2 && exp1 && exp2) {
                    //exp1^exp2
                    moveInstanceToExp1();
                    op = '*';
                    num1 = mult_num;
                } else
                    throw getRep();

                break;
            }

        }

        /* After all is said and done, try to add back together any of the variables,
         as they may now add together after being multiplied by something.*/

    }

}

void Expression::div(Number* divNum) {
    //COPY Number ZEROth
    divNum = copyNumber(divNum);

    if (debug) {
        cout << "Expression " << getRep() << " div(Number* " << divNum->getRep() << ") called!" << endl;
    }

    //TRY TO SIMPLIFY NUMBER FIRST
    Expression* divExpSimp = divNum->simplify();
    Number* divNumSimp = NULL;

    //if the Number simplifies to another expression...
    if (divExpSimp) {
        //check if we can simplify it further to a lone number...
        divNumSimp = divExpSimp->simplify();
        //if it simplifies to a single Number*
        if (divNumSimp) {
            //set divNum equal to that, and continue on with the method.
            delete(divNum);
            divNum = NULL;
            divNum = divNumSimp;
        } else { //if we could not get the simplified expression down to a lone number...
            //delete divNum
            delete(divNum);
            divNum = NULL;

            //call the div(Expression*) method to deal with the number.
            div(divExpSimp);
        }
    }

    //if we are still dealing with dividing a Number...
    if (divNum) {

        //and we are a half expression...
        if (isHalfExp()) {
            Expression* divSimpExp = num1->div(divNum);

            //if it returns a simplified expression
            if (divSimpExp) {
                Number* divSimpNum = divSimpExp->simplify();
                //and that turns into a single Number...
                if (divSimpNum) {
                    //delete num1 and replace.
                    delete(divNum);
                    divNum = NULL;
                    divNum = divSimpNum;
                    delete(divSimpExp);
                    divSimpExp = NULL;
                    num1 = divSimpNum;
                }//and if you cannot simplify that expression into a single Number,
                else {
                    //make that the current instance.
                    becomeExpression(divSimpExp);
                }
            } else { //if it could not simplify, just use the number

                if (Expression::debug)
                    cout << "divide special case test help: (" << num1->getRep() << "/" << divNum->getRep() << ")" << endl;

                //setup for our special fraction case.
                Integer* integer_num1 = dynamic_cast<Integer*> (num1);

                Integer* integer_divNum = dynamic_cast<Integer*> (divNum);

                Root* root_divNum = dynamic_cast<Root*> (divNum);

                Integer* integer_numerator = NULL;

                //okay, so this is our special case when we must invoke a fraction
                if (integer_divNum || root_divNum) {



                    //lets check if num1 is an integer, if so, that goes on
                    if (integer_num1) {

                        //delete(integer_numerator);

                        integer_numerator = integer_num1;

                        //null it out so we can check for this at the end of this case
                        num1 = NULL;
                    } else {
                        integer_numerator = new Integer(1);
                    }

                    if (Expression::debug) {
                        cout << "integer_numerator is " << integer_numerator->getRep() << endl;
                    }

                    Fraction* frac = new Fraction(integer_numerator, divNum);
                    Expression* frac_simp_exp = frac->simplify();


                    //if fraction simplifies to an expression...
                    if (frac_simp_exp) {
                        if (debug) {
                            cout << frac_simp_exp->getRep();
                        }
                        Number* frac_simp_num = frac_simp_exp->simplify();
                        //and if it simplifies further to a number...
                        if (frac_simp_num) {
                            //check if num1 is set...
                            if (num1) {
                                op = '*';
                                num2 = frac_simp_num;
                            } else {
                                //otherwise, the fraction = num1
                                num1 = frac_simp_num;
                            }
                        } else {
                            //if fraction simplifies to an expression...

                            //become the expression and multiply it by num1 (if applicable)
                            Number* tempNum = num1;
                            num1 = NULL;
                            becomeExpression(frac_simp_exp);
                            if (tempNum) {
                                mult(tempNum);
                            }

                        }
                    } else { //if the fraction does not simplify

                        //if num1 is not null...
                        if (num1) {
                            //represent it as being multiplied
                            op = '*';
                            num2 = frac;
                        } else { //if num1 IS null..
                            num1 = frac;
                        }
                    }


                } else {//if it is not our special case where we create a fraction,
                    //then we just represent the current expression with a '/'

                    op = '/';

                    num2 = divNum;

                }

            }
        }/* END OF HALF EXPRESSION CASE!!! BEGIN REGUALR CASE! */
        else {
            bool work = divHelper(divNum);
            
            if(!work) {
                moveInstanceToExp1();
                op = '/';
                
                num1 = divNum;
            }
            
            
        }



        /* After all is said and done, try to add back together any of the variables,
         as they may now add together after being divided by something.*/
    }
    // else


    //else
}

/* divHelper will return true if for current added expression block, it was successfully
 * divided.
 * 
 * The job of the reciever of the "true" is to run the simplify method on the code,
 * attempting to get a number of out of, but otherwise simplifying it even if it does
 * not return anything.
 * 
 * if the reciever gets a "false", 
 */

bool Expression::divHelper(Number* div_num) {

    switch (op) {
            // Valid operators: +,-,*,/,|,>,<,r,R,l,L
        case '+':
        {
            //since in the case of addition, everything must be divided
            //no matter what, we can check each variable individually instead
            //of in groups like add()
            
            //num1 + num2
            if (num1 && num2 && !exp1 && !exp2) {
                
                bool num1_div = false;
                bool num2_div = true;
                
                //attempt to divide  num1.
                Expression* num1_ret_exp = num1->div(div_num);
                if (num1_ret_exp) {
                    Number* num1_ret_num = num1_ret_exp->simplify();
                    if (num1_ret_num) {
                        delete(num1);
                        num1 = num1_ret_num;
                       // num1_div = true;
                    } else {
                        //if division yields an expression
                        delete(num1);
                        num1 = NULL;
                        exp1 = num1_ret_exp;
                        //num1_div = true;
                    }
                } else { //if division by num1 fails

                    //we must create an expression to represent num1.
                    Expression* newExp = new Expression(num1);
                    newExp->div(div_num);
                    exp1 = newExp;
                    //num1_div = true;

                }

                //time to divide num2
                Expression* num2_ret_exp = num2->div(div_num);
                if (num2_ret_exp) {
                    Number* num2_ret_num = num2_ret_exp->simplify();
                    if (num2_ret_num) {
                        delete(num2);
                        num2 = num2_ret_num;

                    } else {
                        //if division yields an expression
                        delete(num2);
                        num2 = NULL;
                        if(exp1)
                            exp2 = num2_ret_exp;
                        else
                            exp1 = num2_ret_exp;
                        //num1_div = true;
                    }
                } else { //if division by num1 fails

                    //we must create an expression to represent num1.
                    Expression* newExp = new Expression(num2);
                    newExp->div(div_num);
                    
                    if(exp1)
                            exp2 = newExp;
                        else
                            exp1 = newExp;
                    //num1_div = true;

                }
                //returns true regardless b/c it HAS to work.
                return true;
                
                //exp1 + num1
            } else if (num1 && !num2 && exp1 && !exp2) {

                //attempt to divide  num1.
                Expression* num1_ret_exp = num1->div(div_num);
                if (num1_ret_exp) {
                    Number* num1_ret_num = num1_ret_exp->simplify();
                    if (num1_ret_num) {
                        delete(num1);
                        num1 = num1_ret_num;
                       // num1_div = true;
                    } else {
                        //if division yields an expression
                        delete(num1);
                        num1 = NULL;
                        exp1 = num1_ret_exp;
                        //num1_div = true;
                    }
                } else { //if division by num1 fails

                    //we must create an expression to represent num1.
                    Expression* newExp = new Expression(num1);
                    newExp->div(div_num);
                    exp1 = newExp;
                    //num1_div = true;

                }
                
                //now the interesting part: exp1
                
                bool exp1_div_success = exp1->divHelper(div_num);
                
                if(exp1_div_success) {
                    //attempt to simplify
                    Number* exp1_simp_num = exp1->simplify();
                    if(exp1_simp_num) {
                        delete(exp1);
                        exp1 = NULL;
                        
                        if(num1)
                            num2 = exp1_simp_num;
                        else
                            num1 = exp1_simp_num;
                    }
                    //if it successfully divided but did not simplify, leave it as is.
                    
                    
                }
                else { //if the expression did NOT successfully divide, we must help
                    //it out.
                    
                    exp1->moveInstanceToExp1();
                    
                }
                


            } else if (!num1 && !num2 && exp1 && exp2) {


            }

            else
                throw getRep();
            break;
        }
        case '-':
        {
            throw "Subtracted Expression Exception";
            break;
        }
        case '*':
        {
            if (num1 && num2) {
                //attempt to divide  num1.
                Expression* num1_ret_exp = num1->div(div_num);
                if (num1_ret_exp) {
                    Number* num1_ret_num = num1_ret_exp->simplify();
                    if (num1_ret_num) {
                        delete(num1);
                        num1 = num1_ret_num;
                        return true;
                    }
                    //if multiplication yields an expression
                    Number* tempNum = copyNumber(num2);
                    becomeExpression(num1_ret_exp);
                    mult(tempNum);
                    return true;
                } else { //if division by num2 fails
                    Expression* num2_ret_exp = num2->div(div_num);
                    if (num2_ret_exp) {
                        Number* num2_ret_num = num2_ret_exp->simplify();
                        if (num2_ret_num) {
                            delete(num2);
                            num2 = num2_ret_num;
                            return true;
                        }
                        //if multiplication yields an expression
                        Number* tempNum = copyNumber(num1);
                        becomeExpression(num2_ret_exp);
                        mult(tempNum);
                        return true;
                    }
                }
                //if successful, return true. If not, return false.
                return false;

                //exp1*num1
            } else if (exp1 && num1) {

                //try to do num1 first
                //attempt to divide  num1.
                Expression* num1_ret_exp = num1->div(div_num);
                if (num1_ret_exp) {
                    Number* num1_ret_num = num1_ret_exp->simplify();
                    if (num1_ret_num) {
                        delete(num1);
                        num1 = num1_ret_num;
                        return true;
                    }
                    //if multiplication yields an expression
                    exp1->mult(num1_ret_exp);
                    becomeExpression(exp1);     //may cause issues, nothing I can do now
                    
                    return true;
                } else { //if num1 does NOT work, try exp1
                    
                    bool exp1_div = exp1->divHelper(div_num);
                    if(exp1_div) {
                        //if exp1 returned true, that means it successfully divided it.
                        //so, we should run simplify, check it if it turned into a number,
                        //and pass true down the line.
                        Number* exp1_simp_num = exp1->simplify();
                        
                        if(exp1_simp_num) {
                            delete(exp1);
                            exp1 = NULL;
                            num2 = exp1_simp_num;
                        }
                        else { 
                            //if it did not simplify, just leave it as is!
                        }
                        return true;
                    }
                    else {
                        return false;
                    }
                    
                }



            } else if (exp1 && exp2) {

                bool exp1_div = exp1->divHelper(div_num);
                if (exp1_div) {
                    //if exp1 returned true, that means it successfully divided it.
                    //so, we should run simplify, check it if it turned into a number,
                    //and pass true down the line.
                    Number* exp1_simp_num = exp1->simplify();

                    if (exp1_simp_num) {
                        delete(exp1);
                        exp1 = NULL;
                        num1 = exp1_simp_num;
                    } else {
                        //if it did not simplify(to a number), just leave it as is!
                    }
                    return true;
                } else {
                    bool exp2_div = exp2->divHelper(div_num);
                    if (exp2_div) {
                        //if exp1 returned true, that means it successfully divided it.
                        //so, we should run simplify, check it if it turned into a number,
                        //and pass true down the line.
                        Number* exp2_simp_num = exp2->simplify();

                        if (exp2_simp_num) {
                            delete(exp2);
                            exp2 = NULL;
                            if(num1)
                                num2 = exp2_simp_num;
                            else {
                                num1 = exp2_simp_num;
                            }
                        } else {
                            //if it did not simplify, just leave it as is!
                        }
                        return true;
                    } else {
                        return false;
                    }
                }


            } else {
                throw getRep();
            }
            break;
        }
        case '/':

            //if what they are trying to multiply equals the bottom,
            //the expression becomes what was on top.
        {
            break;
        }
        case '|':
        {
            break;
        }
        case '>':
        {
            break;
        }
        case '<':
        {
            break;
        }
        case 'r':
        {
            break;
        }
        case 'R':
        {
            break;
        }
        case 'l':
        case 'L':
        {
            break;
        }

    }


    return false;
}

/* NEEDS TO BE ABLE TO HANDLE FRACTIONS */
void Expression::power(Number* power_num) {
    //COPY Number ZEROth
    power_num = copyNumber(power_num);

    //try to simplify number first
    Expression* power_simp_exp = power_num->simplify();
    if (power_simp_exp) {
        Number* power_simp_num = power_simp_exp->simplify();
        if (power_simp_num) {
            delete(power_num);
            power_num = power_simp_num;
        } else {
            delete(power_num);
            power_num = NULL;
            power(power_simp_exp);
        }
    }

    if (power_num) {

        //ANY expression raised to the zero power == 1. So, let's handle that case first.

        //if the power is an integer...
        Integer* power_int = dynamic_cast<Integer*> (power_num);
        Fraction* power_frac = dynamic_cast<Fraction*> (power_num);

        if (power_int) {
            //and that integer's value is zero...
            if (power_int->getValue() == 0) {
                //delete everything in the expression and create a new Integer(1) to put in num1;
                delete(num1);
                delete(num2);
                delete(exp1);
                delete(exp2);
                num1 = NULL;
                num2 = NULL;
                exp1 = NULL;
                exp2 = NULL;

                Number* newExpValue = new Integer(1);
                num1 = newExpValue;

                //make sure we reflect the change that we now only have one number
                //in our expression, once again.
                op = ' ';
            } else { //if the power is an integer but NOT zero
                int power = power_int->getValue();

                if (power > 1) {
                    Expression* mult_exp = new Expression(*this);
                    for (int a = 1; a < power; a++) {
                        mult(mult_exp);
                    }
                } else if (power < 1) {
                    Integer* one = new Integer(1);
                    Expression* one_over_exp = new Expression(one);
                    delete(one);
                    one = NULL;

                    one_over_exp->div(this);
                    becomeExpression(one_over_exp);

                    //after putting everything over 1, we can now call the power
                    //method again with a positive power.
                    delete(power_int);
                    power_int = new Integer(power*-1);
                    this->power(power_int);
                }

            }
        } else if (power_frac) {
            Integer* num = dynamic_cast<Integer*> (power_frac->getNumInt());
            Integer* den = dynamic_cast<Integer*> (power_frac->getDenInt());
            Root* denRoot = dynamic_cast<Root*> (power_frac->getDenRoot());

            if (denRoot) {
                // should never happen
                throw "Cannot take a power with an irrational fraction";
            }
            // make sure the fraction is rational
            if (num && den) {
                // we can leave the numerator as a power, but the denominator becomes a root
                if (power_frac->getEstimation() > 0) {
                    power(num);
                    root(den);
                } else if (power_frac->getEstimation() < 0) {
                    // first take the power, num should be negative
                    if (num->getValue() < 0) {
                        Number* temp = new Integer(num->getValue() * -1);
                        power(temp);
                        delete temp;
                        temp = NULL;
                    } else {
                        throw "The numerator of the power fraction is not negative";
                    }

                    // next we take the root of the current expression, den should be positive
                    root(den);

                    // finally we put the result over 1
                    Integer* one = new Integer(1);
                    Expression* one_over_exp = new Expression(one);
                    delete(one);
                    one = NULL;
                    one_over_exp->div(this);
                    becomeExpression(one_over_exp);
                }
            } else {
                // chances are something didn't simplify
                throw "Fractional power failed, most likely due to a simplification error";
            }
        }
    } else {
        //fail gracefully by representing the number with the expression.
        if (isHalfExp()) {
            op = '>'; //arrow is right because this is the num1 && num2 case
            num2 = power_num;
        }
    }

}

void Expression::root(Number* base_num) {
    if (Expression::debug) {
        cout << "Expression " << getRep() << " root(Number* " << base_num->getRep() << ") called." << endl;
    }

    //COPY Number ZEROth
    base_num = copyNumber(base_num);

    //try to simplify the number taken in FIRST
    Expression* simp_base_exp = new Expression(base_num);

    //try to simplify the base to a single number
    Number* simp_base_num = simp_base_exp->simplify();

    //if it worked, set that to base_num
    if (simp_base_num) {
        delete(base_num);
        base_num = simp_base_num;
        simp_base_num = NULL; //b/c we do not need it anymore.
    } else {
        //if it didn't, delete and NULL base_num and refer it to the Expression method.
        delete(base_num);
        base_num = NULL;

        mult(simp_base_exp);
    }

    //if we are still dealing with a Number* for a base....

    //lets go ahead and cast the base to integer to use for later.
    Number* integer_base = dynamic_cast<Integer*> (base_num);

    if (base_num) {

        if (isHalfExp()) {
            //try to cast num1 to the various different types that ROOT can handle.

            Number* rootable_value = dynamic_cast<Integer*> (num1);
            //we only need to cast it once, so lets do an if/else block so we can
            //reuse the same variable
            if (!rootable_value) {
                rootable_value = dynamic_cast<Constant*> (num1);
            }

            //if it's possible to create a root based on these values, let's do it.
            if (rootable_value && integer_base) {
                //lets create the root!
                Number* root = new Root(rootable_value, integer_base);

                //we need to simplify this root now
                Expression* simp_root_exp = root->simplify();
                if (simp_root_exp) {
                    //try to simplify it further to a number
                    Number* simp_root_num = simp_root_exp->simplify();
                    //if it simplifies...
                    if (simp_root_num) {
                        //go ahead and assign that to num1
                        delete(num1);
                        num1 = simp_root_num;
                    } else { //if it simplifies to an expression only
                        //become the returned expression
                        becomeExpression(simp_root_exp);
                    }
                } else { //if it does not simplify at all...
                    //assign the root directly to num1
                    delete(num1);
                    num1 = root;
                }
            } else { //if we cannot create a "root" based on the current base or 
                //num1, we  need the current expression to represent it.

                //num1 stays the same
                op = 'r'; //since we are going left to right
                num2 = base_num;
            }
        }//end half expression case

        else {

        }



    }


}

void Expression::root(Expression* baseExp) {

    //COPY Number ZEROth
    baseExp = new Expression(*baseExp);

    //TRY TO SIMPLIFY NUMBER FIRST
    if (Expression::debug) {
        cout << "Expression root(Expression* " << baseExp->getRep() << ") called!" << endl;
    }

    //TRY TO SIMPLIFY NUMBER FIRST
    Number* baseNumSimp = baseExp->simplify();

    //if the Expression simplifies to a Number
    if (baseNumSimp) {
        // delete the old expression and just use the number
        delete(baseExp);
        baseExp = NULL;
        // rootExp = rootNumSimp;
        // call the root(Number*) method to deal with the number.
        root(baseNumSimp);
    } else {
        // Expression didn't simplify to a Number
        if (isHalfExp()) {
            // we are a half expression and the parameter is a full expression
            // create an Expression representation
            op = 'r';
            exp2 = baseExp;
        } else {
            // we are not a half expression and the parameter is a full expression
            Expression* copy_exp = new Expression(*this);
            exp1 = copy_exp;
            exp2 = baseExp;
            op = 'r';
        }
    }
}

//Jeff's modified version of LOG for expression

void Expression::log(Number* base) {
    //COPY Number ZEROth
    base = Expression::copyNumber(base);

    //TRY TO SIMPLIFY NUMBER FIRST
    if (debug) {
        cout << "Expression log(Number* " << base->getRep() << ") called!" << endl;
    }

    //TRY TO SIMPLIFY NUMBER FIRST
    Expression* baseExpSimp = base->simplify();
    Number* baseNumSimp;

    //if the Number simplifies to another expression...
    if (baseExpSimp) {
        //check if we can simplify it further to a lone number...
        baseNumSimp = baseExpSimp->simplify();
        //if it simplifies to a single Number*
        if (baseNumSimp) {
            //set divNum equal to that, and continue on with the method.
            delete(base);
            base = NULL;
            base = baseNumSimp;
        } else { //if we could not get the simplified expression down to a lone number...
            //delete divNum
            delete(base);
            base = NULL;

            //call the log(Expression*) method to deal with the number.
            log(baseExpSimp);
        }
    }
    // simplification is finished
    if (base) {
        if (isHalfExp()) {
            Number* log = new Log(base, num1);

            Expression* log_exp = log->simplify();
            if (log_exp) {
                Number* log_num = log_exp->simplify();
                if (log_num) {
                    num1 = log_num;
                } else {
                    becomeExpression(log_exp);
                }
            } else {
                num1 = log;
                op = ' ';
                num2 = NULL;
            }
        } else { //value is a full expression
            if (op == '*') {
                if (num1) { //at least one part is a Number
                    Number* log1 = new Log(base, num1);
                    if (num2) { //if second part is also a Number
                        Number* log2 = new Log(base, num2);
                        Expression* log_exp = new Expression(log1); //turn first Number into Expression (this should also simplify, if necessary)
                        log_exp->add(log2); //add the second Number (this should also simplify the second Number, if necessary)
                        becomeExpression(log_exp);
                    } else if (exp1) { //if second part is an Expression
                        exp1->log(base); //recursively break down expression into simplest, extended-log form
                        Expression* log_exp = new Expression(log1); //turn first Number into Expression (this should also simplify, if necessary)
                        log_exp->add(exp1); //add the second Expression
                        becomeExpression(log_exp);
                    } else
                        throw "Invalid full expression";
                } else if (exp1 && exp2) { //Both parts are expressions
                    exp1->log(base); //take log of expression recursively
                    exp2->log(base); //take log of expression recursively
                    exp1->add(exp2); //add log expressions
                    becomeExpression(exp1); //new added log is now our expression
                } else
                    throw "Invalid full expression";
            } else if (op == '/') {
                if (num1) { //at least one part is a Number
                    Number* log1 = new Log(base, num1);
                    if (num2) { //if second part is also a Number
                        Number* log2 = new Log(base, num2);
                        Expression* log_exp = new Expression(log1); //turn first Number into Expression (this should also simplify, if necessary)
                        log_exp->sub(log2); //sub the second Number (this should also simplify the second Number, if necessary)
                        //cout << "HERE" << endl;
                        becomeExpression(log_exp);
                    } else if (exp1) { //if second part is an Expression
                        exp1->log(base); //recursively break down expression into simplest, extended-log form
                        Expression* log_exp = new Expression(log1); //turn first Number into Expression (this should also simplify, if necessary)
                        log_exp->sub(exp1); //sub the second Expression
                        becomeExpression(log_exp);
                    } else
                        throw "Invalid full expression";
                } else if (exp1 && exp2) { //Both parts are expressions
                    exp1->log(base); //take log of expression recursively
                    exp2->log(base); //take log of expression recursively
                    exp1->sub(exp2); //sub log expressions
                    becomeExpression(exp1); //new added log is now our expression
                } else
                    throw "Invalid full expression";
            } else if (op == '>') {
                //implement exponentials within logs
            } else {
                Expression* copy_exp = new Expression(*this);
                exp1 = copy_exp;
                exp2 = new Expression(base);
                op = 'l';
            }
        }
    }
}

/* =============Expression* methods============== */
void Expression::add(Expression* add_exp) {

    add_exp = new Expression(*add_exp);

    if(Expression::debug) {
        cout << "Expression " << getRep() << " add(" << add_exp->getRep() << ") called!" << endl;
    }
    //Try to simplify and add that way first
    Number* addSimpNum = add_exp->simplify();
    if (addSimpNum) {
        add(addSimpNum);
    } else {


        /* code goes here*/






        /* what if addHelper for Expressions took in either some sort of array or multiplied expression,
         *  that as it recursively went down, deleted (or divided) Numbers that it had. If it ever detected that it was not possible, it returned false.*/

        /* What about 1/pi + 1/pi ?*/


        //let's extract all the variables out of addExp and create copies of them.
        //that way, we can manipulate them however we want.

        Number* add_num1 = NULL;
        Number* add_num2 = NULL;
        Expression* add_exp1 = NULL;
        Expression* add_exp2 = NULL;
        char add_op = add_exp->op;


        if (add_exp->num1)
            add_num1 = copyNumber(add_exp->num1);
        if (add_exp->num2)
            add_num2 = copyNumber(add_exp->num2);
        if (add_exp->exp1)
            add_exp1 = new Expression(*add_exp->exp1);
        if (add_exp->exp2)
            add_exp2 = new Expression(*add_exp->exp2);

        if (isHalfExp()) {
            // left operand is a half expression or just num1
            // right operand is a full expression
            // we should switch operands and retry addition
            Number* temp = copyNumber(num1); // copy left operand's number
            becomeExpression(add_exp);
            add(temp);
            delete(temp);
            temp = NULL;
        } 
        else {

            /* PERFORMING A GRACEFUL FAIL FOR NOW */
            moveInstanceToExp1();
            op = '+';
            exp2 = add_exp;

            switch (add_op) {

                    /* I just want to make SURE I account for things like (1/pi) + (1/pi), but
                     * I believe this will be simple once it's all written out.
                     */

            }
        }
    }

}

void Expression::sub(Expression* sub_exp) {

    sub_exp = new Expression(*sub_exp);


    //Try to simplify and add that way first
    Number* add_simp_num = sub_exp->simplify();
    if (add_simp_num) {
        sub(add_simp_num);
    } else {
        
        Integer* negOne = new Integer(-1);

        sub_exp->mult(negOne);
        
        add(sub_exp);

        /* code goes here*/


        /* what if addHelper for Expressions took in either some sort of array or multiplied expression,
         *  that as it recursively went down, deleted (or divided) Numbers that it had. If it ever detected that it was not possible, it returned false.*/

        /* What about 1/pi + 1/pi ?*/


        //let's extract all the variables out of addExp and create copies of them.
        //that way, we can manipulate them however we want.

        Number* add_num1 = NULL;
        Number* add_num2 = NULL;
        Expression* add_exp1 = NULL;
        Expression* add_exp2 = NULL;
        char add_op = sub_exp->op;


        if (sub_exp->num1)
            add_num1 = copyNumber(sub_exp->num1);
        if (sub_exp->num2)
            add_num2 = copyNumber(sub_exp->num2);
        if (sub_exp->exp1)
            add_exp1 = new Expression(*sub_exp->exp1);
        if (sub_exp->exp2)
            add_exp2 = new Expression(*sub_exp->exp2);



        switch (add_op) {

                /* I just want to make SURE I account for things like (1/pi) + (1/pi), but
                 * I believe this will be simple once it's all written out.
                 */

        }



        /* The way I want to represent a NUMBER subtracted from an EXPRESSION 
         *  i.e. 1 - pi*e is actually -pi*e + 1. That way, I never have to deal
         * with the case of// if(exp1 && num2), as it would cause unnecessay duplication
         * of code. Displaying this correctly can later be put into the getRep method. */
    }
}

void Expression::mult(Expression* mult_exp) {

    //copyExpression zeroth
    mult_exp = new Expression(*mult_exp);

    //Try to simplify expression and multiply that way first
    Number* multSimpNum = mult_exp->simplify();
    if (multSimpNum) {
        mult(multSimpNum); // multiply by a Number
    } else {
        // the expression could not simplify to a Single Number
        if (isHalfExp()) {
            // left operand is a half expression or just num1
            // right operand is a full expression
            // we should switch operands and retry multiplication

            Number* temp = num1;
            becomeExpression(mult_exp);
            mult(temp);
        } else {


            //let's extract all the variables out of multExp and create copies of them.
            //that way, we can manipulate them however we want.

            Number* mult_num1 = NULL;
            Number* mult_num2 = NULL;
            Expression* mult_exp1 = NULL;
            Expression* mult_exp2 = NULL;
            char mult_op = mult_exp->op;


            if (mult_exp->num1)
                mult_num1 = copyNumber(mult_exp->num1);
            if (mult_exp->num2)
                mult_num2 = copyNumber(mult_exp->num2);
            if (mult_exp->exp1)
                mult_exp1 = new Expression(*mult_exp->exp1);
            if (mult_exp->exp2)
                mult_exp2 = new Expression(*mult_exp->exp2);



            switch (mult_op) {


                case '+':
                {
                    //for any add case, we create two copies of the expression, then add them at the end.
                    Expression* temp1_exp = new Expression(*this);
                    Expression* temp2_exp = new Expression(*this);

                    if (mult_num1 && mult_num2 && !mult_exp1 && !mult_exp2) {
                        temp1_exp->mult(mult_num1);
                        temp2_exp->mult(mult_num2);
                    } else if (mult_num1 && !mult_num2 && mult_exp1 && !mult_exp2) {
                        temp1_exp->mult(mult_num1);
                        temp2_exp->mult(mult_exp1);
                    } else if (!mult_num1 && !mult_num2 && mult_exp1 && mult_exp2) {
                        temp1_exp->mult(mult_exp1);
                        temp2_exp->mult(mult_exp2);
                    } else {
                        throw getRep();
                    }

                    temp1_exp->add(temp2_exp);
                    delete(temp2_exp);
                    temp2_exp = NULL;
                    becomeExpression(temp1_exp);
                    //need to make sure addExp at least fails nicely
                    //need to make sure multiplication by zero works as intended.
                    break;
                }
                case '*':
                {
                    if (mult_num1 && mult_num2 && !mult_exp1 && !mult_exp2) {
                        mult(mult_num1);
                        mult(mult_num2); //that's it!
                    } else if (mult_exp1 && mult_num1 && !mult_num2 && !mult_exp2) {
                        mult(mult_num1);
                        mult(mult_exp1);
                    } else if (mult_exp1 && mult_exp2 && !mult_num2 && !mult_num1) { //I really don't think this case should happen, but if it does...
                        //just multiply by both!
                        mult(mult_exp1);
                        mult(mult_exp2);
                    } else {
                        throw "multiplied Expression is invalid";
                    }
                    break;

                    // Valid operators: +,-,*,/,|,>,<,r,R,l,L
                }
                case '/':
                {
                    if (num1 && num2 && !exp1 && !exp2) {

                    } else
                        if (num1 && num2 && !exp1 && !exp2) {

                    } else
                        if (num1 && num2 && !exp1 && !exp2) {

                    } else
                        throw "multiplied Expression is invalid";
                    break;
                }
                case '|':
                    break;
                case '>':
                    break;
                case '<':
                    break;
                case 'r':
                    break;
                case 'R':
                    break;
                case 'l':
                    break;
                case 'L':
                    break;

            }
        }
    }




}

void Expression::div(Expression* div_exp) {

    div_exp = new Expression(*div_exp);

    if (Expression::debug) {
        cout << "Expression.div(" << div_exp->getRep() << ") called. Current value: " << getRep() << endl;
    }
    //Try to simplify and add that way first
    Number* addSimpNum = div_exp->simplify();
    if (addSimpNum) {
        div(addSimpNum);
    } else {

        /*TO BE EXPANDED UPON GREATLY*/

        if(isHalfExp()) {
            op = '|';
        }else {
            moveInstanceToExp1();
            op = '/';
            exp2 = div_exp;
        }

    }

}

void Expression::power(Expression* power_exp) {
    power_exp = new Expression(*power_exp);

    //Try to simplify and add that way first
    Number* addSimpNum = power_exp->simplify();
    if (addSimpNum) {
        power(addSimpNum);
    } else {
        // parameter is a full expression
        if (isHalfExp()) {
            // we are a half expression and parameter is a full expression
            // if the full expression is an add we could extract the two sides
            //   and form a new expression, ex. 2^(2+pi) = 2^2 + 2^pi
            if (power_exp->op == '+') {
                Number* temp = copyNumber(num1);
                if (power_exp->num1 && power_exp->num2) {
                    power(power_exp->num1);
                    // second expression
                    Expression* tempExp = new Expression(temp);
                    tempExp->power(power_exp->num2);
                    // combine
                    add(tempExp);
                    delete tempExp;
                    tempExp = NULL;
                } else if (power_exp->exp1 && power_exp->exp2) {
                    power(power_exp->exp1);
                    // second expression
                    Expression* tempExp = new Expression(temp);
                    tempExp->power(power_exp->exp2);
                    // combine
                    add(tempExp);
                    delete tempExp;
                    tempExp = NULL;
                } else if (power_exp->exp1 && power_exp->num1) {
                    power(power_exp->exp1);
                    // second expression
                    Expression* tempExp = new Expression(temp);
                    tempExp->power(power_exp->num1);
                    // combine
                    add(tempExp);
                    delete tempExp;
                    tempExp = NULL;
                }
            }
        } else {
            // we are a full expression and parameter is a full expression
            // just give up and exit gracefully
            op = '^';
            exp2 = power_exp;
        }
    }

}




//incomplete

Number** Expression::getMultFactors(int &size) {
    Number** factors = NULL;

    //base case is when the expression contains simply one or two Numbers

    //if the expression only contains ONE NUMBER...
    if (isHalfExp()) {
        if (num1) { //this should never really be NULL, but it's good practice to check anyways
            size = 1; //there is only one factor
            factors = new Number*[1]; //create Number array
            factors[0] = copyNumber(num1); //put a copy of our element into the array

            return factors; //return the array
        } else {
            throw "Invalid Expression somehow created. Check isHalfExp value.";
        }
    } else {//if it is a full expression...

        if (num1 && num2) { //if both "slots" in the expression are Number*s,

            //then depending on the operation, we return different things.
            switch (op) {
                case '+':
                case '-':
                    //if the two numbers are added(or subtracted), then it means they could not
                    //be added any other way, therefore, there are no common factors.
                    size = 0;
                    return NULL;
                case '*':
                    //multiply is the fun one! when two Numbers are multiplied,
                    //they're both able to be returned as factors.
                    size = 2;
                    factors = new Number*[2];
                    factors[0] = copyNumber(num1);
                    factors[1] = copyNumber(num2);
                    return factors;
                case '/':
                    //since in this case the expression is acting like a fraction,
                    //just return the numerator (aka, num 1)
                    size = 1;
                    factors = new Number*[1];
                    factors[0] = copyNumber(num1);
                    return factors;
                case '^':
                    //whenever this operator is used, it means the number could
                    //not raise itself to whatever the other number is. It is an
                    //unsimplifiable expression. Therefore, no factors.
                    size = 0;
                    return NULL;
                default:
                    throw "Invalid Operator";
            }
        } else if (exp1 && num2) { //if "slot 1" is an expression, and slot two is a numebr
            //depending on what operation happens between these guys, we do different things.
            switch (op) {

            }
        } else {//(if exp1 & exp2)

        }


    }
}

void Expression::log(Expression* base) {
    // copy
    base = new Expression(*base);

    // try to simplify the parameter
    Number* baseSimpNum = base->simplify();
    if (baseSimpNum) {
        // the parameter is a Number
        log(baseSimpNum);
    } else {
        // the parameter is a full expression
        if (isHalfExp()) {
            // we are a half expression and parameter is a full expression
            op = 'l';
            exp2 = base;
        } else {
            // we are a full expression and parameter is a full expression
            Expression* copy_exp = new Expression(*this);
            exp1 = copy_exp;
            exp2 = base;
            op = 'l';
        }

    }

}

Number* Expression::simplify() {


    /* I'm thinking this method should be revamped later to re-perform operations done
       On two numbers, in that during an operation, they became combine-able. */



    Number* retNum = NULL;

    /*What if in this method we also tried to do whatever operation again? And, you know, simplify it.*/

    //if the expression only has one item in it
    //AND that one item is a number object,
    if (isHalfExp() && num1) {
        //return a COPY of num1, since we must assume the Expression class can be deleted at any time.
        retNum = num1;
    }

    Integer* num1_int = dynamic_cast<Integer*> (num1);
    Integer* num2_int = dynamic_cast<Integer*> (num2);

    /*I want it to be so, that if I run this method, and I have a exp1 * num1, && num1 == 1, that it becomes exp1*/

    //this fixes our multiply *1 case
    if ((num1_int || num2_int) && !isHalfExp() && (op == '*')) {
        if (num1_int && num1_int->getValue() == 1) {
            delete(num1);
            num1 = NULL;
            num1_int = NULL;
            //need some more logic here to get rid of the pesky exp1*num1 case
            if (num2) {
                num1 = num2;
                num2 = NULL;
                op = ' ';
                retNum = num1;
            } else if (exp1) {
                becomeExpression(exp1);
            }
        } else if (num2_int && num2_int->getValue() == 1) {
            delete(num2);
            num2 = NULL;
            num2_int = NULL;
            op = ' ';
            retNum = num1;
        }
    }

    return copyNumber(retNum);
}

/*static*/
Number* Expression::copyNumber(Number* originalNum) {
    //return a COPY of the Input, and to do that, we must check it's type first.





    //if the input is NULL,
    if (!originalNum) {
        if (Expression::debug) {
            cout << "NULL copy attempted! Returned NULL" << endl;
        }
        return NULL;
    }

    if (Expression::debug)
        cout << "Expression copyNumber(Number* " << originalNum->getRep() << ") called!" << endl;

    Number* returnNum = NULL;

    //Check Integer
    Integer* returnInt = dynamic_cast<Integer*> (originalNum);
    if (returnInt) { //if num1 is an integer
        //create a copy of num 1
        returnNum = new Integer(*returnInt);

        if (Expression::debug) {
            cout << "Integer copy attempted! Original Value: " << originalNum->getRep() << " Returned " << returnNum->getRep() << endl;
        }

        return returnNum;
    }

    //Check Constant
    Constant* returnConst = dynamic_cast<Constant*> (originalNum);
    if (returnConst) { //if num1 is a Constant
        //create a copy of num 1
        returnNum = new Constant(*returnConst);

        if (debug) {
            cout << "Constant copy attempted! Original Value: " << originalNum->getRep() << " Returned: " << returnNum->getRep() << endl;
        }

        return returnNum;
    }

    //Check Fraction
    Fraction* returnFrac = dynamic_cast<Fraction*> (originalNum);
    if (returnFrac) { //if num1 is a Constant
        //create a copy of num 1
        returnNum = new Fraction(*returnFrac);

        if (debug) {
            cout << "Fraction copy attempted! Original Value: " << originalNum->getRep() << " Returned: " << returnNum->getRep() << endl;
        }

        return returnNum;
    }

    //Check Log
    Log* returnLog = dynamic_cast<Log*> (originalNum);
    if (returnLog) { //if num1 is a Constant
        //create a copy of num 1
        returnNum = new Log(*returnLog);

        if (debug) {
            cout << "Log copy attempted! Original Value: " << originalNum->getRep() << " Returned: " << returnNum->getRep() << endl;
        }

        return returnNum;
    }


    //Check Root
    Root* returnRoot = dynamic_cast<Root*> (originalNum);
    if (returnRoot) { //if num1 is a Constant
        //create a copy of num 1
        returnNum = new Root(*returnRoot);

        if (debug) {
            cout << "Root copy attempted! Original Value: " << originalNum->getRep() << " Returned: " << returnNum->getRep() << endl;
        }

        return returnNum;
    }



    if (debug)
        cout << "COPY CONSTRUCTOR FAILED. PLEASE CHECK CODE.";
    return returnNum;

}

void Expression::becomeExpression(Expression* exp) {
    delete(num1);
    delete(num2);
    delete(exp1);
    delete(exp2);
    num1 = NULL;
    num2 = NULL;
    exp1 = NULL;
    exp2 = NULL;

    if (exp->num1)
        num1 = copyNumber(exp->num1);

    if (exp->num2)
        num2 = copyNumber(exp->num2);

    if (exp->exp1)
        exp1 = new Expression(*exp->exp1);

    if (exp->exp2)
        exp2 = new Expression(*exp->exp2);

    op = exp->op;
}

double Expression::getEstimation() {

    //simplest case
    if (isHalfExp()) {
        return num1->getEstimation();
    }

    //every other case
    double slot1 = 0;
    double slot2 = 0;

    if (exp1 && num1 && !num2 && !exp2) {
        slot1 = exp1->getEstimation();
        slot2 = num1->getEstimation();
    } else if (num1 && num2 && !exp1 && !exp2) {
        slot1 = num1->getEstimation();
        slot2 = num2->getEstimation();
    } else if (exp1 && exp2 && !num1 && !num2) {
        slot1 = exp1->getEstimation();
        slot2 = exp2->getEstimation();
    } else {
        throw "getEstimation() did not get a valid expression";
    }

    switch (op) {
        case '+':
            return slot1 + slot2;
        case '-':
            return slot1 - slot2;
        case '*':
            return slot1 * slot2;
        case '/':
            return slot1 / slot2;
        case '>':
            return pow(slot1, slot2);
        case '<':
            return pow(slot2, slot1);
        case 'R':
            return pow(slot2, 1 / slot1);
        case 'r':
            return pow(slot1, 1 / slot2);
        case 'L':
            return std::log(slot2) / std::log(slot1);
        case 'l':
            return std::log(slot1) / std::log(slot2);
        case '|':
            return slot2 / slot1;
        default:
            throw "Invalid operator.";

    }

}

/*TODO:
        -account for Num - Exp being represented as -Exp + 1?
            -Never report a "negative sign" (unless half Exp).
             instead, rearrange to show subtraction.*/

string Expression::getRep() {


    std::ostringstream strBuilder;
    // +,-,*,/,\,>,<,r,R,l,L


    Integer* testTwo = new Integer(2);

    if (num1 && !exp1 && !exp2 && !num2) {

        strBuilder << num1->getRep();

    } else if (num1 && num2 && !exp1 && !exp1) {

        switch (op) {
            case '+':
            	if(num2->getEstimation() < 0) {
            		strBuilder << num1->getRep() << num2->getRep();
            	}
            	else {
            		strBuilder << num1->getRep() << op << num2->getRep();
            	}
            	break;
            case '-':
            case '*':
                strBuilder << num1->getRep() << op << num2->getRep();
                break;

                //these cases all have parenthesis around the ENTIRE expression
            case '/':
                strBuilder << "(" << num1->getRep() << op << num2->getRep() << ")";
                break;
            case 'R':
                //special case for sqrt
                if (num1->equals(testTwo))
                    strBuilder << "(sqrt:" << num2->getRep() << ")";
                else {
                    strBuilder << "((" << num1->getRep() << ")rt:(" << num2->getRep() << "))";
                }
                break;
            case 'L':
                strBuilder << "(log_(" << num1->getRep() << "):(" << num2->getRep() << "))";
                break;
            case '>':
                strBuilder << "((" << num1->getRep() << ")^(" << num2->getRep() << "))";
                break;

            case '|'://all of these cases are like above, but reversed.
                strBuilder << "(" << num2->getRep() << "/" << num1->getRep() << ")";
                break;
            case 'r':
                //special case for sqrt
                if (num2->equals(testTwo))
                    strBuilder << "(sqrt:" << num1->getRep() << ")";
                else {
                    strBuilder << "((" << num2->getRep() << ")rt:(" << num1->getRep() << "))";
                }
                break;
            case 'l':
                strBuilder << "(log_(" << num2->getRep() << "):(" << num1->getRep() << "))";
                break;
            case '<':
                strBuilder << "((" << num2->getRep() << ")^(" << num1->getRep() << "))";
                break;
            default:
                strBuilder << "INVALID OPERATOR & NOT HALF EXPRESSION!";
        }
    } else if (exp1 && num1 && !num2 && !exp2) {


        switch (op) {
            case '+':
            	if(num1->getEstimation() < 0) {
					strBuilder << exp1->getRep() << num1->getRep();
				}
            	else {
            		strBuilder << exp1->getRep() << op << num1->getRep();
            	}
				break;
            case '-':
            case '*':
                strBuilder << exp1->getRep() << op << num1->getRep();
                break;

                //these cases all have parenthesis around the ENTIRE expression
            case '/':
                strBuilder << "((" << exp1->getRep() << ")/" << num1->getRep() << ")";
                break;
            case 'R':
                strBuilder << "((" << exp1->getRep() << ")rt:(" << num1->getRep() << "))";
                break;
            case 'L':
                strBuilder << "(log_(" << exp1->getRep() << "):(" << num1->getRep() << "))";
                break;
            case '>':
                strBuilder << "((" << exp1->getRep() << ")^(" << num1->getRep() << "))";
                break;

                //all of these cases are like above, but reversed.

            case '|':
                strBuilder << "(" << num1->getRep() << "/(" << exp1->getRep() << "))";
                break;
            case 'r':
                //special case for sqrt
                if (num1->equals(testTwo))
                    strBuilder << "sqrt:(" << exp1->getRep() << ")";
                else {
                    strBuilder << "((" << num1->getRep() << ")rt:(" << exp1->getRep() << "))";
                }
                break;
            case 'l':
                strBuilder << "(log_(" << num1->getRep() << "):(" << exp1->getRep() << "))";
                break;
            case '<':
                strBuilder << "((" << num1->getRep() << ")^(" << exp1->getRep() << "))";
                break;
            default:
                strBuilder << "INVALID OPERATOR & NOT HALF EXPRESSION!";
        }


    } else if (exp1 && exp2 && !num1 && !num2) {
        switch (op) {
            case '+':
            	if(exp2->getEstimation() < 0) {
					strBuilder << exp1->getRep() << exp2->getRep();
				}
            	else {
            		strBuilder << exp1->getRep() << op << exp2->getRep();
            	}
				break;
            case '-':
            case '*':
                strBuilder << exp1->getRep() << op << exp2->getRep();
                break;

                //these cases all have parenthesis around the ENTIRE expression
            case '/':
                strBuilder << "((" << exp1->getRep() << ")/(" << exp2->getRep() << "))";
                break;
            case 'R':
                strBuilder << "((" << exp1->getRep() << ")rt:(" << exp2->getRep() << "))";
                break;
            case 'L':
                strBuilder << "(log_(" << exp1->getRep() << "):(" << exp2->getRep() << "))";
                break;
            case '>':
                strBuilder << "((" << exp1->getRep() << ")^(" << exp2->getRep() << "))";
                break;

                //all of these cases are like above, but reversed.

            case '|':
                strBuilder << "((" << exp2->getRep() << ")/(" << exp1->getRep() << "))";
                break;
            case 'r':
                //special case for sqrt
                strBuilder << "((" << exp2->getRep() << ")rt:(" << exp1->getRep() << "))";
                break;
            case 'l':
                strBuilder << "(log_(" << exp2->getRep() << "):(" << exp1->getRep() << "))";
                break;
            case '<':
                strBuilder << "((" << exp2->getRep() << ")^(" << exp1->getRep() << "))";
                break;
            default:
                strBuilder << "INVALID OPERATOR & NOT HALF EXPRESSION!";
        }
    } else {
        strBuilder << "====INCOMLPETE OR INVALID EXPRESSION. PRINTING DATA:====\n";

        strBuilder << "isHalfExp:(";
        if (isHalfExp()) strBuilder << "true";
        else strBuilder << "false";
        strBuilder << ")\n";

        if (num1)
            strBuilder << "num1:(" << num1->getRep() << ")\n";
        if (num2)
            strBuilder << "num2:(" << num2->getRep() << ")\n";
        if (exp1)
            strBuilder << "exp1:(" << exp1->getRep() << ")\n";
        if (exp2)
            strBuilder << "exp2:(" << exp2->getRep() << ")\n";
        strBuilder << "====END INVALID/INCOMPLETE EXPRESSION OUTPUT====" << endl;
    }


    if (debug) {
        cout << "Returning (" << strBuilder.str() << ") string representation of expression!" << endl;
    }

    delete(testTwo);
    testTwo = NULL;

    return strBuilder.str();
}

void Expression::moveInstanceToExp1() {

    //create a copy of the instance
    Expression* expCopy = new Expression(*this);

    //2. Empty out the pointer variables in the current class to prevent memory leaks and other errors
    delete(exp1);
    delete(exp2);
    delete(num1);
    delete(num2);

    //NULL them so they don't point to random memory...
    exp1 = NULL;
    exp2 = NULL;
    num1 = NULL;
    num2 = NULL;

    //3. set exp1 = to our copy of our instance
    exp1 = expCopy;

    //something to note about this method is that it does NOT
    //touch the operator or the isHalfExp().
}



