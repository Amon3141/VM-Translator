#include <iostream>
#include <fstream>
#include <filesystem>

#include "Parser.h"
#include "CodeWriter.h"

using namespace std;
namespace fs = std::filesystem;

// @param outputFileName: asm_files/*.asm
// open the output file stream
CodeWriter::CodeWriter(string outputFileName, bool needSysInit) {
  this->symbolRound = 0;
  this->functionName = "";
  setFileName(fs::path(outputFileName).stem().string());
  this->ofile.open(outputFileName);
  ofile << getSPInitializeAssembly() << "\n";
  if (needSysInit) {
    this->writeCall("Sys.init", 0);
  }
}

// @input fileName: * (of vm_file/*.vm)
// set the current file name
void CodeWriter::setFileName(string fileName) {
  this->fileName = fileName;
  this->functionName = "";
}

// @input command: arithmetic command ("add", "eq", etc.)
// translate arithmetic code to assembly code
void CodeWriter::writeArithmetic(string command) {
  string assembly;
  if (command == "add" || command == "sub") {
    assembly = getAddSubAssembly(command);
  } else if (command == "neg") {
    assembly = getNegAssembly();
  } else if (command == "and" || command == "or") {
    assembly = getAndOrAssembly(command);
  } else if (command == "not") {
    assembly = getNotAssembly();
  } else if (command == "eq" || command == "gt" || command == "lt") {
    assembly = getEqGtLtAssembly(command);
  } else {
    cout << "invalid arithmetic command: " << command << endl;
  }
  ofile << assembly << "\n";
}

// translate pushcommand to assembly code
void CodeWriter::writePush(string segment, int index) {
  string assembly;
  if (segment == "constant") {
    assembly = getPushConstantAssembly(index);
  } else if (segment == "static") {
    assembly = getPushStaticAssembly(index);
  } else {  // segment == "local", "argument", "this", "that", "pointer", "temp"
    assembly = getPushSegmentAssembly(segment, index);
  }
  ofile << assembly << "\n";
}

// translate pop command to assembly code
void CodeWriter::writePop(string segment, int index) {
  string assembly;
  if (segment == "static") {
    assembly = getPopStaticAssembly(index);
  } else {  // segment == "local", "argument", "this", "that", "pointer", "temp"
    assembly = getPopSegmentAssembly(segment, index);
  }
  ofile << assembly << "\n";
}

// translate label command to assembly code
void CodeWriter::writeLabel(string label) {
  string assembly = getLabelAssembly(label);
  ofile << assembly << "\n";
}

// translate goto command to assembly code
void CodeWriter::writeGoto(string label) {
  string assembly = getGotoAssembly(label);
  ofile << assembly << "\n";
}

// translate if-goto command to assembly code
void CodeWriter::writeIf(string label) {
  string assembly = getIfAssembly(label);
  ofile << assembly << "\n";
}

// translate (call f n) command to assembly code
void CodeWriter::writeCall(string functionName, int numArgs) {
  string assembly = getCallAssembly(functionName, numArgs);
  ofile << assembly << "\n";
}

// translate return command to assembly code
void CodeWriter::writeReturn() {
  string assembly = getReturnAssembly();
  ofile << assembly << "\n";
}

// translate (function f k) command to assembly code
void CodeWriter::writeFunction(string functionName, int numLocals) {
  this->functionName = "";
  string assembly = getFunctionAssembly(functionName, numLocals);
  ofile << assembly << "\n";
  this->functionName = functionName;
}

// close the streams
void CodeWriter::endWriting() {
  ofile << getEndInfiniteLoopAssembly();
  ofile.close();
}


//--------Prvate--------

string CodeWriter::getSPInitializeAssembly() {
  string assembly =
  "// set SP (RAM[0]) = 256\n"
  "@256\n"
  "D=A\n"
  "@SP\n"
  "M=D\n";
  return assembly;
}

string CodeWriter::getDecrementSPAssembly() {
  string assembly =
  "// SP = SP - 1\n"
  "@SP\n"
  "M=M-1\n"
  "\n"
  "// RAM[SP] = 0\n"
  "@0\n"
  "D=A\n"
  "@SP\n"
  "A=M\n"
  "M=D\n";
  return assembly;
}

