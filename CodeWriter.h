#include <fstream>

using namespace std;

#ifndef CODEWRITER_H
#define CODEWRITER_H

class CodeWriter {
public:
  CodeWriter(string fileName);
  void setFileName(string fileName);
  void writeArithmetic(string command);
  void writePush(string segment, int index);
  void writePop(string segment, int index);
  void endWriting();

private:
  string fileName;
  ofstream ofile;
  int symbolRound;
  unordered_map<string, string> segToSymbol = {
    {"local", "LCL"}, {"argument", "ARG"}, {"this", "THIS"}, {"that", "THAT"}
  };
  string getSPInitializeAssembly();
  string getAfterTwoArgsAssembly();
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
};

#endif