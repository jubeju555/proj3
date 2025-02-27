#include "disjoint.h"
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cstring>

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
void usage(const char *s)
{
  fprintf(stderr, "usage: sb-read rows cols min-score-size colors\n");
  if (s != NULL)
    fprintf(stderr, "%s\n", s);

  exit(1);
}
Superball::Superball(int argc, char **argv)
{
  int i, j;
  string s;

  if (argc != 5)
    usage(NULL);

  if (sscanf(argv[1], "%d", &row) == 0 || row <= 0)
    usage("Bad rows");
  if (sscanf(argv[2], "%d", &column) == 0 || column <= 0)
    usage("Bad cols");
  if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0)
    usage("Bad min-score-size");

  colors.resize(256, 0);

  for (i = 0; i < (int)strlen(argv[4]); i++)
  {
    if (!isalpha(argv[4][i]))
      usage("Colors must be distinct letters");
    if (!islower(argv[4][i]))
      usage("Colors must be lowercase letters");
    if (colors[argv[4][i]] != 0)
      usage("Duplicate color");
    colors[argv[4][i]] = 2 + i;
    colors[toupper(argv[4][i])] = 2 + i;
  }

  board.resize(row * column);
  goals.resize(row * column, 0);

  empty = 0;

  for (i = 0; i < row; i++)
  {
    if (!(cin >> s))
    {
      fprintf(stderr, "Bad board: not enough rows on standard input\n");
      exit(1);
    }
    if ((int)s.size() != column)
    {
      fprintf(stderr, "Bad board on row %d - wrong number of characters.\n", i);
      exit(1);
    }
    for (j = 0; j < column; j++)
    {
      if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0)
      {
        fprintf(stderr, "Bad board row %d - bad character %c.\n", i, s[j]);
        exit(1);
      }
      board[i * column + j] = s[j];
      if (board[i * column + j] == '.')
        empty++;
      if (board[i * column + j] == '*')
        empty++;
      if (isupper(board[i * column + j]) || board[i * column + j] == '*')
      {
        goals[i * column + j] = 1;
        board[i * column + j] = tolower(board[i * column + j]);
      }
    }
  }
}
struct Metadata
{
  int size;
  bool has_goal;
  int scorecell;
};

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

      int root = ds.Find(currentindex);
      // adds root if not in scoringset
      if (scoringset.find(root) == scoringset.end())
      {
        scoringset[root] = {1, s->goals[currentindex] != 0, s->goals[currentindex] ? currentindex : -1};
      }

      // checks the column to the right
      if (j + 1 < s->column && s->board[currentindex] == s->board[currentindex + 1])
      {
        ds.Union(ds.Find(currentindex), ds.Find(currentindex + 1));
      }
      // checks the row below
      if (i + 1 < s->row && s->board[currentindex] == s->board[currentindex + s->column])
      {
        ds.Union(ds.Find(currentindex), ds.Find(currentindex + s->column));
      }
    }
  }

  for (int i = 0; i < s->row; i++)
  {
    for (int j = 0; j < s->column; j++)
    {
      int currentindex = i * s->column + j;
      // Skip empty cells
      if (s->board[currentindex] == '.' || s->board[currentindex] == '*')
        continue;

      int root = ds.Find(currentindex);

      // If the root is already in the scoring set, update its metadata
      if (scoringset.find(root) != scoringset.end())
      {
        scoringset[root].size++;
        scoringset[root].has_goal |= (s->goals[currentindex] != 0);
        // update if cell is scoring cell and pick earlist scoring cell
        if (s->goals[currentindex] && (scoringset[root].scorecell == -1 || currentindex < scoringset[root].scorecell))
        {
          scoringset[root].scorecell = currentindex;
        }
      }
    }
  }
}
void bestmove(Superball *s, DisjointSetByRankWPC &ds, unordered_map<int, Metadata> &scoringset)
{
  int bestswapI = -1;
  int bestswapJ = -1;
  int bestscore = 0;

  if (s->empty < 5)
  {

    for (int i = 0; i < s->row; i++)
    {
      for (int j = 0; j < s->column; j++)
      {
        if (s->board[i * s->column + j] == '.' || s->board[i * s->column + j] == '*')
          continue;
        swap(s->board[i * s->column + j], s->board[i * s->column + j + 1]);
        int scoresize = ds.Find(i * s->column + j);
        if (scoresize > bestscore)
        {
          bestscore = scoresize;
          bestswapI = i;
          bestswapJ = j;
        }
        swap(s->board[i * s->column + j], s->board[i * s->column + j + 1]);
      }
    }
  }
  if (bestswapI != -1 && bestswapJ != -1)
  {
    cout << "Swap: " << bestswapI << "," << bestswapJ << " with " << bestswapI << "," << bestswapJ + 1 << endl;
  }
  else
  {
    int secondbestsize = 0;
    int secondbestrow = -1;
    int secondbestcol = -1;

    for (unordered_map<int, Metadata>::iterator it = scoringset.begin(); it != scoringset.end(); it++)
    {
      Metadata data = it->second;

      if (data.size >= s->mss && data.has_goal && data.size > 1 && data.scorecell != -1)
      {
        secondbestsize = data.size;
        int Grow = data.scorecell / s->column;
        int Gcol = data.scorecell % s->column;
      }
    }
  }
  if (bestscore > 0)
  {
   printf("Score at %d, %d\n", bestswapI, bestswapJ);
  }
  
}

//   return;
// }

// for (int i = 0; i < s->row; i++)
// {
//   for (int j = 0; j < s->column - 1; j++)
//   {
//     swap(s->board[i * s->column + j], s->board[i * s->column + j + 1]);
//     cout << "Swap: " << i << "," << j << " with " << i << "," << j + 1 << endl;
//     return;
//   }
// }

int main(int argc, char **argv)
{
  Superball *s;
  s = new Superball(argc, argv);
  DisjointSetByRankWPC ds(s->row * s->column);
  unordered_map<int, Metadata> scoringset;
  unordered_map<int, int> scoringcell;
  // cout << "This program doesn't do anything yet.\n";
  sbanalyze(s, ds, scoringset, scoringcell);
  bestmove(s, ds, scoringset);

  return 0;
}