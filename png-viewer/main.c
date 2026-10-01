#include "decoder.h"
#include "utils.h"
#include <endian.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>

// todo:
// read IDAT
// verify CRC
// robim to w raylibie

#define FILE_NAME "./resources/image.png"

int main(void) {
  struct ihdr_data *png_header = read_png_header(FILE_NAME);

  printf("%ipx x %ipx\ndepth: %i\ncolor: %i\n", png_header->width, png_header->height, png_header->bit_depth,
         png_header->color_type);

  free(png_header);

  return 0;
}