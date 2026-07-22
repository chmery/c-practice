#include <ctype.h>
#include <stdio.h>
#include <string.h>

const int POINTS[] = {1, 3, 3, 2,  1, 4, 2, 4, 1, 8, 5, 1, 3,
                      1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10};
const int MAX_WORD_LENGTH = 15;

// int char_to_pts(char c) {
//   switch (tolower(c)) {
//   case 'a':
//   case 'e':
//   case 'i':
//   case 'l':
//   case 'n':
//   case 'o':
//   case 'r':
//   case 's':
//   case 't':
//   case 'u':
//     return 1;
//   case 'd':
//   case 'g':
//     return 2;
//   case 'b':
//   case 'c':
//   case 'm':
//   case 'p':
//     return 3;
//   case 'f':
//   case 'h':
//   case 'v':
//   case 'w':
//   case 'y':
//     return 4;
//   case 'k':
//     return 5;
//   case 'j':
//   case 'x':
//     return 8;
//   case 'q':
//   case 'z':
//     return 10;
//   default:
//     return 0;
//   }
// }

int sum_pts(int len, char word[]) {
  int sum = 0;
  for (int i = 0; i < len; i++) {
    if (!isalpha(word[i])) {
      continue;
    }
    int idx = tolower(word[i]) - 97;
    sum += POINTS[idx];
  }
  return sum;
}

void print_result(int p1_pts, int p2_pts) {
  if (p1_pts > p2_pts) {
    printf("p1 wins\n");
  } else if (p1_pts < p2_pts) {
    printf("p2 wins\n");
  } else {
    printf("tie\n");
  }
}

int main(void) {
  char p1_word[MAX_WORD_LENGTH + 1];
  char p2_word[MAX_WORD_LENGTH + 1];

  printf("p1 word: ");
  scanf("%s", p1_word);
  printf("p2 word: ");
  scanf("%s", p2_word);

  int p1_pts = sum_pts(strlen(p1_word), p1_word);
  int p2_pts = sum_pts(strlen(p2_word), p2_word);

  print_result(p1_pts, p2_pts);
}