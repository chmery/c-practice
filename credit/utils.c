int char_to_int(char c) { return c - '0'; };

int str_len(char *str) {
  int len = 0;

  while (str[len] != '\0') {
    len++;
  };

  return len;
}

void str_cpy(char *a, char *b) {
  int a_len = str_len(a);
  for (int i = 0; i < a_len; i++) {
    b[i] = a[i];
  }
  b[a_len] = '\0';
}