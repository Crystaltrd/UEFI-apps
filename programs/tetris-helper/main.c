#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BORDER_GLYPH "░"
#define FULL_GLYPH "█"
#define NUM_TETROMINOS 7

enum COLORS { NONE, CYAN, BLUE, ORANGE, YELLOW, GREEN, PURPLE, RED, BORDER };
struct Tetromino {
  enum COLORS color;
  uint16_t rotations[4];
};
static struct {
  uint8_t board[BOARD_HEIGHT][BOARD_WIDTH];
  struct {
    const struct Tetromino *ttm;
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
     .rotations = {0x0132, 0x0630, 0x2640, 0x0063}},
    {// J
     .color = BLUE,
     .rotations = {0x0223, 0x0170, 0x0622, 0x0074}},
    {// L
     .color = ORANGE,
     .rotations = {0x0322, 0x0470, 0x0226, 0x0071}}};
void print_board() {
  printf("    ");
  for (int x = 0; x < BOARD_WIDTH; x++) {
    printf("%X", x);
  }
  printf("\n");
  printf("   ");
  for (int x = 0; x < BOARD_WIDTH + 2; x++) {
    fputs(BORDER_GLYPH, stdout);
  }
  putchar('\n');
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    printf("%02X " BORDER_GLYPH, y);
    for (int x = 0; x < BOARD_WIDTH; x++) {
      fputs(state.board[y][x] != 0 ? FULL_GLYPH : " ", stdout);
    }
    puts(BORDER_GLYPH);
  }
  printf("   ");
  for (int x = 0; x < BOARD_WIDTH + 2; x++) {
    fputs(BORDER_GLYPH, stdout);
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
          printf("%s", (TETROMINOS[x].rotations[rot] & (1 << col * 4 + line))
                           ? FULL_GLYPH
                           : BORDER_GLYPH);
        }
        printf("\n");
      }
    }
  }
}
bool can_spawn(int x, int y, const struct Tetromino *ttm) {
  for (int line = 0; line < 4; line++)
    for (int col = 0; col < 4; col++) {
      if (state.board[y + line][x + col] &
          ((ttm->rotations[0] & (1 << col * 4 + line)) ? 1 : 0))
        return false;
    }
  return true;
}
bool spawn(int x, int y, const struct Tetromino *ttm) {
  if (!can_spawn(x, y, ttm)) {
    printf("Cant spawn !\n");
    return false;
  }
  state.curr.ttm = ttm;
  state.curr.pos_x = x;
  state.curr.pos_y = y;
  for (int line = 0; line < 4; line++)
    for (int col = 0; col < 4; col++) {
      state.board[y + line][x + col] |=
          (ttm->rotations[0] & (1 << col * 4 + line)) ? 1 : 0;
    }
  return true;
}
void update() {
  for (int line = 0; line < 4; line++)
    for (int col = 0; col < 4; col++) {
      if (state.curr.ttm->rotations[0] & (1 << col * 4 + line))
        state.board[state.curr.pos_y + line][state.curr.pos_x + col] &= 0;
    }
  spawn(state.curr.pos_x, state.curr.pos_y + 1, state.curr.ttm);
}
int main(void) {
  printf("Fat Teto\n");
  print_board();
  spawn(BOARD_WIDTH / 2, 0, &TETROMINOS[0]);
  print_board();
  while (1) {
    update();
    print_board();
    getchar();
  }
  ttm_test();
  return 0;
}
