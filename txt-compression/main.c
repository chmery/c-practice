#include "decoder.h"
#include "encoder.h"

int main(void) {
  // compress_ascii_text("raw.txt", "test");
  decode_ascii_text("test", "new.txt");
}