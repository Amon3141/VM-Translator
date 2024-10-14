#include <iostream>
#include <filesystem>
#include <vector>

#include "Parser.h"
#include "CodeWriter.h"

using namespace std;
namespace fs = std::filesystem;

// @input: path to the directory: vm_files/*
// returns the list of VM file names in the directory
vector<string> getVMFiles(const string &path) {
  vector<string> vmFiles;
  for (auto &entry : fs::directory_iterator(path)) {
    if (entry.path().extension() == ".vm") {
      vmFiles.push_back(entry.path().string());
    }
  }
  return vmFiles;
}

int main() {
  // get path to the VM file or directory
  string inputPath;
  cout << "Name of the VM file or directory containing VM files (inside vm_files/): ";
  cin >> inputPath;

  vector<string> filesToProcess;
  bool needSysInit;

  // if the inputPath starts with "vm_files/", remove it
  if (inputPath.substr(0, inputPath.find("/")) == "vm_files") {
    inputPath = inputPath.substr(inputPath.find("/") + 1);
  }

  // construct filesToProcess
  if (fs::is_directory("vm_files/" + inputPath)) { // if inputPath is a directry, get all the VM files inside it
    filesToProcess = getVMFiles("vm_files/" + inputPath);
    needSysInit = true;
  } else { // if inputPath is a fileName
    if (inputPath.find(".vm") == string::npos) { // if the fileName doesn't have .vm extension, add it
      inputPath += ".vm";
    }
    string fullPath = "vm_files/" + inputPath;

    // check if the file exists
    if (fs::exists(fullPath)) {
      filesToProcess.push_back(fullPath);
    } else {
      cout << "Error: File '" << fullPath << "' does not exists." << endl;
      return 1;
    }
    needSysInit = false;
  }

  // if no valid .vm files found, print an error message
  if (filesToProcess.empty()) {
    cout << "No .vm files found to process." << endl;
    return 1;
  }

  // initialize CodeWriter with the output filename
  string outputFileName = inputPath.substr(0, inputPath.find(".")) + ".asm";
  CodeWriter writer("asm_files/" + outputFileName, needSysInit);

  // process each VM file
  for (auto file : filesToProcess) {
    cout << "Processing file: " << file << endl;

    writer.setFileName(fs::path(file).stem().string());
    Parser parser(file);

    // process the VM file line by line
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
      } else if (commandType == "C_CALL") {
        writer.writeCall(parser.arg1(), parser.arg2());
      } else if (commandType == "C_RETURN") {
        writer.writeReturn();
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
    parser.endParsing();
  }

  // finish parser and code-writer
  writer.endWriting();

  cout << "VM translation completed. Output file: " << outputFileName << endl;
  return 0;
}

// g++ -std=c++20 -o program CodeWriter.cpp Parser.cpp main.cpp