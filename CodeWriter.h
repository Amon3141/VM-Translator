#include <fstream>

using namespace std;

#ifndef CODEWRITER_H
#define CODEWRITER_H

// TODOS
// 1. complete label, goto, funciton-related translation functions ✅
// 2. support for directly input (multiple files)
//  2-1. adjust to directly input
//  2-2. search for Sys.init and related error handling
//  2-3. remove getSPInitializeAssembly() if multi-file input,
//       keep if single-file input

class CodeWriter {
public:
  CodeWriter(string fileName, bool needSysInit);
  void setFileName(string fileName);
  void writeArithmetic(string command);
  void writePush(string segment, int index);
  void writePop(string segment, int index);
  void writeLabel(string label);
  void writeGoto(string label);
  void writeIf(string label);
  void writeCall(string functionName, int numArgs);
  void writeReturn();
  void writeFunction(string functionName, int numLocals);
  void endWriting();

private:
  ofstream ofile; // output asm file
  string fileName; // current file that are being parsed
  string functionName; // current function, NULL if at top-level
  int symbolRound; // for making internal symbols unique (for eq, gt, lt)
  unordered_map<string, string> segToSymbol = {
    {"local", "LCL"}, {"argument", "ARG"}, {"this", "THIS"}, {"that", "THAT"}
  };
  string getSPInitializeAssembly();
  string getDecrementSPAssembly();
  string getEndInfiniteLoopAssembly();
  string getPushConstantAssembly(int x);
  string getPushSegmentAssembly(string segment, int x);
  string getPopSegmentAssembly(string segment, int x);
  string getPushStaticAssembly(int x);
  string getPopStaticAssembly(int x);
  string getAddSubAssembly(string command);
  string getNegAssembly();
  string getEqGtLtAssembly(string command);
  string getAndOrAssembly(string command);
  string getNotAssembly();

  string getLabelAssembly(string label);
  string getGotoAssembly(string label);
  string getIfAssembly(string label);
  string getCallAssembly(string functionName, int numArgs);
  string getReturnAssembly();
  string getFunctionAssembly(string functionName, int numLocalas);

  string getPushConstantInD();
  void setFunctionName(string functionName);
};

#endif