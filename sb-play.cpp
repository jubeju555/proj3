#include "disjoint.h"
#include <iostream>

using namespace std;

class Superball {
  public:
    Superball(int argc, char **argv);
    int row;
    int column;
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

void bestmove(Superball *s, DisjointSetByRankWPC &ds){
  cout << "This function doesn't do anything yet.\n";
  for (int i = 0; i < s->row; i++)
  {
    for (int j = 0; j < s->column; j++)
    {
      int currentindex = i * s->column + j;
      if (s->board[currentindex] == '.' || s->board[currentindex] == '*')
        continue;
    }
  }
}
int main() {

  cout << "This program doesn't do anything yet.\n";
  return 0;

}
