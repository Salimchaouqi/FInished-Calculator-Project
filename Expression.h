/* 
 * Author: max
 * 
 * Version 2.0
 */

#ifndef EXPRESSION_H
#define	EXPRESSION_H

#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

class Number;

class Expression {
public:
    Expression();
    Expression(const Expression& orig);
    virtual ~Expression();
    
    Expression(Number* num);
    
    //Number* Methods
    virtual void add(Number* addNum);
    virtual void sub(Number* subNum);
    virtual void mult(Number* multNum);
    virtual void div(Number* divNum);
    virtual void power(Number* powerNum);
    virtual void root(Number* rootNum);
    virtual void log(Number* base);
    
    
    //Expression Methods
    void add(Expression* addExp);
    void sub(Expression* subExp);
    void mult(Expression* multExp);
    void div(Expression* divExp);
    void power(Expression* powerExp);
    void root(Expression* rootExp);
    void log(Expression* base);
    
    Number* simplify(); //if possible, return the expression as a single Number
    
    //returns common factors for any given expression in an array of pointers.
    //assume all numbers are copies, and, therefore, must be deleted properly
    Number** getMultFactors(int &size);
    
    //returns a pointer to a copy of the inputted number
    static Number* copyNumber(Number* originalNum);
    
    double getEstimation();
    string getRep();
    
    static bool debug; //if true, will output more about what it is doing.
    
private:
    Number* num1;
    Number* num2;
    
   // void simplifyNumber(Number* number);
    
    //if exp1 is a valid Expression pointer, it is ASSUMED to be on the left(first), followed by either num1 or exp2.
    Expression* exp1;
    
    
    //only used when ANOTHER EXPRESSION is divided by an unsimplifable expression,
    //or has two two added elements that contain groups of multiplied things : 2*pi + 2*e
    //raised to a multi Number, irrational power. i.e. (3+pi)/(2+e) || (2+pi)^(2+e);\
    //log and root?
    Expression* exp2;
    
    
    char op; // Valid operators: +,-,*,/,|,>,<,r,R,l,L
             /* for division / means what you think while \ just means to divide the opposite way.
              * The distinguishment is for the case exp1 && num1, which it is assumed exp1 is always on the right,
              * just for consistency purposes.
              * 
              for power, < == (right to the power of left),
               while > ==(left to the power of right). Read it like an arrow. Remember, if
                exp1 is initialized, it is assumed to be on the left. 
              
              As for roots, the op 'r' means left to right("left root right"), while 'R' means right to left ("right root left)
              * "left root right" translates to left being the "value" and right to being what it's "rooted to". 
              * e.g 3 r 2 = sqrt:3)
              *     2 R 3 = 3rt:2
              
              For logs, L means read left to right, while l means right to left.
              * Ex. L => log base (slot1) of (slot2). l => log base(slot2) of slot(1) */
    
    
    //bool isHalfExp; //represents whether both "slots" have been used yet, or simply one.
    bool isHalfExp();
    
    //these methods will be used internally by the Expression class,
    //and other Expression classes in order to help with the simplification process.
    
    //returns true if they simplify, false if they do not.
    bool addHelper(bool firstRun, Number* addNum);
    
    /*Add helper for Expression returns true if the expression was able to incorporate it into one of its current numbers*/
    bool addHelper(Expression* addExp);
    
    void multHelper(bool has_multed_exp, Number* mult_num); /* */
    bool divHelper(Number* div_num);
    bool powerHelper(Number* addNum);
    

    void becomeExpression(Expression* exp); //changes the current expression into
                                            //the expression given
    
    void moveInstanceToExp1(); //creates a copy of the current instance, deletes and NULL's exp1, exp2, num1, and num2, and then puts the copy into exp1;
    
    //ector<Number*> getNotCoefficient();
    

};

#endif	/* EXPRESSION_H */

/*
 * <<Standard Constructors >>
+Expression()
+Expression(orig: const Expression&)
+~Expression()
+Expression(Number* num)

<< Operation Constructors >>
+Expression(num1: Number*, op: char, num2: Number*)
+Expression(exp1: Expression*, op: char, exp2: Expression*)
+Expression(exp: Expression*, op: char, num: Number*)
+Expression(num: Number*, op: char, exp: Expression*)

<< Number* Methods >>
+.add(num: Number*):bool
+.sub(num: Number*):bool
+.mult(num: Number*):bool
+.div(num: Number*):bool
+.power(num: Number*):bool

<< Expression* Methods >>
+.add(exp: Expression*):bool
+.sub(exp: Expression*):bool
+.mult(exp: Expression*):bool
+.div(exp: Expression*):bool
+.power(exp: Expression*):bool

<< Output Methods >>
+getRep(): string
+getEstimation(): double
 */
