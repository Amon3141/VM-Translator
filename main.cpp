#include <iostream>

#include "Parser.h"
#include "CodeWriter.h"

using namespace std;

int main() {
  // get file name
  string fileName;
  cin >> fileName;
  if (fileName.find(".vm") == string::npos) fileName += ".vm";
  fileName = "vm_files/" + fileName;

  // initialize parser and code-writer
  Parser parser(fileName);
  CodeWriter writer(fileName);

  // process vm file line by line
  while (parser.hasNextCommand()) {
    parser.advance();
    string commandType = parser.commandType();
    if (commandType == "C_ARITHMETIC") {
      writer.writeArithmetic(parser.arg1());
    } else if (commandType == "C_PUSH") {
      writer.writePush(parser.arg1(), parser.arg2());
    } else if (commandType == "C_POP") {
      writer.writePop(parser.arg1(), parser.arg2());
    } else if (commandType == "C_FUNCTION") {
      writer.writeFunction(parser.arg1(), parser.arg2());
    } else if (commandType == "C_LABEL") {
      writer.writeLabel(parser.arg1());
    } else if (commandType == "C_GOTO") {
      writer.writeGoto(parser.arg1());
    } else if (commandType == "C_IF") {
      writer.writeIf(parser.arg1());
    } else {
      continue;
    }
  }

  // finish parser and code-writer
  parser.endParsing();
  writer.endWriting();

  return 0;
}

// g++ -std=c++20 -o program CodeWriter.cpp Parser.cpp main.cpp