string CodeWriter::getEndInfiniteLoopAssembly() {
  string assembly =
  "// infinite loop\n"
  "(END)\n"
  "@END\n"
  "0;JMP\n";
  return assembly;
}

string CodeWriter::getPushConstantAssembly(int x) {
  string assembly = "// push constant x\n";
  assembly += "@" + to_string(x) + "\n";
  assembly +=
  "D=A\n"
  "@SP\n"
  "A=M\n"
  "M=D\n"
  "@SP\n"
  "M=M+1\n";
  return assembly;
}

string CodeWriter::getPushSegmentAssembly(string segment, int x) {
  string assembly = "// push segment x\n";

  if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
    assembly += "@" + segToSymbol[segment] + "\n";
    assembly += "D=M\n";
  } else if (segment == "pointer" || segment == "temp") {
    if (segment == "pointer") assembly += "@3\n";
    else assembly += "@5\n";
    assembly += "D=A\n";
  }

  assembly += "@" + to_string(x) + "\n";
  assembly += 
  "A=D+A\n"
  "D=M\n"
  "@SP\n"
  "A=M\n"
  "M=D\n"
  "@SP\n"
  "M=M+1\n";
  return assembly;
}

string CodeWriter::getPopSegmentAssembly(string segment, int x) {
  string assembly = "// pop segment x\n";

  if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
    assembly += "@" + segToSymbol[segment] + "\n";
    assembly += "D=M\n";
  } else if (segment == "pointer" || segment == "temp") {
    if (segment == "pointer") assembly += "@3\n";
    else assembly += "@5\n";
    assembly += "D=A\n";
  }

  assembly += "@" + to_string(x) + "\n";
  assembly +=
  "D=D+A\n"
  "@R13\n"
  "M=D\n"
  "\n"
  "@SP\n"
  "A=M-1\n"
  "D=M\n"
  "\n"
  "@R13\n"
  "A=M\n"
  "M=D\n";
  assembly += "\n" + getDecrementSPAssembly();
  return assembly;
}

string CodeWriter::getPushStaticAssembly(int x) {
  string assembly = "// push static x\n";
  assembly += "@" + this->fileName + "." + to_string(x) + "\n";
  assembly +=
  "D=M\n"
  "@SP\n"
  "A=M\n"
  "M=D\n"
  "@SP\n"
  "M=M+1\n";
  return assembly;
}

string CodeWriter::getPopStaticAssembly(int x) {
  string assembly = "// pop static x\n";
  assembly +=
  "@SP\n"
  "A=M-1\n"
  "D=M\n";
  assembly += "@" + this->fileName + "." + to_string(x) + "\n";
  assembly += "M=D\n";
  assembly += "\n" + getDecrementSPAssembly();
  return assembly;
}

// @require (command == "add" || command == "sub")
string CodeWriter::getAddSubAssembly(string command) {
  if (!(command == "add" || command == "sub")) {
    cout << "command should be ADD or SUB" << endl;
    return "";
  }

  string assembly;
  if (command == "add") assembly += "//add\n";
  else if (command == "sub") assembly += "//sub\n";

  assembly +=
  "@SP\n"
  "A=M\n"
  "A=A-1\n"
  "D=M\n"
  "A=A-1\n";

  if (command == "add") assembly += "D=M+D\n";
  else if (command == "sub") assembly += "D=M-D\n";
  else return "";

  assembly += "M=D\n";
  assembly += "\n" + getDecrementSPAssembly();
  return assembly;
}

string CodeWriter::getNegAssembly() {
  string assembly =
  "// neg\n"
  "@SP\n"
  "A=M\n"
  "A=A-1\n"
  "M=-M\n";
  return assembly;
}

