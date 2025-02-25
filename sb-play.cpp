#include "disjoint.h"
#include <iostream>

using namespace std;

class Superball {
  public:
    Superball(int argc, char **argv);
    int r;
    int c;
    int mss;
    int empty;
    vector <int> board;
    vector <int> goals;
    vector <int> colors;
};

// void usage(const char *s) 
// {
//   fprintf(stderr, "usage: sb-read rows cols min-score-size colors\n");
//   if (s != NULL) fprintf(stderr, "%s\n", s);
  
//   exit(1);
// }
 
int swap(int &a, int &b) {
  int temp = a;
  a = b;
  b = temp;
  return 0;
}


int main() {

  cout << "This program doesn't do anything yet.\n";
  return 0;

}
