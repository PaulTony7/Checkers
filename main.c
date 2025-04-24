// #define SDL_MAIN_USE_CALLBACKS
// #include <SDL3/SDL_main.h>
// #include <SDL3/SDL.h>
// #include <SDL3_ttf/SDL_ttf.h>
//
// #define CLAY_IMPLEMENTATION
// #include "clay.h"

// #include "darray.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>


enum Field {
  FIELD_NONE,
  FIELD_WHITE,
  FIELD_BLACK
};

typedef struct {
  enum Field fields[8][8]; 
} Board;

const int32_t ROWS = 8;
const int32_t COLS = 8;
Board board;

enum GameState state; 

enum GameState {
  STATE_PRE_GAME,
  STATE_WHITE_MOVE,
  STATE_BLACK_MOVE,
  STATE_POST_GAME
};

void pre_game() {
  // Set all the states before the game
  // This would be initialize the board
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      board.fields[i][j] = FIELD_NONE; 
    }
  }

  // Initialize black fields
  // 0 1 0 1 0 1 0 1
  // 1 0 1 0 1 0 1 0
  // 0 1 0 1 0 1 0 1
  // 0 0 0 0 0 0 0 0
  // 0 0 0 0 0 0 0 0
  // 1 0 1 0 1 0 1 0
  // 0 1 0 1 0 1 0 1
  // 1 0 1 0 1 0 1 0

  board.fields[0][1] = FIELD_BLACK;
  board.fields[0][3] = FIELD_BLACK;
  board.fields[0][5] = FIELD_BLACK;
  board.fields[0][7] = FIELD_BLACK;
  board.fields[1][0] = FIELD_BLACK;
  board.fields[1][2] = FIELD_BLACK;
  board.fields[1][4] = FIELD_BLACK;
  board.fields[1][6] = FIELD_BLACK;
  board.fields[2][1] = FIELD_BLACK;
  board.fields[2][3] = FIELD_BLACK;
  board.fields[2][5] = FIELD_BLACK;
  board.fields[2][7] = FIELD_BLACK;

  // Initialize White fields
  board.fields[5][0] = FIELD_WHITE;
  board.fields[5][2] = FIELD_WHITE;
  board.fields[5][4] = FIELD_WHITE;
  board.fields[5][6] = FIELD_WHITE;
  board.fields[6][1] = FIELD_WHITE;
  board.fields[6][3] = FIELD_WHITE;
  board.fields[6][5] = FIELD_WHITE;
  board.fields[6][7] = FIELD_WHITE;
  board.fields[7][0] = FIELD_WHITE;
  board.fields[7][2] = FIELD_WHITE;
  board.fields[7][4] = FIELD_WHITE;
  board.fields[7][6] = FIELD_WHITE;

  state = STATE_WHITE_MOVE;
}

void draw_board() {
  for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
        switch(board.fields[i][j]) {
          case FIELD_NONE:
            printf("#");
            break;
          case FIELD_WHITE:
            printf("W");
            break;
          case FIELD_BLACK:
            printf("B");
            break;
        }
      }
    printf("\n");
  }
}

void main_loop() {
  bool exit = false;
  while (!exit) {
    switch (state)
    {
      case STATE_PRE_GAME:
        pre_game();
        draw_board();
        break;
    }
  }
}

int main() {
  state = STATE_PRE_GAME;

  main_loop();
  // draw_board();    
  
  return 0;
}
