#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "ai.h"
#include "display.h"

Tree *new_tree(Game *gs);
Node *new_node(Node *parent, Board *src);

void destroy_tree(Tree *t);

void copy_board(Board *src, Board *tgt);
Piece get_next_turn(Board *b);

static void print_node(Node *n, const char *indent);
static void print_tree(Tree *t);

int random_int(int lower, int upper) {
    // Initialize random seed
    srand(time(NULL));

    // Calculate range
    int range = upper - lower + 1;

    // Generate random integer
    int random_val = rand() % range;

    // Shift range to desired interval
    return lower + random_val;
}

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

/**
 * @brief The selection phase of MCTS
 * It traverses the tree by selecting child nodes with the highest UCB
 * values until it reaches a node without children and returns
 * 
 * @param n
 * @return Node* 
 */
static Node *select_node(Node *n) {
  if (n->childCount == 0) return n;

  // get child node with highest ucb
  Node *child;
  int ucb = 0;
  for (int i = 0; i < n->childCount; i++) {
    if (((Node*)n->children[i])->ucb > ucb) {
      child = (Node*)n->children[i];
      ucb = child->ucb;
    }
  }

  return select_node(child);
}

/**
 * @brief The expansion phase of MCTS
 * Adds child nodes for each possible move
 * 
 * @param n 
 */
static void expand_node(Node *n) {
  int childInd = 0;
  int numEmptySquares = num_empty_squares(n->board);
  n->childCount = numEmptySquares;
  n->children = calloc(n->childCount, sizeof(void*));

  for (int i = 0; i < 9; i++) {
    if (n->board->squares[i]->piece == PIECE_EMPTY) {
      Node *child = new_node(n, n->board);
      child->board->squares[i]->piece = n->nextTurn;
      child->movePos = i;
      child->nextTurn = get_next_turn(child->board);
      n->children[childInd] = (void*)child;
      childInd++;
    }
  }
}

/**
 * @brief heuristic function to help the simulate phase
 * Grabs an immediate win if available
 * Prevents an immediate loss if necessary
 * Returns a random move otherwise
 * 
 * @param n 
 * @return int 
 */
static int simulate_move(Board *b, Piece currentMove) {
  int numEmptySquares = num_empty_squares(b);
  if (numEmptySquares == 0) return -1;

  int winMove = -1;
  int noLoss = -1;
  Line wl;
  Piece winner;

  for (int i = 0; i < 9; i++) {
    if (b->squares[i]->piece == PIECE_EMPTY) {
      place_piece(b, i, PIECE_O);
      wl = get_winning_line(b);
      winner = get_winning_piece(b, wl);

      if (winner == currentMove) {
        winMove = i;
      } else if (winner != PIECE_EMPTY) {
        noLoss = i;
      }

      // reset square
      reset_square(b->squares[i]);
      
      place_piece(b, i, PIECE_X);
      wl = get_winning_line(b);
      winner = get_winning_piece(b, wl);

      if (winner == currentMove) {
        winMove = i;
      } else if (winner != PIECE_EMPTY) {
        noLoss = i;
      }

      // reset square
      reset_square(b->squares[i]);
    }

    if (winMove >= 0) return winMove;
  }

  if (noLoss >= 0) return noLoss;

  // return random move pos
  int moveNum = random_int(0, (numEmptySquares - 1));
  int sq = -1;
  for (int i = 0; i < 9; i++) {
    if (b->squares[i]->piece == PIECE_EMPTY) sq++;
    if (sq == moveNum) {
      return i;
    }
  }
}

/**
 * @brief The simulation phase of MCTS
 * Randomly plays the game starting from Node *n until an end condition
 * is reached. 
 * 
 * @param n 
 * @return Line 
 */
static Line simulate_game(Node *n) {
  // copy the board so we can play the game without messing up the tree
  Board *b = new_board();
  copy_board(n->board, b);

  Line wl = get_winning_line(b);
  Piece p = get_next_turn(b);

  while (wl == NO_WINNER) {
    int pos = simulate_move(b, p);
    if (pos < 0) break;

    if (place_piece(b, pos, p) == BPR_INVALID) {
      printf("pos: %d | piece: %c\n", pos, get_piece_char(p));
      print_board(b, "invalid placement");
      exit(EXIT_FAILURE);
    }

    wl = get_winning_line(b);
    p = get_next_turn(b);
  }

  destroy_board(b);

  return wl;
}

