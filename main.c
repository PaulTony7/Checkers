#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "clay_renderer.c"
#include "layout.c"

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

static const Uint32 FONT_ID = 0;
SDL_Surface* sample_image;

typedef struct {
  SDL_Window* window;
  Clay_SDL3RendererData rendererData;
  ClayVideoDemo_Data demoData;
} AppState;

static inline Clay_Dimensions SDL_MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData) {
  TTF_Font** fonts = userData;
  TTF_Font* font = fonts[config->fontId];
  int width, height;

  if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s", SDL_GetError());
  }

  return (Clay_Dimensions) {(float) width, (float) height};
}

void HandleClayErrors(Clay_ErrorData errorData) {
  printf("%s", errorData.errorText.chars);
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
  (void) argc;
  (void) argv;

  if (!TTF_Init()) {
    return SDL_APP_FAILURE;
  }

  AppState* state = SDL_calloc(1, sizeof(AppState));
  if (!state) {
    return SDL_APP_FAILURE;
  }
  *appstate = state;

  if (!SDL_CreateWindowAndRenderer("demo", 640, 480, 0, &state->window, &state->rendererData.renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->rendererData.textEngine = TTF_CreateRendererTextEngine(state->rendererData.renderer);
  if (!state->rendererData.textEngine) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create text engine from renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->rendererData.fonts = SDL_calloc(1, sizeof(TTF_Font*));
  if (!state->rendererData.fonts) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for the font array: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  
  TTF_Font* font = TTF_OpenFont("Roboto-Regular.ttf", 24);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->rendererData.fonts[FONT_ID] = font;

  sample_image = IMG_Load("sample.png");

  uint64_t totalMemorySize = Clay_MinMemorySize();
  Clay_Arena clayMemory = (Clay_Arena) {
    .memory = SDL_malloc(totalMemorySize),
    .capacity = totalMemorySize
  };

  int width, height;
  SDL_GetWindowSize(state->window, &width, &height);
  Clay_Initialize(clayMemory, (Clay_Dimensions) {(float) width, (float) height}, (Clay_ErrorHandler) {HandleClayErrors});
  Clay_SetMeasureTextFunction(SDL_MeasureText, state->rendererData.fonts);

  state->demoData = ClayVideoDemo_Initialize();

  *appstate = state;
  return SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    SDL_AppResult ret_val = SDL_APP_CONTINUE;

    switch (event->type) {
        case SDL_EVENT_QUIT:
            ret_val = SDL_APP_SUCCESS;
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            Clay_SetLayoutDimensions((Clay_Dimensions) { (float) event->window.data1, (float) event->window.data2 });
            break;
        case SDL_EVENT_MOUSE_MOTION:
            Clay_SetPointerState((Clay_Vector2) { event->motion.x, event->motion.y },
                                 event->motion.state & SDL_BUTTON_LMASK);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            Clay_SetPointerState((Clay_Vector2) { event->button.x, event->button.y },
                                 event->button.button == SDL_BUTTON_LEFT);
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            Clay_UpdateScrollContainers(true, (Clay_Vector2) { event->wheel.x, event->wheel.y }, 0.01f);
            break;
        default:
            break;
    };

    return ret_val;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *state = appstate;

    Clay_RenderCommandArray render_commands = ClayVideoDemo_CreateLayout(&state->demoData);

    SDL_SetRenderDrawColor(state->rendererData.renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->rendererData.renderer);

    SDL_Clay_RenderClayCommands(&state->rendererData, &render_commands);

    SDL_RenderPresent(state->rendererData.renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    (void) result;

    if (result != SDL_APP_SUCCESS) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Application failed to run");
    }

    AppState *state = appstate;

    if (state) {
        if (state->rendererData.renderer)
            SDL_DestroyRenderer(state->rendererData.renderer);

        if (state->window)
            SDL_DestroyWindow(state->window);

        if (state->rendererData.fonts) {
            for(size_t i = 0; i < sizeof(state->rendererData.fonts) / sizeof(*state->rendererData.fonts); i++) {
                TTF_CloseFont(state->rendererData.fonts[i]);
            }

            SDL_free(state->rendererData.fonts);
        }

        if (state->rendererData.textEngine)
            TTF_DestroyRendererTextEngine(state->rendererData.textEngine);

        SDL_free(state);
    }
    TTF_Quit();
}

// int main() {
//   state = STATE_PRE_GAME;
//
//   main_loop();
//   // draw_board();    
//
//   return 0;
// }
