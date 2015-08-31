// Source: http://www.daniweb.com/software-development/cpp/code/427500/calculator-using-shunting-yard-algorithm#
// Author: Jesse Brown
// Modifications: Brandon Amos

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <math.h>
#include "Integer.h"
#include "Number.h"
#include "Constant.h"
#include "Root.h"
#include "Log.h"
#include "Fraction.h"

#include "shunting-yard.h"

#define isvariablechar(c) (isalpha(c) || c == '_')

TokenQueue_t calculator::toRPN(const char* expr, std::map<std::string, int> opPrecedence) {
//TokenQueue_t calculator::toRPN(const char* expr,
//    std::map<std::string, double>* vars,
//    std::map<std::string, int> opPrecedence) {

  TokenQueue_t rpnQueue; std::stack<std::string> operatorStack;
  bool lastTokenWasOp = true;
  bool negativeUnaryOp = false;

  // In one pass, ignore whitespace and parse the expression into RPN
  // using Dijkstra's Shunting-yard algorithm.
  while (*expr && isspace(*expr )) ++expr;
  while (*expr ) {
    if (isdigit(*expr )) {
      // If the token is a number, add it to the output queue.
      char* nextChar = 0;
      double digit = strtod(expr , &nextChar);

      /*
       * Integer* conversion
       */
      int negativeMultiplier = 1;
      // check if there was a negative unary operator
      if(negativeUnaryOp) {
    	  negativeMultiplier = -1;
    	  negativeUnaryOp = false; // reset flag

    	  // find the last "-" in the stack, skipping "("
    	  int parenthesisCount = 0;
    	  while(operatorStack.size() > 0) {
    		  std::string currentOperator = operatorStack.top();
			  if(currentOperator == "(") {
				  parenthesisCount++;
				  operatorStack.pop(); // temporarily remove "(" from stack
			  }
			  else if (currentOperator == "-") {
				  operatorStack.pop(); // remove "-" from the stack
				  // reinsert the parentheses
				  for(int i = 0; i < parenthesisCount; i++) {
					  operatorStack.push("(");
				  }
				  break;
			  }
			  else {
				  throw "Shunting-yard encountered an error while finding negative unary operator in the stack";
			  }
    	  }

    	  //cout << "rpnQueue size: " << rpnQueue.size() << endl;
    	  //cout << "operatorStack size: " << operatorStack.size() << endl;
      }
      // create a positive/negative Integer
      Integer* value = new Integer(negativeMultiplier * digit);
      //cout << "Integer value: " << value->getValue() << endl;

#     ifdef DEBUG
        std::cout << digit << std::endl;
#     endif
      //rpnQueue.push(new Token<double>(digit));
      // put Number into rpnQueue
      rpnQueue.push(new Token<Integer*>(value));
      expr = nextChar;
      lastTokenWasOp = false;
    }
      else if (isvariablechar(*expr )) {
    	  // Read the rest of the string
    	  std::stringstream ss;
    	  ss << *expr;
		  ++expr;
		  while (isvariablechar(*expr )) {
			  ss << *expr;
			  ++expr;
		  }
		  std::string key = ss.str();
		  // Create Number* and construct it using appropriate constant
		  Constant* val;
		  if(key == "pi") {
    		  val = new Constant('p');
    	  }
    	  else if(key == "e") {
    		  val = new Constant('e');
    	  }
    	  else {
    		  //throw std::domain_error("Invalid variable '" + key + "'.");
    		  string error = "Invalid variable '" + key + "'.";
    		  throw (char*)error.c_str();
    	  }

    	  rpnQueue.push(new Token<Constant*>(val));;
    	  lastTokenWasOp = false;
      }

    /* Commented out parsing of variables
      else if (isvariablechar(*expr )) {
      // If the function is a variable, resolve it and
      // add the parsed number to the output queue.
      if (!vars) {
        throw std::domain_error(
            "Detected variable, but the variable map is null.");
      }

      std::stringstream ss;
      ss << *expr;
      ++expr;
      while (isvariablechar(*expr )) {
        ss << *expr;
        ++expr;
      }
      std::string key = ss.str();
      std::map<std::string, double>::iterator it = vars->find(key);
      if (it == vars->end()) {
        throw std::domain_error(
            "Unable to find the variable '" + key + "'.");
      }
      double val = vars->find(key)->second;
#     ifdef DEBUG
        std::cout << val << std::endl;
#     endif
      rpnQueue.push(new Token<double>(val));;
      lastTokenWasOp = false;
    } */
      else {
      // Otherwise, the variable is an operator or paranthesis.
      switch (*expr) {
        case '(':
          operatorStack.push("(");
          ++expr;
          break;
        case ')':
          while (operatorStack.top().compare("(")) {
            rpnQueue.push(new Token<std::string>(operatorStack.top()));
            operatorStack.pop();
          }
          operatorStack.pop();
          ++expr;
          break;
        default:
          {
            // The token is an operator.
            //
            // Let p(o) denote the precedence of an operator o.
            //
            // If the token is an operator, o1, then
            //   While there is an operator token, o2, at the top
            //       and p(o1) <= p(o2), then
            //     pop o2 off the stack onto the output queue.
            //   Push o1 on the stack.
            std::stringstream ss;
            ss << *expr;
            ++expr;
            while (*expr && !isspace(*expr ) && !isdigit(*expr )
                && !isvariablechar(*expr) && *expr != '(' && *expr != ')') {
              ss << *expr;
              ++expr;
            }
            ss.clear();
            std::string str;
            ss >> str;
#           ifdef DEBUG
              std::cout << str << std::endl;
#           endif

            if (lastTokenWasOp) {
              //cout << "Unary op call for: " << str << endl;
              // Convert unary operators to binary in the RPN.
              //if (!str.compare("-") || !str.compare("+")) {
                //rpnQueue.push(new Token<double>(0));
              if (!str.compare("-")) {
            	  negativeUnaryOp = true;
              } else if (!str.compare("+")) {
            	  //unary + operator can be ignored
              }
              else {
                //throw std::domain_error(
                //    "Unrecognized unary operator: '" + str + "'.");
            	  string error = "Unrecognized unary operator: '" + str + "'.";
            	  throw (char*)error.c_str();
              }
            }

            while (!operatorStack.empty() &&
                opPrecedence[str] <= opPrecedence[operatorStack.top()]) {
              rpnQueue.push(new Token<std::string>(operatorStack.top()));
              operatorStack.pop();
            }
            operatorStack.push(str);
            lastTokenWasOp = true;
          }
      }
    }
    while (*expr && isspace(*expr )) ++expr;
  }
  while (!operatorStack.empty()) {
    rpnQueue.push(new Token<std::string>(operatorStack.top()));
    operatorStack.pop();
  }

  /*
  while(rpnQueue.size()) {
	  cout << "rpnQueue size: " << rpnQueue.size() << endl;
	  TokenBase* base = rpnQueue.front();
	  rpnQueue.pop();

	  Token<std::string>* strTok = dynamic_cast<Token<std::string>*>(base);
	  Token<Integer*>* IntegerTok = dynamic_cast<Token<Integer*>*>(base);
	  Token<Constant*>* ConstantTok = dynamic_cast<Token<Constant*>*>(base);
	  Token<Root*>* RootTok = dynamic_cast<Token<Root*>*>(base);
	  Token<Log*>* LogTok = dynamic_cast<Token<Log*>*>(base);

	  if(strTok) {
		  cout << "string: " << strTok->val << endl;
	  }
	  else if (IntegerTok) {
		  cout << "integer: " << IntegerTok->val->getRep() << endl;
	  }
	  else if (ConstantTok) {
		  cout << "constant: " << ConstantTok->val->getRep() << endl;
	  }
	  else if (RootTok) {
		  cout << "root: " << RootTok->val->getRep() << endl;
	  }
	  else if (LogTok) {
		  cout << "log: " << LogTok->val->getRep() << endl;
	  }
  }
  */

  return rpnQueue;
}

