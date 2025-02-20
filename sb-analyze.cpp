#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include "disjoint.h"
#include <map>
#include <unordered_map>
#include <algorithm>

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
  private:

  // DisjointSetByRankWPC ds;
  // vector<vector<int> > board;
  int rows, cols;
};

void usage(const char *s) 
{
  fprintf(stderr, "usage: sb-analyze rows cols min-score-size colors\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

Superball::Superball(int argc, char **argv)
{
  int i, j;
  string s;

  if (argc != 5) usage(NULL);

  if (sscanf(argv[1], "%d", &row) == 0 || row <= 0) usage("Bad rows");
  if (sscanf(argv[2], "%d", &column) == 0 || column <= 0) usage("Bad cols");
  if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0) usage("Bad min-score-size");

  colors.resize(256, 0);

  for (i = 0; i < (int) strlen(argv[4]); i++) {
    if (!isalpha(argv[4][i])) usage("Colors must be distinct letters");
    if (!islower(argv[4][i])) usage("Colors must be lowercase letters");
    if (colors[argv[4][i]] != 0) usage("Duplicate color");
    colors[argv[4][i]] = 2+i;
    colors[toupper(argv[4][i])] = 2+i;
  }

  board.resize(row*column);
  goals.resize(row*column, 0);

  empty = 0;

  for (i = 0; i < row; i++) {
    if (!(cin >> s)) {
      fprintf(stderr, "Bad board: not enough rows on standard input\n");
      exit(1);
    }
    if ((int) s.size() != column) {
      fprintf(stderr, "Bad board on row %d - wrong number of characters.\n", i);
      exit(1);
    }
    for (j = 0; j < column; j++) {
      if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0) {
        fprintf(stderr, "Bad board row %d - bad character %c.\n", i, s[j]);
        exit(1);
      }
      board[i*column+j] = s[j];
      if (board[i*column+j] == '.') empty++;
      if (board[i*column+j] == '*') empty++;
      if (isupper(board[i*column+j]) || board[i*column+j] == '*') {
        goals[i*column+j] = 1;
        board[i*column+j] = tolower(board[i*column+j]);
      }
    }
  }
}

int main(int argc, char **argv)
{
  Superball *s;
  s = new Superball(argc, argv);
  DisjointSetByRankWPC ds(s->row*s->column);
  
 
 
  unordered_map<int, int> scoringset;
  map<int, bool> scoregoal;

  for (int i = 0; i < s->row; i++)
  {
    for (int j = 0; j < s->column; j++)
  {
    int currentindex = i * s->column + j;

    if (s->board[currentindex] == '.' || s->board[currentindex] == '*'){
      continue;
    }
   // find the root of the currentindex
    int root = ds.Find(currentindex);
    // if the root is not in the scoringset, add it to the scoringset else increment the value
    if (scoringset.find(root) == scoringset.end()){
      scoringset[root] = 1;
    } else {
      scoringset[root]++;
    }
    if (s->goals[currentindex] && !scoregoal[root]) scoregoal[root] = true;
    if (s->goals[currentindex] == scoregoal[root]) scoregoal[root] = true;


  }
} 

for (int i = 0; i < s->row; i++)
  {
    for (int j = 0; j < s->column; j++)
    {
      int currentindex = i * s->column + j;
      if (s->board[currentindex] == '.' || s->board[currentindex] == '*'){
        continue;
      }
      // this checks if the currentindex is the same as the one to the right and checks out of bounds
      if (j + 1 < s->column && s->board[currentindex] == s->board[currentindex + 1]){
        if (ds.Find(currentindex) != -1 && ds.Find(currentindex + 1) != -1)
        {
          ds.Union(currentindex, currentindex + s->column);

        }
        
      }
      // this checks the one below and has a checks out of bounds
      if (i + 1 < s->row && s->board[currentindex] == s->board[currentindex + s->column]){
        
        if (ds.Find(currentindex) != -1 && ds.Find(currentindex + s->column) != -1)
        {
          ds.Union(currentindex, currentindex + 1);

        }
      }
      
    }
    
  }
  ds.Print();
  
}
