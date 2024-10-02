#include <fstream>

using namespace std;

#ifndef PARSER_H
#define PARSER_H

class Parser {
public:
  Parser(string fileName);
  bool hasNextCommand();
  void advance();
  string commandType();
  string arg1();
  int arg2();
  void endParsing();

private:
  string currentCommand;
  ifstream vmfile;

  void removeSpaces(string &str);
};

#endif