static double compute_ucb(Node *n) {
  if (n->parent == NULL) return 0; // ucb of the root node is irrelevant
  if (n->visitCount == 0) return INITIAL_UCB;
  Node *parent = n->parent;
  return ((double)n->winCount / (double)n->visitCount) + (1.41 * sqrt(log((double)parent->visitCount) / (double)n->visitCount));
}

static void backpropagate_node(Node *leaf, bool isWin) {
  Node *node = leaf;

  while (node != NULL) {
    node->visitCount++;
  
    if (isWin) {
      node->winCount++;
    }

    node->ucb = compute_ucb(node);
    node = node->parent;
  }
}

static int choose_best_move(Node *n) {
  double ucb = 0.;
  int bestMove = -1;

  for (int i = 0; i < n->childCount; i++) {
    Node *child = (Node*)n->children[i];
    if (child->ucb > ucb) {
      ucb = child->ucb;
      bestMove = child->movePos;
    }
  }

  return bestMove;
}

/**
 * @brief the main monte carlo tree search loop
 * 
 * @param t 
 * @return int 
 */
static int mcts(Tree *t) {

  while (t->iterCount <= MAX_ITERATIONS) {
    Node *n;
    
    n = select_node(t->root);

    Line wl = get_winning_line(n->board);

    if (wl == NO_WINNER) {
      expand_node(n);
      wl = simulate_game(n);
    }

    Piece winner = get_winning_piece(n->board, wl);
    
    if (winner == t->player) {
      backpropagate_node(n, true);
    } else {
      backpropagate_node(n, false);
    }
    
    t->iterCount++;
  }

  return choose_best_move(t->root);
}

int get_next_move(Game *g) {
  Tree *t = new_tree(g);

  // printf("turn: %c\n", get_piece_char(t->player));

  int pos = mcts(t);

  // print_tree(t);

  destroy_tree(t);

  return pos;
}

Tree *new_tree(Game *g) {
  Tree *t = malloc(sizeof(Tree));

  Node *root = new_node(NULL, g->board);
  t->root = root;

  t->iterCount = 0;
  
  if (g->state == GS_CPU_TURN) {
    t->player = PIECE_O;
  } else if (g->state == GS_PLAYER_TURN) {
    t->player = PIECE_X;
  }

  return t;
}

Node *new_node(Node *parent, Board *src) {
  Node *n = malloc(sizeof(Node));
  n->parent = parent;
  n->childCount = 0;
  n->children = NULL;
  n->board = new_board();
  copy_board(src, n->board);
  n->nextTurn = get_next_turn(n->board);
  n->movePos = -1;
  n->visitCount = 0;
  n->winCount = 0;
  n->ucb = INITIAL_UCB;

  return n;
}

static void destroy_node(Node *n) {
  for (int i = 0; i < n->childCount; i++) {
    destroy_node(n->children[i]);
  }

  destroy_board(n->board);
  free(n->children);
  free(n);
}

void destroy_tree(Tree *t) {
  destroy_node(t->root);
  free(t);
}

void copy_board(Board *src, Board *tgt) {
  for (int i = 0; i < 9; i++) {
    tgt->squares[i]->piece = src->squares[i]->piece;
  }
}

static void print_node(Node *n, const char *indent) {
  printf("%s pos:      %d\n", indent, n->movePos);
  printf("%s children: %d\n", indent, n->childCount);
  printf("%s ucb:      %lf\n", indent, n->ucb);
  printf("%s wins:     %d\n", indent, n->winCount);
  printf("%s visits:   %d\n", indent, n->visitCount);
}

static void print_tree(Tree *t) {
  printf("===== Tree =====\n");
  printf("== Turn: %c\n", get_piece_char(t->player));
  printf("== Root\n");
  printf("== children: %d\n", t->root->childCount);
  printf("----\n");

  for (int i = 0; i < t->root->childCount; i++) {
    print_node(t->root->children[i], "====");
    printf("----\n");
  }
}