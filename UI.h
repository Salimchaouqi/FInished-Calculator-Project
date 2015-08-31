//
//  UserInput.h
//  Menu
//
//  Created by Christine Moore on 4/10/14.
//  Copyright (c) 2014 Christine Moore. All rights reserved.
//
#ifndef __Menu__UI__
#define __Menu__UI__
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
//#include <exception>
#include <stdexcept>

#include "Expression.h"
#include "Number.h"
#include "Integer.h"
#include "Constant.h"
#include "shunting-yard.h"

class UI {
public:
    //constructors and deconstructors
    UI();
    //UI(Expression* expression);
    ~UI();
    //basic methods
    vector<string> getPastans();
    void setPastans(vector<string> pastans);
    
    //output methods
    void printMenu();
        //input methods
    string checkInput(string input);
    //string convertToRPN(string input);
    Expression* calculate(string input);
private:
    vector<string> pastans;
};

#endif /* defined(__Menu__UserInput__) */
