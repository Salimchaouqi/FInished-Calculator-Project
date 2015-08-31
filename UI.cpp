//
//  UserInput.cpp
//  Menu
//
//  Created by Christine Moore on 4/10/14.
//  Copyright (c) 2014 Christine Moore. All rights reserved.
//

#include "UI.h"
using namespace std;

//constructors and deconstructors
UI::UI(){

};

/*UI::UI(Expression* expression){
    this->ans = expression;
};*/

UI::~UI(){
//deconstructor
};


//basic methods
vector<string> UI::getPastans(){
    return pastans;
};

void UI::setPastans(vector<string> pastans){
    this->pastans = pastans;
};

//input methods
string UI::checkInput(string input){
	// change sqrt: to " 2@" operator
	size_t pos = input.find("sqrt:");
	while(pos != string::npos) {
		input.replace(pos, 5, " 2@");
		pos += 3; // skip over the replaced string
		// skip over opening parentheses
		while(pos < input.length() && input.at(pos) == '(') {
			pos++;
		}
		// index is out of range
		if(pos >= input.length()) {
			throw "Check input could not find a value for sqrt:";
		}
		else if(input.at(pos) == '-') {
			input.replace(pos, 1, "(-");
			pos += 2; // skip (-
			while(pos < input.length() && (isdigit(input.at(pos)) || input.at(pos) == 'e' ||
					input.at(pos) == 'p')) {
				// while within the string and the next char is a digit
				if((pos+1) < input.length()) {
					if(input.at(pos+1) == 'i') {
						pos++;
					}
				}
				pos++; // go to next char
			}
			input.replace(pos, 0, ")");
		}
		// the next char is not a number and not e and not pi
		else if(!isdigit(input.at(pos)) && input.at(pos) != 'e' &&
				!(input.at(pos) == 'p' && input.at(pos+1) == 'i')) {
			throw "Check input could not find a value for sqrt:";
		}

		// search for the next sqrt:
		pos = input.find("sqrt:", pos+1);
	}
	// change rt: to "@" operator
	pos = input.find("rt:");
	while(pos != string::npos) {
		input.replace(pos, 3, "@");
		pos++; // skip over @
		// skip over opening parentheses
		while(pos < input.length() && input.at(pos) == '(') {
			pos++;
		}
		// index is out of range
		if(pos >= input.length()) {
			throw "Check input could not find a value for rt:";
		}
		else if(input.at(pos) == '-') {
			input.replace(pos, 1, "(-");
			pos += 2; // skip (-
			while(pos < input.length() && (isdigit(input.at(pos)) || input.at(pos) == 'e' ||
					input.at(pos) == 'p')) {
				// while within the string and the next char is a digit
				if((pos+1) < input.length()) {
					if(input.at(pos+1) == 'i') {
						pos++;
					}
				}
				pos++; // go to next char
			}
			input.replace(pos, 0, ")");
		}
		// the next char is not a number and not e and not pi
		else if(!isdigit(input.at(pos)) && input.at(pos) != 'e' &&
				!(input.at(pos) == 'p' && input.at(pos+1) == 'i')) {
			throw "Check input could not find a value for rt:";
		}

		// find the next root
		pos = input.find("rt:");
	}
	// change @- to "@(-x)"
	pos = input.find("@");
	while(pos != string::npos) {
		if(input.at(++pos) == '-') {
			input.replace(pos, 1, "(-");
			pos += 2; // skip (-
			while(pos < input.length() && (isdigit(input.at(pos)) || input.at(pos) == 'e' ||
					(input.at(pos) == 'p' && input.at(pos+1) == 'i'))) {
				// while within the string and the next char is a digit
				if(input.at(pos+1) == 'i') {
					pos++;
				}
				pos++; // go to next char
			}
			input.replace(pos, 0, ")");
		}
		pos = input.find("@", pos);
	}
	// change *- to "*(-x)"
	pos = input.find("*");
	while(pos != string::npos) {
		if(input.at(++pos) == '-') {
			input.replace(pos, 1, "(-");
			pos += 2; // skip (-
			while(pos < input.length() && (isdigit(input.at(pos)) || input.at(pos) == 'e' ||
					(input.at(pos) == 'p' && input.at(pos+1) == 'i'))) {
				// while within the string and the next char is a digit
				if(input.at(pos+1) == 'i') {
					pos++;
				}
				pos++; // go to next char
			}
			input.replace(pos, 0, ")");
		}
		pos = input.find("*", pos);
	}
	// remove log_ and replace the colon with "$" operator
	pos = input.find("log_");
	while(pos != string::npos) {
		input.replace(pos, 4, " "); // replace log_ with a space
		pos++; // go to next char
		unsigned int startOfFirstNumber = pos; // helps check if the first number is missing
		while(pos < input.length() &&
				(isdigit(input.at(pos))	|| input.at(pos) == 'e' || (input.at(pos) == 'p'))) {
			// while within the string and the next char is a digit or a variable
			if (input.at(pos) == 'p' && input.at(pos + 1) == 'i') {
				pos++;
			}
			pos++; // go to next char
		}
		// if a parenthesis is found, skip until it is closed
		if(input.at(pos) == '(') {
			int numOfOpenedParenthesis = 1; // keep track if more parenthesis are opened
			// while we have parentheses open and we did not pass ":" go to next char
			while(numOfOpenedParenthesis > 0 && pos < input.length() && input.at(pos) != ':') {
				pos++; // go to next char
				if(input.at(pos) == '(') {
					numOfOpenedParenthesis++;
				}
				else if (input.at(pos) == ')') {
					numOfOpenedParenthesis--;
				}
			}
			// either we reached the end of the input or a ":" was found before the parentheses were closed
			if(pos == input.length() || input.at(pos) == ':') {
				throw "Check input could not find a closing parenthesis in log";
			}
			pos++; // skip the closing parenthesis
		}
		// check if the first number is missing or if the starting position is also the ending position
		if(startOfFirstNumber == pos) {
			throw "Check input detected a missing base in log";
		}
		if(input.at(pos) == ':') {
			input.replace(pos, 1, "$");
		}
		else {
			throw "Check input could not find a colon after log";
		}
		// check if there is a number after the colon
		// first skip opening parentheses
		pos++; // skip the "$", previously ":"
		while(pos < input.length() && input.at(pos) == '(') {
			pos++;
		}
		// index is out of range
		if(pos >= input.length()) {
			throw "Check input could not find a value for a log";
		}
		else if(input.at(pos) == '-') {
			input.replace(pos, 1, "(-");
			pos += 2; // skip (-
			while(pos < input.length() && (isdigit(input.at(pos)) || input.at(pos) == 'e' ||
					(input.at(pos) == 'p' && input.at(pos+1) == 'i'))) {
				// while within the string and the next char is a digit
				if(input.at(pos+1) == 'i') {
					pos++;
				}
				pos++; // go to next char
			}
			input.replace(pos, 0, ")");
		}
		// the next char is not a number and not e and not pi
		else if(!isdigit(input.at(pos)) && input.at(pos) != 'e' &&
				!(input.at(pos) == 'p' && input.at(pos+1) == 'i')) {
			throw "Check input could not find a value for a log";
		}

		pos = input.find("log_"); // look for the next occurrence of log_
	}
	// change ans to last result
	pos = input.find("ans");
	while(pos != string::npos) {
		// if ans was found and the answer vector is empty throw exception
		if((pastans.size() == 0)) {
			throw "Can not use ans keyword since there are no previous answers.";
		}
		string lastResult = "(";
		lastResult += pastans[0];
		lastResult += ")";
		input.replace(pos, 3, lastResult);
		pos = input.find("ans");
	}
	// change decimal numbers to non decimal divided by appropriate power of 10
	// for example 2.0 becomes 20/10 and 0.02 becomes 002/100
	// unfortunately leading zeroes remain, but Integer class will ignore those
	pos = input.find(".");
	while(pos != string::npos) {
		input.replace(pos, 1, "");
		int i = 0;
		while(pos < input.length() && (isdigit(input.at(pos)))) {
			// while within the string and the next char is a digit
			pos++; // go to next char
			i++;
		}
		string divisor = "/1";
		for(int j = 0; j < i; j++) {
			divisor += "0";
		}
		input.replace(pos, 0, divisor);
		pos = input.find(".");
	}

	// check if all of the parenthesis are matching
	int numOfOpenedParentheses = 0; // keep track of opened parentheses
	int numOfClosedParentheses = 0; // keep track of closed parentheses
	for(unsigned int i = 0; i < input.length(); i++) {
		if(input.at(i) == '(') {
			numOfOpenedParentheses++;
		}
		else if (input.at(i) == ')') {
			numOfClosedParentheses++;
		}
	}
	if(numOfOpenedParentheses != numOfClosedParentheses) {
		throw "Check input detected unmatched parentheses";
	}

	return input;
};

