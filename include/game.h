#ifndef GAME_H
#define GAME_H

typedef enum GameState {
  GS_INIT,
  GS_PLAYER_TURN,
  GS_CPU_TURN,
  GS_END_TIE,
  GS_END_X,
  GS_END_O
} GameState;

typedef enum UserAction {
  UA_CURSOR_UP,
  UA_CURSOR_DOWN,
  UA_CURSOR_LEFT,
  UA_CURSOR_RIGHT,
  UA_PLACE_PIECE,
  UA_QUIT,
  UA_NONE
} UserAction;

typedef enum CursorDirection {
  CUR_UP,
  CUR_DOWN,
  CUR_LEFT,
  CUR_RIGHT
} CursorDirection;

typedef struct Cursor {
  int row;
  int col;
} Cursor;

typedef enum BoardPlacementResult {
  BPR_OK,
  BPR_OCCUPIED,
  BPR_INVALID
} BoardPlacementResult;

typedef enum Piece {
  PIECE_X,
  PIECE_O,
  PIECE_EMPTY
} Piece;

typedef enum SquareColor {
  SQ_RED,
  SQ_GREEN,
  SQ_NONE
} SquareColor;

typedef enum Line {
  TOP_ROW,
  MIDDLE_ROW,
  BOTTOM_ROW,
  LEFT_COL,
  CENTER_COL,
  RIGHT_COL,
  BACKSLASH,
  FORWARD_SLASH,
  NO_WINNER
} Line;

typedef struct Square {
  int row;
  int col;
  Piece piece;
  SquareColor color;
} Square;

typedef struct Board {
  Square *squares[9];
} Board;

typedef struct Game {
  GameState state;
  Board *board;
  Cursor *cursor;
} Game;


Game *new_game();
void destroy_game(Game *g);

Board *new_board();
void destroy_board(Board *b);

BoardPlacementResult place_x(Board *b, int pos);
BoardPlacementResult place_o(Board *b, int pos);

int get_board_pos_from_cursor(Board *b, Cursor *c);

/* the main game loop */
void play(Game *g);

// set line colors
void set_top_row_color(Board *b, SquareColor c);
void set_middle_row_color(Board *b, SquareColor c);
void set_bottom_row_color(Board *b, SquareColor c);
void set_left_col_color(Board *b, SquareColor c);
void set_center_col_color(Board *b, SquareColor c);
void set_right_col_color(Board *b, SquareColor c);
void set_backslash_color(Board *b, SquareColor c);
void set_forwardslash_color(Board *b, SquareColor c);

#endif /* GAME_H */