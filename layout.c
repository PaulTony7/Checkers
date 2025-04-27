#include "clay.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <stdint.h>
#include <stdlib.h>

const int FONT_ID_BODY_16 = 0;
Clay_Color COLOR_WHITE = { 255, 255, 255, 255};

void RenderDropdownMenuItem(Clay_String text) {
  CLAY({.layout = { .padding = CLAY_PADDING_ALL(16)}}) {
    CLAY_TEXT(text, CLAY_TEXT_CONFIG({
      .fontId = FONT_ID_BODY_16,
      .fontSize = 16,
      .textColor = { 255, 255, 255, 255 }
    }));
  }
}

void HandleCellClick(
  Clay_ElementId elementId,
  Clay_PointerData pointerData,
  intptr_t userData
) {
  if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"cell pressed: %zd", userData);
  }
}

typedef struct {
  intptr_t offset;
  intptr_t memory;
} ClayVideoDemo_Arena;

typedef struct {
  int32_t selectedDocumentIndex;
  float yOffset;
  ClayVideoDemo_Arena frameArena;
} ClayVideoDemo_Data;

ClayVideoDemo_Data ClayVideoDemo_Initialize() {
  ClayVideoDemo_Data data = {
    .frameArena = { .memory = (intptr_t)malloc(1024) }
  };
  return data;
}

Clay_RenderCommandArray ClayVideoDemo_CreateLayout(ClayVideoDemo_Data *data) {
  data->frameArena.offset = 0;

  Clay_BeginLayout();

  Clay_Sizing layoutExpand = {
    .width = CLAY_SIZING_GROW(0),
    .height = CLAY_SIZING_GROW(0)
  };
  Clay_Sizing cellSize = { .width = 50, .height = 50};

  Clay_Color contentBackgroundColor = { 44, 44, 44, 255 };
  Clay_Color cellColorWhite = {240, 240, 240, 255};
  Clay_Color cellColorBlack = {10, 10, 10, 255};

  // Build UI here
  CLAY({ .id = CLAY_ID("OuterContainer"),
    .backgroundColor = contentBackgroundColor,
    .layout = {
      .layoutDirection = CLAY_TOP_TO_BOTTOM,
      .sizing = layoutExpand,
    }
  }) {
    for (int i = 0; i < 8; i++) {
      CLAY({
        .id = CLAY_IDI("ROW",i),
        .layout = {
          .layoutDirection = CLAY_LEFT_TO_RIGHT,
          .sizing = layoutExpand,
        }
      }) {
        for (int j = 0; j < 8; j++) {
          CLAY ({
              .id = CLAY_IDI("CELL",i*10+j),
              .backgroundColor = (i+j)%2 ? cellColorBlack : cellColorWhite,
              .layout = {
                .sizing = cellSize,
                .childAlignment = {
                  .x = CLAY_ALIGN_X_CENTER,
                  .y = CLAY_ALIGN_Y_CENTER
                }
              },
              })
            {
            Clay_OnHover(HandleCellClick, (intptr_t)(i*10+j));
            CLAY_TEXT(CLAY_STRING("W"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
            }));
          }
        }
      }
    }
  }

  Clay_RenderCommandArray renderCommands = Clay_EndLayout();
  for (int32_t i = 0; i < renderCommands.length; i++) {
    Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data->yOffset;
  }
  return renderCommands;
}
