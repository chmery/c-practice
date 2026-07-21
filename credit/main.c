#include "utils.h"
#include <stdio.h>

void set_brand(char *cc_num, char *brand) {
  switch (cc_num[0]) {
  case '2':
  case '5':
    str_cpy("MASTERCARD", brand);
    break;
  case '3':
    str_cpy("AMEX", brand);
    break;
  case '4':
    str_cpy("VISA", brand);
  }
}

int validate_cc_num(char *num) {
  int num_len = str_len(num);
  int is_len_odd = num_len % 2 != 0 ? 1 : 0;

  int first_sum = 0;
  for (int i = is_len_odd; i < num_len; i += 2) {
    int result = char_to_int(num[i]) * 2;
    if (result >= 10) {
      first_sum += 1 + (result - 10);
    } else {
      first_sum += result;
    }
  }

  int second_sum = 0;
  for (int i = !is_len_odd; i < num_len; i += 2) {
    second_sum += char_to_int(num[i]);
  }

  return (first_sum + second_sum) % 10 == 0 ? 1 : 0;
}

int main(void) {
  char cc_num[20];
  printf("gib cc number:\n");
  scanf("%s", cc_num);

  int is_valid = validate_cc_num(cc_num);
  if (is_valid) {
    char brand[10 + 1];
    set_brand(cc_num, brand);
    printf("%s\n", brand);
  } else {
    printf("INVALID\n");
  }
}