#ifndef GAME_H
#define GAME_H

typedef enum GameState {
  GS_INIT,
  GS_PLAYER_TURN,
  GS_CPU_TURN,
  GS_END_TIE,
  GS_END_X,
  GS_END_O,
  GS_QUIT
} GameState;

typedef enum UserInput {
  UI_UP,
  UI_DOWN,
  UI_LEFT,
  UI_RIGHT,
  UI_ENTER,
  UI_Q,
  UI_NONE
} UserInput;

typedef enum UserAction {
  UA_CURSOR_UP,
  UA_CURSOR_DOWN,
  UA_CURSOR_LEFT,
  UA_CURSOR_RIGHT,
  UA_PLACE_PIECE,
  UA_NEW_GAME,
  UA_QUIT,
  UA_NONE
} UserAction;

typedef enum CursorDirection {
  CUR_UP,
  CUR_DOWN,
  CUR_LEFT,
  CUR_RIGHT
} CursorDirection;

typedef struct Location {
  int row;
  int col;
} Location;

typedef enum CursorCtx {
  CURCTX_BOARD,
  CURCTX_MENU
} CursorCtx;

typedef struct Cursor {
  CursorCtx ctx;
  Location *loc;
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
  Location *loc;
  Piece piece;
  SquareColor color;
} Square;

typedef struct Board {
  Square *squares[9];
} Board;

typedef enum MenuAction {
  MENU_NEW_GAME,
  MENU_QUIT
} MenuAction;

typedef struct MenuItem {
  MenuAction menuAction;
  Location *loc;
} MenuItem;

typedef struct Menu {
  MenuItem *items[2];
} Menu;

typedef struct Game {
  GameState state;
  Menu *menu;
  Board *board;
  Cursor *cursor;
} Game;

Location *new_location(int row, int col);
void destroy_location(Location *l);

Game *new_game();
void destroy_game(Game *g);

Board *new_board();
void destroy_board(Board *b);
void reset_board(Board *b);

Menu *new_menu();
void destroy_menu(Menu *m);
int get_menu_pos_from_cursor(Menu *m, Cursor *c);

BoardPlacementResult place_piece(Board *b, int pos, Piece p);

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