#include <iostream>

using namespace std;

void removeSpaces(string &str) {
  str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
}

int main() {
  string t = "push constant 7";
  string args = t.substr(t.find(" ")+1);
  cout << args << " desu" << endl;
  removeSpaces(args);
  cout << args << " desu" << endl;
  int arg2 = stoi(args.substr(args.find(" ")+1));
  cout << arg2 << endl;
}