//string UI::convertToRPN(string input){

//    return input;
//};

Expression* UI::calculate(string input){
	input = checkInput(input);
	const char* expr = (char*)input.c_str();
	Expression* result = calculator::calculate(expr);
    return result;
};

//output methods
void UI::printMenu(){
    string selection;
    
    // this runs outside of the loop so that it only shows up in the beginning
    cout<<"Welcome to the calculator!" <<endl;
    do {
        cout<<"Please type in the number of your selection"<<endl;
        cout<<"1 - Select to use the calculator by entering own input and being returned a decimal value"<<endl;
        cout<<"2 - Select to use the calculator by entering own input and being returned an exact value"<<endl;
        cout<<"3 - Select to use calculator with a file input"<<endl;
        cout<<"4 - Select for help and instructions for using the calculator"<<endl;
        cout<<"5 - View previous answers"<<endl;
        cout<<"6 - Select to exit out of the calculator"<<endl;
        //start of try catch block
        
        try{
        	getline(cin, selection);
            if(selection != "1" && selection != "2" && selection != "3" && selection != "4" && selection != "5" && selection != "6"){
                //cout<<"Please enter a valid menu selection."<<endl;
                throw invalid_argument("wrong");
            }
        } catch(invalid_argument &e) {
                cout<< "You did not enter a valid menu selection, please enter an integer 1-5.\n" <<endl;
        }
        
        if (selection == "1"){
            cout<<"Please enter in desired calculation (for help on how to correctly enter in expressions see option 2 on the menu):"<<endl;
            //where we would put code to access the calculator
            string enteredexpression;
            getline (cin, enteredexpression);
            //convertToRPN(enteredexpression);
            try {
            	Expression* result = calculate(enteredexpression);
				if(result) {
					// check if the vector is full, if so remove last element
					if(pastans.size() >= 10) {
						pastans.pop_back();
					}
					// insert result into the first place of the vector
					pastans.insert(pastans.begin(), result->getRep());
					//cout << enteredexpression << " = " << result->getRep() << endl;
					cout<< "Your answer expressed as a decimal is:"<<endl;
					cout << "Decimal value: " << result->getEstimation() << endl;
					cout<<endl<<endl;
				}
            } catch(const char* c) {
				cout << c << endl;
				continue;
			}
        }
        else if( selection == "2") {
            cout<<"Please enter in desired calculation (for help on how to correctly enter in expressions see option 4 on the menu):"<<endl;
            string enteredexpression;
            getline (cin, enteredexpression);
            //cout << enteredexpression << endl;
            try {
            	Expression* result = calculate(enteredexpression);
				if(result) {
					if(pastans.size() >= 10){
						pastans.pop_back();
					}
					pastans.insert(pastans.begin(), result->getRep());
					cout<< "Your answer is as represented below:"<<endl;
					cout << enteredexpression << " = " << result->getRep() <<endl;
					cout << endl<<endl;
				}
            } catch(const char* c) {
				cout << c << endl;
				continue;
			}
        
        }
        
        else if(selection =="3"){
            //the following is for using the expression from an imported text file, if it gets messy we can take it out since its extra credit, I think this is the basic setup but I can not get it to distinguish between when there is and isn't a txt file to import.
            
        	try {
            cout<< "Please enter the name of the file including extension name containing the expression you wish to use." <<endl;
            string filename;
            getline(cin, filename);
            ifstream text_expression;
            //text_expression.open(filename);
            if(!text_expression) { // file couldn't be opened
                text_expression.close();
                throw "No external file with an expression has been detected by the calculator.";
            }
            string input;
            if (text_expression){
            while (!text_expression.eof()){ //reads till end of file
                text_expression >> input;
                //cout << input <<endl;
            //cout<< "The expression in your text file you are going to be using is: "<<input<<endl;
             //text_expression >> input;
             }
            }
            text_expression >> input;
            text_expression.close();
            //cout<< "The calculator has read in your expression. To calculate your expression in the text file select option 1 on the menu and type in the phrase \"text_expression\"." <<endl;
            //end of code to read in a text file
            if(input == "") {
            	throw "The file could not be found or it is empty.\n";
            }
            calculate(input);
            cout<<endl<<endl;
        	} catch(const char* c) {
        		cout << c << endl;
        	}
        
        
        }
        else if (selection =="4"){
            cout<<"How to use the calculator\n_________________________"<<endl<<endl;
            cout<<"***Using the menu***\nWhen viewing the menu make sure you are entering a number 1 -5 to make your selection. You are able to use the previous answer (expression or number) that you calculated by entering the value as \"ans\". You may also choose to use expressions from an imported file. Just please make sure the expression or expressions you want to calculate are each written on their own lines."<<endl<<endl;
            cout<<"***Entering in what you want to calculate***\nWhen you are entering in your mathematical expression make sure that you have entered the correct amount of parentheses and that you have not left any parentheses unclosed. Also, make sure that you have entered correct mathematical operands: +, -. *, /. !Note! When you are multiplying two values, enter them as value1 * value2. For example, 2 times pi must be entered 2*pi, not 2pi.\n\nFractions:\nFor entering fractions, make sure you enter your fraction within parentheses like so (numerator/denominator). For example, one half would be entered as follows: (1/2). Improper fractions are allowed to be entered, as well as fractions containing constants.\n\nEntering constants like pi and e:\nTo use pi and e in your calculation, use operators like normal and represent these two constant values as \"pi\" and \"e\". For example, adding a value of two to pi  would be entered as \"pi + 2\".\n\nRaising a value to another value (exponentiating):\nWhen you want to raise a number/expression to a power please place said number/expression in parentheses, followed by a carat (^), then enter what you would like to raise your number/expression to. For example if you wanted to raise 5 to the power of 2 you would enter it as \"(5)^2\". !Note! You can not raise a number/expression to a constant like e or pi./n/nLogarithms:\nWhen entering a logarithmic expression you must enter\"log_\" and then the base of your logarithm followed by a colon and your logartihm's value. For example, to use the expression log of 4 base three, enter \"log_3:4\". For this calculator, you may enter constant values such as log of 2 base e or log of pi base 4. !Note! Logs should be entered in their most separable form. For example, log of 6 base 7 must be written (log of 3 base 7) + (log of 2 base 7).\n\nTaking the nth root of a number:\nTo take the square root of a number please enter it in the format \"sqrt\", then a colon, then your number you wish to take the square root of. For example, if you wanted to take the square root of 9, please enter \"sqrt:9\". If you wish to take the nth root of a number, please enter \"nrt\", but replace n with your desired number. Then, like with the sqrt, enter a colon, followed by the number you wish to take the nth root of. For example, if you wish to take the 3rd root of 8, you would enter \"3rt:8\"."<<endl<<endl;
                
        }
        
        else if(selection == "5"){
            cout<<"Your previous answers to your calculations are as follows from most recent to oldest:"<<endl;
            //pastans.push_back(ans);
            /*if(pastans.size() > 10){
                delete pastans[0];
                pastans.erase(pastans.begin());
            }*/
            for (unsigned int i = pastans.size(); i > 0; i--){
                cout<< pastans[i-1] <<endl;
            }
            cout << endl;
        }
    } while (selection != "6");
 
};
