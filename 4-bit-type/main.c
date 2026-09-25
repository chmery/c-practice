
#include <limits.h>
#include <stdio.h>
#include <sys/types.h>

u_int32_t to_two_complement(u_int32_t value) {
  return (value ^ UINT_MAX) + 1;
}

u_int32_t uint32_to_uint4(u_int32_t value) {
  return value % 16;
}

int32_t int32_to_int4(int32_t value) {
  int32_t last_4_bits = value & 15u;
  return last_4_bits > 7 ? last_4_bits - 16 : last_4_bits;
}

int main(void) {
  int32_t a = int32_to_int4(int32_to_int4(-1) + int32_to_int4(-8));
  int32_t b = int32_to_int4(7);
  int32_t c = int32_to_int4(14);

  printf("\n%i %i %i\n", a, b, c);
  return 0;
}