// @require (command == "eq" || command == "gt" || command == "lt")
string CodeWriter::getEqGtLtAssembly(string command) {
  if (!(command == "eq" || command == "gt" || command == "lt")) {
    cout << "command should be EQ or GT or LT" << endl;
    return "";
  }

  string assembly;
  if (command == "eq") assembly += "//eq\n";
  else if (command == "gt") assembly += "//gt\n";
  else if (command == "lt") assembly += "//lt\n";

  assembly +=
  "@SP\n"
  "A=M\n"
  "A=A-1\n"
  "D=M\n"
  "A=A-1\n"
  "D=M-D\n"
  "@" + this->fileName + ".TRUE" + to_string(symbolRound) + "\n";

  if (command == "eq") assembly += "D;JEQ\n";
  else if (command == "gt") assembly += "D;JGT\n";
  else if (command == "lt") assembly += "D;JLT\n";
  else return "";
  
  assembly +=
  "@" + this->fileName + ".FALSE" + to_string(symbolRound) + "\n"
  "0;JMP\n"
  "(" + this->fileName + ".TRUE" + to_string(symbolRound) + ")\n"
  "@0\n"
  "D=!A\n"
  "@" + this->fileName + ".ENDIF" + to_string(symbolRound) + "\n"
  "0;JMP\n"
  "(" + this->fileName + ".FALSE" + to_string(symbolRound) + ")\n"
  "@0\n"
  "D=A\n"
  "("+ this->fileName + ".ENDIF" + to_string(symbolRound) + ")\n"
  "@SP\n"
  "A=M\n"
  "A=A-1\n"
  "A=A-1\n"
  "M=D\n";
  assembly += "\n" + getDecrementSPAssembly();

  symbolRound++;
  return assembly;
}

// @require (command == "and" || command == "or")
string CodeWriter::getAndOrAssembly(string command) {
  if (!(command == "and" || command == "or")) {
    cout << "command should be AND or OR" << endl;
    return "";
  }

  string assembly;
  if (command == "and") assembly += "//and\n";
  else if (command == "or") assembly += "//or\n";

  assembly +=
  "@SP\n"
  "A=M\n"
  "A=A-1\n"
  "D=M\n"
  "A=A-1\n";

  if (command == "and") assembly += "D=D&M\n";
  else if (command == "or") assembly += "D=D|M\n";

  assembly += "M=D\n";
  assembly += "\n" + getDecrementSPAssembly();
  return assembly;
}

string CodeWriter::getNotAssembly() {
  string assembly =
  "//not\n"
  "@SP\n"
  "A=M\n"
  "A=A-1\n"
  "M=!M\n";
  return assembly;
}

string CodeWriter::getLabelAssembly(string label) {
  string assembly = "// label xxx\n";
  if (!this->functionName.empty()) {
    assembly += "(" + this->fileName + "." + this->functionName + ".";
  } else {
    assembly += "(" + this->fileName + ".";
  }
  assembly += label + ")\n";
  return assembly;
}

string CodeWriter::getGotoAssembly(string label) {
  string assembly = "// goto xxx\n";
  if (!this->functionName.empty()) {
    assembly += "@" + this->fileName + "." + this->functionName + ".";
  } else {
    assembly += "@" + this->fileName + ".";
  }
  assembly += label + "\n";
  assembly += "0;JMP\n";
  return assembly;
}

string CodeWriter::getIfAssembly(string label) {
  string assembly = "//if-goto xxx \n";
  assembly += 
  "@SP\n"
  "M=M-1\n"
  "A=M\n"
  "D=M\n"
  "@R13\n"
  "M=D\n"
  "@0\n"
  "D=A\n"
  "@SP\n"
  "A=M\n"
  "M=D\n"
  "@R13\n"
  "D=M\n";
  if (!this->functionName.empty()) {
    assembly += "@" + this->fileName + "." + this->functionName + ".";
  } else {
    assembly += "@" + this->fileName + ".";
  }
  assembly += label + "\n";
  assembly += "D;JNE\n";
  return assembly;
}

string CodeWriter::getFunctionAssembly(string functionName, int numLocal) {
  // f = functionName, k = numLocal
  string assembly = "// function f k\n";
  assembly += "(" + functionName + ")\n";
  assembly += "@" + to_string(numLocal) + "\n";
  assembly += "D=A\n";
  assembly +=
  "@13\n"
  "M=D\n";
  assembly += "(" + functionName + ".init.START)\n";
  assembly +=
  "@13\n"
  "D=M\n";
  assembly += "@" + functionName + ".init.END\n";
  assembly += "D;JLE\n";
  assembly += "\n" + getPushConstantAssembly(0) + "\n";
  assembly +=
  "@13\n"
  "M=M-1\n";
  assembly += "@" + functionName + ".init.START\n";
  assembly += "0;JMP\n";
  assembly += "(" + functionName + ".init.END)\n";
  return assembly;
}

