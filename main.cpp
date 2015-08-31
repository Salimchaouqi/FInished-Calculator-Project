/*
 * main.cpp
 *
 *  Created on: Apr 14, 2014
 *      Author: Igor
 */
#include "UI.h"
#include "Root.h"
#include "Log.h"
#include "Expression.h"

int main() {
//	UI* ui = new UI();
//	ui->printMenu();
//    
    Number* one = new Integer(1);
    Number* two = new Integer(2);
    Number* three = new Integer(3);
    Number* four = new Integer(4);
    Number* five = new Integer(5);
    Number* six = new Integer(6);
    Number* ten = new Integer(10);
    Number* sixteen = new Integer(16);
    Number* twenty_seven = new Integer(27);
    Number* one_hundred_eight = new Integer(108);    
    
    Number* pi = new Constant('p');
    Number* e = new Constant('e');
//    
//    
//    Expression* exp = new Expression(sqrt3);
//    
//    cout << exp->getRep();
//    
    Expression* pi_exp = new Expression(pi);
    Expression* two_exp = new Expression(two);
    Expression* three_exp = new Expression(three);
    Expression* three_exp2 = new Expression(three);
    
    Expression* b = new Expression(one_hundred_eight);
    b->root(three);
    
    Expression* a = new Expression(sixteen);
    a->root(three);
    
    b->mult(a);
    
    cout << b->getRep();
//    Expression* four_exp = new Expression(four);
//    Expression* six_exp = new Expression(six);
//    


    //pi*2
//    two_exp->add(pi_exp);
//    
//    //
//    two_exp->mult(three_exp);
//    
//    //three_exp->add(three_exp2);
//    
//    cout << two_exp->getRep();
    
//    Expression* four_exp = new Expression(four);
//    Expression* six_exp = new Expression(six);
//    Expression* ten_exp = new Expression(ten);

      
//    Expression* pi_exp = new Expression(pi);
//    Expression* e_exp = new Expression(e);
//    
//    two_exp->div(four_exp);
//    
//    two_exp->mult(pi_exp);
//    two_exp->mult(e_exp);
//    two_exp->mult(pi_exp);
//    
//    two_exp->add(six_exp);
//    
//    cout << "(1/2) = " << two_exp->getRep() << endl;
//    
//    try {
//        
//        two_exp->div(sqrt3);
//        cout << two_exp->getRep();
//    
//    }
//    catch(const char* e) {
//        cout << e << endl;
//    }
    
   // cout << two_exp->getRep() << endl;

    return 0;
 }



