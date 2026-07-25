#include <ctype.h>
#include <stdio.h>
#include <string.h>

const int KEY_LENGTH = 26;
const int TEXT_LENGTH = 512;

void encode_str(char str[], char key[]) {
  int len = strlen(str);
  for (int i = 0; i < len; i++) {
    if (!isalpha(str[i])) {
      continue;
    }

    if (isupper(str[i])) {
      str[i] = toupper(key[str[i] - 'A']);
    } else {
      str[i] = tolower(key[str[i] - 'a']);
    }
  }
}

void decode_str(char str[], char key[]) {
  int len = strlen(str);
  for (int i = 0; i < len; i++) {
    if (!isalpha(str[i])) {
      continue;
    }

    char *cp = strchr(key, tolower(str[i]));
    int char_idx = cp - key;

    if (isupper(str[i])) {
      str[i] = char_idx + 'A';
    } else {
      str[i] = char_idx + 'a';
    }
  }
}

int count_occurences(char arr[], char val) {
  int len = strlen(arr);
  int count = 0;
  for (int i = 0; i < len; i++) {
    if (arr[i] == val) {
      count++;
    }
  }
  return count;
}

int is_valid_key(char key[]) {
  if (strlen(key) != KEY_LENGTH) {
    return 0;
  }

  for (int i = 0; i < KEY_LENGTH; i++) {
    if (!isalpha(key[i])) {
      return 0;
    }

    if (count_occurences(key, key[i]) != 1) {
      return 0;
    }
  }

  return 1;
}

int main(int argc, char *argv[]) {
  if (argc != 3 || (strcmp("-d", argv[2]) && strcmp("-e", argv[2]))) {
    printf("usage: ./main <key> <-e | -d>\n");
    return 1;
  }

  if (!is_valid_key(argv[1])) {
    printf("key must contain 26 unique alphabetic characters\n");
    return 1;
  }

  char text[TEXT_LENGTH];
  int encode = strcmp("-e", argv[2]) ? 0 : 1;

  printf("text to %s: ", encode ? "encode" : "decode");
  fgets(text, TEXT_LENGTH, stdin);
  text[strcspn(text, "\n")] = 0;

  if (encode) {
    encode_str(text, argv[1]);
  } else {
    decode_str(text, argv[1]);
  }

  printf("result: %s\n", text);
  return 0;
}