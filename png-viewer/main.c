#include "decoder.h"
#include "utils.h"
#include <endian.h>
#include <raylib.h>
#include <stddef.h>
#include <sys/types.h>

// todo:
// read IDAT
// verify CRC

#define FILE_NAME "./resources/image.png"

int main(void) {
  struct ihdr_data *png_header = read_png_header(FILE_NAME);
  if (!png_header) {
    return 0;
  }

  const int screenWidth = png_header->width;
  const int screenHeight = png_header->height;

  InitWindow(screenWidth, screenHeight, "png viewer");
  SetTargetFPS(1);

  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText(TextFormat("size: %ipx x %ipx", png_header->width, png_header->height), 10, 10, 20, LIGHTGRAY);
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  CloseWindow();
  free(png_header);

  return 0;
}