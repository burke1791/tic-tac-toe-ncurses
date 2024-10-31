#include <stdlib.h>

#include "ai.h"

#define MAX_ITERATIONS 100

Tree *new_tree();
Node *new_node(Node *parent, Board *src);
State *new_state(Board *src);

void destroy_tree(Tree *t);

void copy_board(Board *src, Board *tgt);

/**
 * @brief runs the AI logic and returns the position of the square
 *        where the next move should be made
 * 
 * @param g 
 * @return int 
 */
int next_move(Game *g) {

  for (int i = 0; i < 9; i++) {
    if (g->board->squares[i]->piece == PIECE_EMPTY) return i;
  }

  return -1;
}

Tree *new_tree(Game *g) {
  Tree *t = malloc(sizeof(Tree));

  Node *root = new_node(NULL, g->board);
}

Node *new_node(Node *parent, Board *src) {
  Node *n = malloc(sizeof(Node));
  n->parent = parent;
  n->state = new_state(src);
  n->children = NULL;

  return n;
}

static Piece get_next_turn(Board *b) {
  int numX = 0;
  int numO = 0;

  for (int i = 0; i < 9; i++) {
    if (b->squares[i]->piece == PIECE_X) numX++;
    if (b->squares[i]->piece == PIECE_O) numO++;
  }

  if (numO >= numX) {
    return PIECE_X;
  } else {
    return PIECE_O;
  }
}

State *new_state(Board *src) {
  State *s = malloc(sizeof(State));
  s->board = new_board();
  copy_board(src, s->board);

  s->nextTurn = get_next_turn(s->board);

  s->visitCount = 0;
  s->winScore = 0;

  return s;
}

void copy_board(Board *src, Board *tgt) {
  for (int i = 0; i < 9; i++) {
    tgt->squares[i]->piece = src->squares[i]->piece;
  }
}

void destory_tree(Tree *t) {
  
}