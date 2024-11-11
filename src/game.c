#include <stdlib.h>
#include <ncurses.h>

#include "game.h"
#include "display.h"
#include "ai.h"

Location *new_location(int row, int col) {
  Location *l = malloc(sizeof(Location));
  l->row = row;
  l->col = col;
  
  return l;
}

void destroy_location(Location *l) {
  free(l);
}

static Cursor *new_cursor() {
  Cursor *c = malloc(sizeof(Cursor));
  c->loc = new_location(BOARD_ORIGIN_ROW, BOARD_ORIGIN_COL + 1);
  c->ctx = CURCTX_BOARD;

  return c;
}

static void destroy_cursor(Cursor *c) {
  destroy_location(c->loc);
  free(c);
}

Game *new_game() {
  Game *g = malloc(sizeof(Game));
  g->state = GS_INIT;
  g->menu = new_menu();
  g->board = new_board();
  g->cursor = new_cursor();

  return g;
}

void destroy_game(Game *g) {
  destroy_menu(g->menu);
  destroy_board(g->board);
  destroy_cursor(g->cursor);
  free(g);
}

static UserAction get_menu_action_from_cursor(Menu *m, Cursor *c) {
  int menuPos = get_menu_pos_from_cursor(m, c);

  switch (m->items[menuPos]->menuAction) {
    case MENU_NEW_GAME:
      return UA_NEW_GAME;
    case MENU_QUIT:
      return UA_QUIT;
  }

  return UA_NONE;
}

/**
 * @brief The user selected the action button. This function determines
 * what action the user wishes to take
 * 
 * @param g 
 * @return UserAction 
 */
static UserAction determine_user_cursor_action(Game *g) {
  switch (g->cursor->ctx) {
    case CURCTX_BOARD:
      return UA_PLACE_PIECE;
    case CURCTX_MENU:
      return get_menu_action_from_cursor(g->menu, g->cursor);
  }

  return UA_NONE;
}

/**
 * @brief determines which action the user wants to take based on
 * the game's current state, the cursor context, and the user's input
 * 
 * @param g 
 * @param in 
 * @return UserAction 
 */
static UserAction get_user_action(Game *g, UserInput in) {
  switch (in) {
    case UI_NONE:
      return UA_NONE;
    case UI_Q:
      return UA_QUIT;
    case UI_UP:
      return UA_CURSOR_UP;
    case UI_DOWN:
      return UA_CURSOR_DOWN;
    case UI_LEFT:
      return UA_CURSOR_LEFT;
    case UI_RIGHT:
      return UA_CURSOR_RIGHT;
    case UI_ENTER:
      return determine_user_cursor_action(g);
  }

  return UA_NONE;
}

static UserInput parse_user_input(int c) {
  switch (c) {
    case 81:
    case 113:
      return UI_Q;
    case KEY_UP:
      return UI_UP;
    case KEY_DOWN:
      return UI_DOWN;
    case KEY_LEFT:
      return UI_LEFT;
    case KEY_RIGHT:
      return UI_RIGHT;
    case 32:
      return UI_ENTER;
    default:
      return UI_NONE;
  }
}

static void user_place_piece(Board *b, Cursor *c) {
  int pos = get_board_pos_from_cursor(b, c);
  place_piece(b, pos, PIECE_X);
}

static void move_cursor_to_menu(Cursor *c, Menu *m) {
  c->ctx = CURCTX_MENU;
  c->loc->row = m->items[0]->loc->row;
  // column doesn't matter in the menu
}

static void move_cursor_to_board(Cursor *c, Board* b) {
  c->ctx = CURCTX_BOARD;

  // move the cursor to the bottom left square
  c->loc->row = b->squares[6]->loc->row;
  c->loc->col = b->squares[6]->loc->col;
}

static void move_cursor_from_board(Game *g, CursorDirection d) {
  int pos = get_board_pos_from_cursor(g->board, g->cursor);
  int newPos;

  switch (d) {
    case CUR_UP:
      newPos = pos - 3;
      break;
    case CUR_DOWN:
      newPos = pos + 3;
      break;
    // allowing wraparound movement
    case CUR_LEFT:
      newPos = pos - 1;
      break;
    case CUR_RIGHT:
      newPos = pos + 1;
      break;
    default:
      newPos = pos;
  }

  if (newPos > 8) {
    move_cursor_to_menu(g->cursor, g->menu);
    return;
  } else if (newPos < 0 || newPos > 8) {
    newPos = pos;
  }

  g->cursor->loc->row = g->board->squares[newPos]->loc->row;
  g->cursor->loc->col = g->board->squares[newPos]->loc->col;
}

static void move_cursor_from_menu(Game *g, CursorDirection d) {
  int pos = get_menu_pos_from_cursor(g->menu, g->cursor);
  int newPos;

  switch (d) {
    case CUR_UP:
      newPos = pos - 1;
      break;
    case CUR_DOWN:
      newPos = pos + 1;
      break;
    default:
      newPos = pos;
  }

  if (newPos < 0) {
    move_cursor_to_board(g->cursor, g->board);
    return;
  } else if (newPos > 1) {
    newPos = 1;
  }

  g->cursor->loc->row = g->menu->items[newPos]->loc->row;
}

static void move_cursor(Game *g, CursorDirection d) {
  CursorCtx ctx = g->cursor->ctx;

  switch (ctx) {
    case CURCTX_BOARD:
      move_cursor_from_board(g, d);
      break;
    case CURCTX_MENU:
      move_cursor_from_menu(g, d);
      break;
  }
}