Expression* calculator::calculate(const char* expr) {
//double calculator::calculate(const char* expr){,
   // std::map<std::string, double>* vars) {
  // 1. Create the operator precedence map.
  std::map<std::string, int> opPrecedence;
  opPrecedence["("] = -1;
  opPrecedence["+"]  = 1; opPrecedence["-"]  = 1;
  opPrecedence["+-"]  = 1; opPrecedence["-+"]  = 1;
  opPrecedence["--"]  = 1; opPrecedence["++"]  = 1;
  opPrecedence["*"]  = 2; opPrecedence["/"]  = 2;
  opPrecedence["^"]  = 3;
  opPrecedence["@"] = 4; // rt
  opPrecedence["$"] = 4; // log

  // 2. Convert to RPN with Dijkstra's Shunting-yard algorithm.
  //TokenQueue_t rpn = toRPN(expr, vars, opPrecedence);
  TokenQueue_t rpn = toRPN(expr, opPrecedence);

  // 3. Evaluate the expression in RPN form.
  //std::stack<double> evaluation;
  //std::stack<Number*> evaluation; // stack for Numbers
  std::stack<Expression*> evaluation; // stack for Expressions

  Expression* expression = NULL; // expression pointer

  while (!rpn.empty()) {
    TokenBase* base = rpn.front();
    rpn.pop();

    // try casting string aka operator
    Token<std::string>* strTok = dynamic_cast<Token<std::string>*>(base);
      //Token<double>* doubleTok = dynamic_cast<Token<double>*>(base);
    // try casting Number Types
    Token<Integer*>* IntegerTok = dynamic_cast<Token<Integer*>*>(base);
    Token<Constant*>* ConstantTok = dynamic_cast<Token<Constant*>*>(base);
    Token<Root*>* RootTok = dynamic_cast<Token<Root*>*>(base);
    Token<Log*>* LogTok = dynamic_cast<Token<Log*>*>(base);
    //cout << "String: " << strTok << " Integer: " << IntegerTok << " Constant: " << ConstantTok << endl;

    if (strTok) {
      std::string str = strTok->val;
      if (evaluation.size() < 2) {
    	  //cout << "eval size: " << evaluation.size() << endl;
        //throw std::domain_error("Invalid equation.");
    	  throw "Invalid equation.";
      }
      //Number* right = evaluation.top(); evaluation.pop();
      //Number* left  = evaluation.top(); evaluation.pop();
      Expression* right = evaluation.top(); evaluation.pop();
      Expression* left  = evaluation.top(); evaluation.pop();

      	if(!str.compare("--")) {
		  str = "+";
		}
		else if (!str.compare("+-")) {
		  str = "-";
		}
		else if (!str.compare("-+")) {
		  str = "-";
		}
		else if (!str.compare("++")) {
		  str = "+";
		}


      if (!str.compare("+")) {
    	  if(!expression) { // if expression doesn't exist yet
    		  left->add(right);
    		  evaluation.push(left);
    	  }
    	  else {
    		  //cout << "Add: " << left->getRep() << " + " << right->getRep() << endl;
    		  left->add(right);
    		  //cout << "Result of Add: " << left->getRep() << endl;
    		  evaluation.push(left);
    	  }
      } else if (!str.compare("*")) {
    	  if(!expression) { // if expression doesn't exist yet
    		  //cout << "Mult: " << left->getRep() << " * " << right->getRep() << endl;
			  left->mult(right);
			  evaluation.push(left);
		  }
		  else {
			  left->mult(right);
			  evaluation.push(left);
		  }
      } else if (!str.compare("-")) {
    	  if(!expression) { // if expression doesn't exist yet
			  left->sub(right);
			  evaluation.push(left);
		  }
		  else {
			  left->sub(right);
			  evaluation.push(left);
		  }
      } else if (!str.compare("/")) {
    	  if(!expression) { // if expression doesn't exist yet
    		  left->div(right);
			  evaluation.push(left);
		  }
		  else {
			  left->div(right);
			  evaluation.push(left);
		  }
      } else if (!str.compare("^")) {
    	  if(!expression) { // if expression doesn't exist yet
			  left->power(right);
			  evaluation.push(left);
		  }
		  else {
			  left->power(right);
			  evaluation.push(left);
		  }
      } else if (!str.compare("@")) { // root
        //evaluation.push(pow(right, 1 / left));
    	  if(!expression) { // if expression doesn't exist yet
			  right->root(left); // right is expression, left is root base
			  evaluation.push(right);
		  }
		  else {
			  right->root(left); // left is root base
			  evaluation.push(right);
		  }
      } else if (!str.compare("$")) { // log
        //evaluation.push(new Log(left, right));
    	  if(!expression) { // if expression doesn't exist yet
			  right->log(left); // right is expression, left is root base
			  evaluation.push(right);
		  }
		  else {
			  left->log(left); // left is root base
			  evaluation.push(left);
		  }
      } else {
        //throw std::domain_error("Unknown operator: '" + str + "'.");
    	  string error = "Unknown operator: '" + str + "'.";
    	  throw (char*)error.c_str();
      }
    //} else if (doubleTok) {
    //  evaluation.push(doubleTok->val);
    } else if (IntegerTok) { // detected Integer
    	//cout << "Integer added to eval: " << IntegerTok->val->getRep() << endl;
    	evaluation.push(new Expression(IntegerTok->val)); // add it to stack
    } else if (ConstantTok) { // detected Constant
        evaluation.push(new Expression(ConstantTok->val)); // add it to stack
    } else if (RootTok) { // detected Constant
        evaluation.push(new Expression(RootTok->val)); // add it to stack
    } else if (LogTok) { // detected Constant
        evaluation.push(new Expression(LogTok->val)); // add it to stack
    } else {
      //throw std::domain_error("Invalid token.");
      throw "Invalid token.";
    }
    delete base;
  }
  // if expression was not initialized, we only had one number, assign number to expression
  if(!expression && !evaluation.empty()) {
	  //cout << "expression was not init" << endl;
	  expression = evaluation.top();
	  evaluation.pop();
  }
  else if (!evaluation.empty()) {
	  expression = evaluation.top();
	  evaluation.pop();
  }
  else {
	  throw "Shunting Yard did not build an expression";
  }
  //cout << "expression: " << expression->getRep() << endl;
  return expression;
}
