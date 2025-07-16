#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define NUM_TETROMINOS 7
#define IN_BOARD(_x, _y)                                                       \
  ((_x) >= 0 && (_y) >= 0 && (_x) < BOARD_WIDTH && (_y) < BOARD_HEIGHT)
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
     .rotations = {0x2222, 0x0F00, 0x4444, 0x00F0}},
    {// O
     .color = YELLOW,
     .rotations = {0x0330, 0x0330, 0x0330, 0x0330}},
    {// T
     .color = PURPLE,
     .rotations = {0x0232, 0x0270, 0x0262, 0x0072}},
    {// Z
     .color = RED,
     .rotations = {0x0231, 0x0360, 0x0462, 0x0036}},
    {// S
     .color = GREEN,
     .rotations = {0x0132, 0x0630, 0x0264, 0x0063}},
    {// J
     .color = BLUE,
     .rotations = {0x0223, 0x0170, 0x0622, 0x0074}},
    {// L
     .color = ORANGE,
     .rotations = {0x0322, 0x0470, 0x0226, 0x0071}}};
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
      for (int line = 0; line < 4; line++) {
        for (int col = 0; col < 4; col++) {
          printf("%c", (TETROMINOS[x].rotations[rot] & (1 << col * 4 + line))
                           ? colors[TETROMINOS[x].color]
                           : colors[BORDER]);
        }
        printf("\n");
      }
    }
  }
}
bool can_modify(uint8_t x, uint8_t y, int cr, int nr,
                const struct Tetromino *ttm) {

  uint8_t board[BOARD_HEIGHT][BOARD_WIDTH];
  memcpy(&board, &state.board, sizeof(board));
  for (int line = 0; line < 4; line++)
    for (int col = 0; col < 4; col++) {
      if (ttm->rotations[cr] & (1 << col * 4 + line))
        board[y + line][x + col] &= 0;
    }
  printf("nr %d\n", nr);
  for (int line = 0; line < 4; line++)
    for (int col = 0; col < 4; col++) {
      if (!IN_BOARD(x + col, y + line) ||
          (board[y + line][x + col] &&
           ((ttm->rotations[nr] & (1 << col * 4 + line)) ? ttm->color : NONE)))
        return false;
    }
  return true;
}
bool spawn(const struct Tetromino *ttm) {
  if (!can_modify(BOARD_WIDTH / 2 - 2, 0, 0, 0, ttm)) {
    printf("Cant spawn !\n");
    return false;
  }
  state.curr.ttm = ttm;
  state.curr.pos_x = BOARD_WIDTH / 2 - 2;
  state.curr.pos_y = 0;
  for (int line = 0; line < 4; line++)
    for (int col = 0; col < 4; col++) {
      state.board[line][state.curr.pos_x + col] |=
          (ttm->rotations[0] & (1 << col * 4 + line)) ? ttm->color : NONE;
    }
  return true;
}
void rotate() {
  if (!can_modify(state.curr.pos_x, state.curr.pos_y, state.curr.rot,
                  (state.curr.rot + 1) % 4, state.curr.ttm)) {
    printf("Cant rotate !\n");
    return;
  }
  for (int line = 0; line < 4; line++)
    for (int col = 0; col < 4; col++) {
      if (state.curr.ttm->rotations[state.curr.rot] & (1 << col * 4 + line))
        state.board[state.curr.pos_y + line][state.curr.pos_x + col] &= 0;
    }
  state.curr.rot = (state.curr.rot + 1) % 4;
  for (int line = 0; line < 4; line++)
    for (int col = 0; col < 4; col++) {
      state.board[line][state.curr.pos_x + col] |=
          (state.curr.ttm->rotations[state.curr.rot] & (1 << col * 4 + line))
              ? state.curr.ttm->color
              : NONE;
    }
}
int main(void) {
  printf("Fat Teto\n");
  state.curr.ttm = NULL;
  state.curr.pos_x = 1;
  state.curr.pos_y = 1;
  state.curr.rot = 0;
  print_board();
  state.board[0][4] = 1;
  spawn(&TETROMINOS[0]);
  while (1) {
    print_board();
    switch (getchar()) {
    case 'r':
      rotate();
      break;
    default:
    }
  }
  printf("Cant spawn more stuff, gameover\n");
  ttm_test();
  return 0;
}
