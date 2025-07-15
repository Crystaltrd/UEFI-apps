#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BORDER_GLYPH "░"
#define FULL_GLYPH "█"
#define NUM_TETROMINOS 7
static struct {
  uint8_t board[BOARD_HEIGHT][BOARD_WIDTH];
} state;
enum COLORS { NONE, CYAN, BLUE, ORANGE, YELLOW, GREEN, PURPLE, RED, BORDER };
struct Tetromino {
  enum COLORS color;
  uint16_t rotations[4];
};
static const struct Tetromino TETROMINOS[NUM_TETROMINOS] = {
    {// I
     .color = CYAN,
     .rotations = {0x2222, 0x00F0, 0x4444, 0x0F00}},
    {// O
     .color = YELLOW,
     .rotations = {0x0330, 0x0330, 0x0330, 0x0330}},
    {// T
     .color = PURPLE,
     .rotations = {0x2320, 0x0720, 0x2620, 0x2700}},
    {// S
     .color = GREEN,
     .rotations = {0x2310, 0x0360, 0x4620, 0x3600}},
    {// Z
     .color = RED,
     .rotations = {0x1320, 0x0630, 0x2640, 0x6300}},
    {// J
     .color = BLUE,
     .rotations = {0x3220, 0x0710, 0x2260, 0x4700}},
    {// L
     .color = ORANGE,
     .rotations = {0x2230, 0x0740, 0x6220, 0x1700}}};
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
int main(void) {
  printf("Fat Teto\n");
  print_board();
  return 0;
}
