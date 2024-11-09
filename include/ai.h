#ifndef AI_H
#define AI_H

#include "game.h"

typedef struct State {
  Board *board;
  Piece nextTurn;
  int visitCount;
  double winScore;
} State;

typedef struct Node {
  void *parent;
  State *state;
  void **children;
} Node;

typedef struct Tree {
  Node *root;
} Tree;

int next_move(Game *g);

#endif /* AI_H */