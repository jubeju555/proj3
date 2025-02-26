#include "disjoint.h"
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>

using namespace std;

class Superball
{
public:
  Superball(int argc, char **argv);
  int row;
  int column;
  int mss;
  int empty;
  vector<int> board;
  vector<int> goals;
  vector<int> colors;
};
struct Metadata
{
  int size;
  bool has_goal;
  int scorecell;
};
// void usage(const char *s)
// {
//   fprintf(stderr, "usage: sb-read rows cols min-score-size colors\n");
//   if (s != NULL) fprintf(stderr, "%s\n", s);

//   exit(1);
// }

int swap(int &a, int &b)
{
  int temp = a;
  a = b;
  b = temp;
  return 0;
}
void sbanalyze(Superball *s, DisjointSetByRankWPC &ds, unordered_map<int, Metadata> &scoringset, unordered_map<int, int> &scoringcell)
{
  for (int i = 0; i < s->row; i++)
  {
    for (int j = 0; j < s->column; j++)
    {
      int currentindex = i * s->column + j;
      if (s->board[currentindex] == '.' || s->board[currentindex] == '*')
        continue;

      // this checks the column to the right
      if (j + 1 < s->column && s->board[currentindex] == s->board[currentindex + 1])
      {
        ds.Union(ds.Find(currentindex), ds.Find(currentindex + 1));
      }
      // this checks the row below
      if (i + 1 < s->row && s->board[currentindex] == s->board[currentindex + s->column])
      {
        ds.Union(ds.Find(currentindex), ds.Find(currentindex + s->column));
      }
    }
  }
}
void bestmove(Superball *s, DisjointSetByRankWPC &ds, unordered_map<int, Metadata> &scoringset)
{

  for (unordered_map<int, Metadata>::iterator it = scoringset.begin(); it != scoringset.end(); it++)
  {
    Metadata data = it->second;

    if (data.size >= s->mss && data.has_goal && data.size > 1 && data.scorecell != -1)
    {
      int Grow = data.scorecell / s->column;
      int Gcol = data.scorecell % s->column;
      printf("Score at %d, %d\n", Grow, Gcol);
    }
    return;
  }
  for (int i = 0; i < s->row; i++)
  {
    for (int j = 0; j < s->column - 1; j++)
    {
      swap(s->board[i * s->column + j], s->board[i * s->column + j + 1]);
      cout << "Swap: " << i << "," << j << " with " << i << "," << j + 1 << endl;
      return;
    }
  }
}

int main(int argc, char **argv)
{
  Superball *s;
  s = new Superball(argc, argv);
  DisjointSetByRankWPC ds(s->row * s->column);
  unordered_map<int, Metadata> scoringset;
  unordered_map<int, int> scoringcell;
  cout << "This program doesn't do anything yet.\n";
  sbanalyze(s, ds, scoringset, scoringcell);
  bestmove(s, ds, scoringset);

  return 0;
}
