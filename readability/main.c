#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

int count_sentences(int length, char text[]) {
  int sentences = 0;
  for (int i = 0; i < length; i++) {
    if (text[i] == '.' || text[i] == '!' || text[i] == '?') {
      sentences++;
    }
  }
  return sentences;
}

int count_words(int length, char text[]) {
  int words = 0;
  for (int i = 0; i < length; i++) {
    if (text[i] == ' ') {
      words++;
    }
  }
  return ++words; // include last word
}

int count_letters(int length, char text[]) {
  int letters = 0;
  for (int i = 0; i < length; i++) {
    if (isalpha(text[i])) {
      letters++;
    }
  }
  return letters;
}

int calc_index(char text[]) {
  int text_length = strlen(text) - 1; // exclude newline character at the end
  int letters = count_letters(text_length, text);
  int sentences = count_sentences(text_length, text);
  int words = count_words(text_length, text);
  float l = ((float)letters / words) * 100;
  float s = ((float)sentences / words) * 100;
  return roundf(0.0588 * l - 0.296 * s - 15.8);
}

const int MAX_TEXT_LEN = 1024;

int main(void) {
  char text[MAX_TEXT_LEN];
  printf("gib text: ");
  fgets(text, MAX_TEXT_LEN, stdin);

  int index = calc_index(text);
  if (index < 1) {
    printf("before grade 1\n");
  } else if (index < 16) {
    printf("grade %i\n", index);
  } else {
    printf("grade 16+\n");
  }
}