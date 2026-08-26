#include <stdio.h>

int main(void) {
  int numbers[10] = {1, 4, 5, 20, 50, 55, 56, 60, 61, 66};
  int target = 5;

  int *left = &numbers[0];
  int *right = &numbers[9];

  while (left < right) {
    int sum = *left + *right;

    if (sum == target) {
      printf("sum: %i %i\n", *left, *right);
      break;
    } else if (sum < target) {
      left++;
    } else {
      right--;
    }
  }
}