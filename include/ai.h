#ifndef AI_H
#define AI_H

#include "game.h"

// if a node has never been visited, we want to ensure it gets picked at least once
#define INITIAL_UCB 99999999.

#define MAX_ITERATIONS 10

typedef struct Node {
  void *parent;
  int childCount;
  void **children;
  Board *board;
  Piece nextTurn;
  int movePos;    // -1 is reserved for the root note
  int visitCount;
  int winCount;
  double ucb;
} Node;

typedef struct Tree {
  Node *root;
  int iterCount;
  Piece player;     // the player we're evaluating
} Tree;

int next_move(Game *g);
int get_next_move(Game *g);

#endif /* AI_H */