string CodeWriter::getCallAssembly(string functionName, int numArgs) {
  // f = functionName, n = numArgs
  string assembly = "// call f n\n";
  assembly += "@Return" + to_string(this->symbolRound) + "\n";
  assembly += "D=A\n";
  assembly += "\n" + getPushConstantInD() + "\n";
  assembly +=
  "@LCL\n"
  "D=M\n";
  assembly += "\n" + getPushConstantInD() + "\n";
  assembly +=
  "@ARG\n"
  "D=M\n";
  assembly += "\n" + getPushConstantInD() + "\n";
  assembly +=
  "@THIS\n"
  "D=M\n";
  assembly += "\n" + getPushConstantInD() + "\n";
  assembly +=
  "@THAT\n"
  "D=M\n";
  assembly += "\n" + getPushConstantInD() + "\n";
  assembly +=
  "// ARG = SP - n - 5\n"
  "@SP\n"
  "D=M\n";
  assembly += "\n" + getPushConstantInD();
  assembly += "\n" + getPushConstantAssembly(numArgs + 5);
  assembly += "\n" + getAddSubAssembly("sub") + "\n";
  assembly +=
  "@SP\n"
  "M=M-1\n"
  "A=M\n"
  "D=M\n"
  "@ARG\n"
  "M=D\n"
  "\n"
  "@0\n"
  "D=A\n"
  "@SP\n"
  "A=M\n"
  "M=D\n"
  "\n"
  "// LCL = SP\n"
  "@SP\n"
  "D=M\n"
  "@LCL\n"
  "M=D\n";
  assembly += "\n@" + functionName + "\n";
  assembly += "0;JMP\n";
  assembly += "(Return" + to_string(this->symbolRound) + ")\n";
  this->symbolRound++;
  return assembly;
}

string CodeWriter::getReturnAssembly() {
  string assembly = "// return\n";
  assembly +=
  "@LCL\n"
  "D=M\n"
  "@R14 // =FRAME\n"
  "M=D\n";
  assembly += "\n" + getPushConstantInD();
  assembly += "\n" + getPushConstantAssembly(5);
  assembly += "\n" + getAddSubAssembly("sub") + "\n";
  assembly +=
  "@SP\n"
  "M=M-1\n"
  "A=M\n"
  "A=M\n"
  "D=M\n"
  "@R15 // =RET\n"
  "M=D\n"
  "@0\n"
  "D=A\n"
  "@SP\n"
  "A=M\n"
  "M=D\n";
  assembly += "\n" + getPopSegmentAssembly("argument", 0) + "\n";
  assembly +=
  "@ARG\n"
  "D=M+1\n"
  "@SP\n"
  "M=D\n"
  "\n"
  "@R14\n"
  "M=M-1\n"
  "A=M\n"
  "D=M\n"
  "@THAT\n"
  "M=D\n"
  "\n"
  "@R14\n"
  "M=M-1\n"
  "A=M\n"
  "D=M\n"
  "@THIS\n"
  "M=D\n"
  "\n"
  "@R14\n"
  "M=M-1\n"
  "A=M\n"
  "D=M\n"
  "@ARG\n"
  "M=D\n"
  "\n"
  "@R14\n"
  "M=M-1\n"
  "A=M\n"
  "D=M\n"
  "@LCL\n"
  "M=D\n"
  "\n"
  "@15\n"
  "A=M // =RET\n"
  "0;JMP\n";
  return assembly;
}


string CodeWriter::getPushConstantInD() {
  string assembly = "// push constant in D\n";
  assembly +=
  "@SP\n"
  "A=M\n"
  "M=D\n"
  "@SP\n"
  "M=M+1\n";
  return assembly;
}

// set the current function's name
void CodeWriter::setFunctionName(string functionName) {
  this->functionName = functionName;
}