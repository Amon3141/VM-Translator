#include <iostream>
#include <fstream>

#include "Parser.h"

using namespace std;

// open a file & prepare for parsing
Parser::Parser(string fileName) {
  vmfile.open(fileName);
}

// return if there is any more commands left
bool Parser::hasNextCommand() {
  return !vmfile.eof();
}

void Parser::advance() {
  getline(vmfile, currentCommand);
}

string Parser::commandType() {
  string commandType;
  string op = currentCommand.substr(0, currentCommand.find(" "));
  removeSpaces(op);
  if (op == "push") commandType = "C_PUSH";
  else if (op == "pop") commandType = "C_POP";
  else if (op == "label") commandType = "C_LABEL";
  else if (op == "goto") commandType = "C_GOTO";
  else if (op == "if-goto") commandType = "C_IF";
  else if (op == "function") commandType = "C_FUNCTION";
  else if (op == "call") commandType = "C_CALL";
  else if (op == "return") commandType = "C_RETURN";
  else if (op == "add" || op == "sub" || op == "neg" || op == "eq" || op == "gt" || op == "lt" || op == "not" || op == "and" || op == "or") commandType = "C_ARITHMETIC";
  else commandType = "SKIP";
  return commandType;
}

// @require commantType() != "C_RETURN"
// return the first argument of the current command
string Parser::arg1() {
  string arg1;
  string commandType = this->commandType();
  if (commandType == "C_ARITHMETIC" || commandType == "SKIP") arg1 = currentCommand;
  else {
    string args = currentCommand.substr(currentCommand.find(" ")+1);
    arg1 = args.substr(0, args.find(" "));
  }
  removeSpaces(arg1);
  return arg1;
}

int Parser::arg2() {
  string commandType = this->commandType();
  int arg2 = 0;
  if (commandType == "C_PUSH" || commandType == "C_POP" || commandType == "C_FUNCTION" || commandType == "C_CALL") {
    string args = currentCommand.substr(currentCommand.find(" ")+1);
    string arg2_str = args.substr(args.find(" ")+1);
    removeSpaces(arg2_str);
    arg2 = stoi(arg2_str);
  }
  return arg2;
}

void Parser::endParsing() {
  vmfile.close();
}

// remove any white spaces (" ", \n, \t, etc.) from string
void Parser::removeSpaces(string &str) {
  str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
}