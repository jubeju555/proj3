#include "disjoint.h"
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cstring>
using namespace std;
// TO RUN:  sh run_multiple.sh 8 10 5 pbyrg bin/sb-play 10 1
// TO RUN SINGLE TIME: time bin/sb-play 8 10 5 pbyrg <input-1.txt
// TESTING: /home/jplank/cs302/Labs/Lab5/bin/sb-player 8 10 5 pbyrg bin/sb-play y y -
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
void swap(int &a, int &b)
{
  int temp = a;
  a = b;
  b = temp;
}

void sbanalyze(Superball *s, DisjointSetByRankWPC &ds, unordered_map<int, Metadata> &scoringset, unordered_map<int, int> &scoringcell)
{
  scoringset.clear();
  scoringcell.clear();

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
  // first set
  int bestSwapI = -1, bestSwapJ = -1;
  // second set
  int bestSwapX = -1, bestSwapY = -1;
  // prem score
  int bestScore = 0;

  unordered_map<int, Metadata> tempScoringSet;
  unordered_map<int, int> tempScoringCell;
  
  //  score first policy 
  for (unordered_map<int, Metadata>::iterator it = scoringset.begin(); it != scoringset.end(); it++)
  {
    Metadata data = it->second;
    if (data.size >= s->mss && data.has_goal && data.scorecell != -1)
    {
      int scoreRow = data.scorecell / s->column;
      int scoreCol = data.scorecell % s->column;
      cout << "SCORE " << scoreRow << " " << scoreCol << endl;
      return;
    }
  }

  // No immediate score — try to set up a future score
  int maxpotentialscore = 0;
  vector<pair<int, int>> candidates;

  // read through for possible swaps
  for (int i = 0; i < s->row; i++)
  {
    for (int j = 0; j < s->column; j++)
    {
      if (s->board[i * s->column + j] != '.' && s->board[i * s->column + j] != '*' && s->board[i * s->column + j] != 0)
      {
        candidates.push_back(make_pair(i, j));
      }
    }
  }

  // i want to swap and unswap and find the best potential swap score
  for (size_t pair1 = 0; pair1 < candidates.size(); pair1++)
  {
    for (size_t pair2 = pair1 + 1; pair2 < candidates.size(); pair2++)
    {
      int i1 = candidates[pair1].first;
      int j1 = candidates[pair1].second;

      int i2 = candidates[pair2].first;
      int j2 = candidates[pair2].second;

      // Swap then analyze
      swap(s->board[i1 * s->column + j1], s->board[i2 * s->column + j2]);
      
      sbanalyze(s, ds, tempScoringSet, tempScoringCell);

      // Find the best potential score after the swap
      int potentialscore = 0;
      for (unordered_map<int, Metadata>::iterator it = tempScoringSet.begin(); it != tempScoringSet.end(); it++)
      {
        Metadata data = it->second;
        if (data.size >= s->mss && data.has_goal)
        {
          potentialscore += data.size;
        }
      }

      // find best move on current board
      if (potentialscore > maxpotentialscore)
      {
        maxpotentialscore = potentialscore;
        bestSwapI = i1;
        bestSwapJ = j1;
        bestSwapX = i2;
        bestSwapY = j2;
      }

      // Undo the swap
      swap(s->board[i1 * s->column + j1], s->board[i2 * s->column + j2]);
    }
  }

  // Output the best swap
  if (maxpotentialscore > 0)
  {
    cout << "SWAP " << bestSwapI << " " << bestSwapJ << " " << bestSwapX << " " << bestSwapY << endl;
  }
}

void printBoard(Superball *s)
{
  for (int i = 0; i < s->row; i++)
  {
    for (int j = 0; j < s->column; j++)
    {
      char cell = s->board[i * s->column + j];
      if (s->goals[i * s->column + j])
      {
        cell = toupper(cell);
      }
      cout << cell << " ";
    }
    cout << endl;
  }
}

int main(int argc, char **argv)
{
  Superball *s;
  s = new Superball(argc, argv);
  DisjointSetByRankWPC ds(s->row * s->column);
  unordered_map<int, Metadata> scoringset;
  unordered_map<int, int> scoringcell;
  // cout << "This program doesn't do anything yet.\n";
  for (int i = 0; i < 5; i++)
  {
    sbanalyze(s, ds, scoringset, scoringcell);
    bestmove(s, ds, scoringset);
    // printBoard(s);
    // sbanalyze(s, ds, scoringset, scoringcell);
    // cin.get();
  }

  return 0;
}