static Line get_winning_line(Board *b) {
  Piece winningPiece = PIECE_EMPTY;
  // top row
  if (b->squares[0]->piece == b->squares[1]->piece && b->squares[1]->piece == b->squares[2]->piece) return TOP_ROW;
  // middle row
  if (b->squares[3]->piece == b->squares[4]->piece && b->squares[4]->piece == b->squares[5]->piece) return MIDDLE_ROW;
  // bottom row
  if (b->squares[6]->piece == b->squares[7]->piece && b->squares[7]->piece == b->squares[8]->piece) return BOTTOM_ROW;

  // left col
  if (b->squares[0]->piece == b->squares[3]->piece && b->squares[3]->piece == b->squares[6]->piece) return LEFT_COL;
  // center col
  if (b->squares[1]->piece == b->squares[4]->piece && b->squares[4]->piece == b->squares[7]->piece) return CENTER_COL;
  // right col
  if (b->squares[2]->piece == b->squares[5]->piece && b->squares[5]->piece == b->squares[8]->piece) return RIGHT_COL;

  // back-slash diagonal
  if (b->squares[0]->piece == b->squares[4]->piece && b->squares[4]->piece == b->squares[8]->piece) return BACKSLASH;
  // forward-slash diagonal
  if (b->squares[6]->piece == b->squares[4]->piece && b->squares[4]->piece == b->squares[2]->piece) return FORWARD_SLASH;

  return NO_WINNER;
}

static Piece get_winning_piece(Board *b, Line wl) {
  Piece winner;

  switch (wl) {
    case MIDDLE_ROW:
    case CENTER_COL:
    case BACKSLASH:
    case FORWARD_SLASH:
      // these four lines go through the center square
      winner = b->squares[4]->piece;
      break;
    case TOP_ROW:
    case LEFT_COL:
      winner = b->squares[0]->piece;
      break;
    case BOTTOM_ROW:
    case RIGHT_COL:
      winner = b->squares[8]->piece;
      break;
    default:
      winner = PIECE_EMPTY;
  }

  return winner;
}

static SquareColor get_winning_line_color(Board *b, Line wl) {
  Piece winner = get_winning_piece(b, wl);
  SquareColor color = SQ_NONE;

  if (winner == PIECE_X) {
    color = SQ_GREEN;
  } else {
    color = SQ_RED;
  }

  return color;
}

static void set_line_color(Board *b, Line wl) {
  SquareColor c = get_winning_line_color(b, wl);

  switch (wl) {
    case TOP_ROW:
      set_top_row_color(b, c);
      break;
    case MIDDLE_ROW:
      set_middle_row_color(b, c);
      break;
    case BOTTOM_ROW:
      set_bottom_row_color(b, c);
      break;
    case LEFT_COL:
      set_left_col_color(b, c);
      break;
    case CENTER_COL:
      set_center_col_color(b, c);
      break;
    case RIGHT_COL:
      set_right_col_color(b, c);
      break;
    case BACKSLASH:
      set_backslash_color(b, c);
      break;
    case FORWARD_SLASH:
      set_forwardslash_color(b, c);
      break;
  }
}

/**
 * @brief checks if the board is full
 * 
 * @param b 
 * @return true 
 * @return false 
 */
static bool is_board_full(Board *b) {
  for (int i = 0; i < 9; i++) {
    if (b->squares[i]->piece == PIECE_EMPTY) return false;
  }

  return true;
}

static void set_game_state_turn(Game *g) {
  int numX = 0;
  int numO = 0;

  for (int i = 0; i < 9; i++) {
    if (g->board->squares[i]->piece == PIECE_X) numX++;
    if (g->board->squares[i]->piece == PIECE_O) numO++;
  }

  if (numO >= numX) {
    g->state = GS_PLAYER_TURN;
  } else {
    g->state = GS_CPU_TURN;
  }
}

/**
 * @brief checks the board for an ending condition and updates the
 *        game state accordingly
 * 
 * @param g 
 */
static void update_game_state(Game *g) {
  Line wl = get_winning_line(g->board);
  Piece winner = get_winning_piece(g->board, wl);

  switch (winner) {
    case PIECE_X:
      g->state = GS_END_X;
      set_line_color(g->board, wl);
      break;
    case PIECE_O:
      g->state = GS_END_O;
      set_line_color(g->board, wl);
      break;
    case PIECE_EMPTY:
      if (is_board_full(g->board)) {
        g->state = GS_END_TIE;
      } else {
        set_game_state_turn(g);
      }
      break;
  }
}

void play(Game *g) {
  update_game_state(g);
  refresh_display(g);

  int input;

  while (true) {
    input = getch();

    UserInput in = parse_user_input(input);
    switch (get_user_action(g, in)) {
      case UA_QUIT:
        return;
      case UA_NONE:
        continue;
      case UA_PLACE_PIECE:
        user_place_piece(g->board, g->cursor);
        break;
      case UA_NEW_GAME:
        reset_board(g->board);
        break;
      case UA_CURSOR_UP:
        move_cursor(g, CUR_UP);
        break;
      case UA_CURSOR_DOWN:
        move_cursor(g, CUR_DOWN);
        break;
      case UA_CURSOR_LEFT:
        move_cursor(g, CUR_LEFT);
        break;
      case UA_CURSOR_RIGHT:
        move_cursor(g, CUR_RIGHT);
        break;
    }

    update_game_state(g);
    refresh_display(g);

    // AI logic
    if (g->state == GS_CPU_TURN) {
      int cpuMove = next_move(g);
      place_piece(g->board, cpuMove, PIECE_O);

      update_game_state(g);
      refresh_display(g);
    }

    /*
      check for an ending condition
      set game state to GS_CPU_TURN if the player played a piece
      run CPU logic
      refresh display
      check for an ending condition
      repeat
    */
  }
}