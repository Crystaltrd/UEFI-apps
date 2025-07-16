#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BOARD_X BOARD_WIDTH / 2 - 2
#define BOARD_Y 0
#define NUM_TETROMINOS 7
#define IN_BOARD(_x, _y)                                                       \
  ((_x) >= 0 && (_y) >= 0 && (_x) < BOARD_WIDTH && (_y) < BOARD_HEIGHT)
#define TTM_SIZE_Y(_t)                                                         \
  (0x8888 & _t)   ? 4                                                          \
  : (0x4444 & _t) ? 3                                                          \
  : (0x2222 & _t) ? 2                                                          \
  : (0x1111 & _t) ? 1                                                          \
                  : 0
#define TTM_SIZE_X(_t)                                                         \
  (0xF000 & _t)   ? 4                                                          \
  : (0x0F00 & _t) ? 3                                                          \
  : (0x00F0 & _t) ? 2                                                          \
  : (0x000F & _t) ? 1                                                          \
                  : 0
enum COLORS { NONE, CYAN, BLUE, ORANGE, YELLOW, GREEN, PURPLE, RED, BORDER };
static const char colors[BORDER + 1] = {" CBOYGPR|"};
struct Tetromino {
  enum COLORS color;
  uint16_t rotations[4];
};
static struct {
  uint8_t board[BOARD_HEIGHT][BOARD_WIDTH];
  struct {
    const struct Tetromino *ttm;
    uint8_t rot;
    uint8_t pos_x, pos_y;
  } curr;
} state;

static const struct Tetromino TETROMINOS[NUM_TETROMINOS] = {
    {// I
     .color = CYAN,
     .rotations = {0x2222, 0x000F, 0x4444, 0x000F}},
    {// O
     .color = YELLOW,
     .rotations = {0x0033, 0x0033, 0x0033, 0x0033}},
    {// T
     .color = PURPLE,
     .rotations = {0x0232, 0x0027, 0x0262, 0x0072}},
    {// Z
     .color = RED,
     .rotations = {0x0231, 0x0036, 0x0462, 0x0036}},
    {// S
     .color = GREEN,
     .rotations = {0x0132, 0x0063, 0x0264, 0x0063}},
    {// J
     .color = BLUE,
     .rotations = {0x0223, 0x0017, 0x0622, 0x0074}},
    {// L
     .color = ORANGE,
     .rotations = {0x0322, 0x0047, 0x0226, 0x0071}}};
void print_board() {
  printf("    ");
  for (int x = 0; x < BOARD_WIDTH; x++) {
    printf("%X ", x);
  }
  printf("\n");
  printf("    ");
  for (int x = 0; x < BOARD_WIDTH; x++) {
    fputc(colors[BORDER], stdout);
    putchar(' ');
  }
  putchar('\n');
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    printf("%02X %c", y, colors[BORDER]);
    for (int x = 0; x < BOARD_WIDTH; x++) {
      fputc(colors[state.board[y][x]], stdout);
      putchar(' ');
    }
    putchar(colors[BORDER]);
    putchar('\n');
  }
  printf("    ");
  for (int x = 0; x < BOARD_WIDTH; x++) {
    fputc(colors[BORDER], stdout);
    putchar(' ');
  }
  putchar('\n');
}
void ttm_test() {
  for (int x = 0; x < NUM_TETROMINOS; x++) {
    printf("Tetromino N°%d:\n", x);
    for (int rot = 0; rot < 4; rot++) {
      printf("Rotation %d:\n", rot);
      int offset_y = TTM_SIZE_Y(TETROMINOS[x].rotations[rot]);
      int offset_x = TTM_SIZE_X(TETROMINOS[x].rotations[rot]);
      for (int line = 0; line < offset_y; line++) {
        for (int col = 0; col < offset_x; col++) {
          printf("%c", (TETROMINOS[x].rotations[rot] & (1 << col * 4 + line))
                           ? colors[TETROMINOS[x].color]
                           : colors[BORDER]);
        }
        printf("\n");
      }
    }
  }
}
bool can_modify(uint8_t xc, uint8_t yc, uint8_t rc, uint8_t xn, uint8_t yn,
                uint8_t rn) {
  if (state.curr.ttm == NULL)
    return false;
  uint8_t board[BOARD_HEIGHT][BOARD_WIDTH];
  memcpy(&board, &state.board, sizeof(board));
  int offset_y = TTM_SIZE_Y(state.curr.ttm->rotations[rc]);
  int offset_x = TTM_SIZE_X(state.curr.ttm->rotations[rc]);

  for (int line = 0; line < offset_y; line++)
    for (int col = 0; col < offset_x; col++) {
      if (state.curr.ttm->rotations[rc] & (1 << col * 4 + line))
        board[yc + line][xc + col] &= 0;
    }
  offset_y = TTM_SIZE_Y(state.curr.ttm->rotations[rn]);
  offset_x = TTM_SIZE_X(state.curr.ttm->rotations[rn]);
  for (int line = 0; line < offset_y; line++)
    for (int col = 0; col < offset_x; col++) {
      if (!IN_BOARD(xn + col, yn + line) ||
          (board[yn + line][xn + col] &&
           ((state.curr.ttm->rotations[rn] & (1 << col * 4 + line))
                ? state.curr.ttm->color
                : NONE)))
        return false;
    }
  return true;
}
bool can_spawn(const struct Tetromino *ttm) {
  if (state.curr.ttm != NULL)
    return false;

  uint8_t board[BOARD_HEIGHT][BOARD_WIDTH];
  memcpy(&board, &state.board, sizeof(board));
  int offset_y = TTM_SIZE_Y(ttm->rotations[0]);
  int offset_x = TTM_SIZE_X(ttm->rotations[0]);
  for (int line = 0; line < offset_y; line++)
    for (int col = 0; col < offset_x; col++) {
      if (!IN_BOARD(BOARD_X + col, BOARD_Y + line) ||
          (board[BOARD_Y + line][BOARD_X + col] &&
           ((ttm->rotations[0] & (1 << col * 4 + line)) ? ttm->color : NONE)))
        return false;
    }
  return true;
}
bool spawn(const struct Tetromino *ttm) {
  if (!can_spawn(ttm))
    return false;
  state.curr.ttm = ttm;
  state.curr.pos_x = BOARD_X;
  state.curr.pos_y = BOARD_Y;
  state.curr.rot = 0;
  int offset_y = TTM_SIZE_Y(ttm->rotations[state.curr.rot]);
  int offset_x = TTM_SIZE_X(ttm->rotations[state.curr.rot]);
  for (int line = 0; line < offset_y; line++)
    for (int col = 0; col < offset_x; col++) {
      state.board[line][state.curr.pos_x + col] |=
          (ttm->rotations[state.curr.rot] & (1 << col * 4 + line)) ? ttm->color
                                                                   : NONE;
    }
  return true;
}
bool modify(uint8_t xc, uint8_t yc, uint8_t rc, uint8_t xn, uint8_t yn,
            uint8_t rn) {
  if (!can_modify(xc, yc, rc, xn, yn, rc)) {
    return false;
  }
  int offset_y = TTM_SIZE_Y(state.curr.ttm->rotations[rc]);
  int offset_x = TTM_SIZE_X(state.curr.ttm->rotations[rc]);
  for (int line = 0; line < offset_y; line++)
    for (int col = 0; col < offset_x; col++) {
      if (state.curr.ttm->rotations[rc] & (1 << col * 4 + line))
        state.board[yc + line][xc + col] &= 0;
    }
  state.curr.pos_x = xn;
  state.curr.pos_y = yn;
  state.curr.rot = rn;
  offset_y = TTM_SIZE_Y(state.curr.ttm->rotations[rn]);
  offset_x = TTM_SIZE_X(state.curr.ttm->rotations[rn]);
  for (int line = 0; line < offset_y; line++)
    for (int col = 0; col < offset_x; col++) {
      state.board[yn + line][xn + col] |=
          (state.curr.ttm->rotations[rn] & (1 << col * 4 + line))
              ? state.curr.ttm->color
              : NONE;
    }
  return true;
}
int main(void) {
  printf("Fat Teto\n");
  state.curr.ttm = NULL;
  state.curr.pos_x = 0;
  state.curr.pos_y = 0;
  state.curr.rot = 0;
  print_board();
  while (1) {
    if (!spawn(&TETROMINOS[rand() % NUM_TETROMINOS]) && state.curr.ttm == NULL)
      break;
    print_board();
    if (!modify(state.curr.pos_x, state.curr.pos_y, state.curr.rot,
                state.curr.pos_x, state.curr.pos_y + 1, state.curr.rot)) {
      state.curr.ttm = NULL;
      state.curr.pos_x = 0;
      state.curr.pos_y = 0;
      state.curr.rot = 0;
    }
    switch (getchar()) {
    case ' ':
      if (!modify(state.curr.pos_x, state.curr.pos_y, state.curr.rot,
                  state.curr.pos_x, state.curr.pos_y,
                  (state.curr.rot + 1) % 4)) {
        state.curr.ttm = NULL;
        state.curr.pos_x = 0;
        state.curr.pos_y = 0;
        state.curr.rot = 0;
      }
      break;
    default:
      break;
    }
  }
  return 0;
}
