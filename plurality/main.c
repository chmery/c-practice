
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char name[16];
  int votes;
} candidate;

int validate_command(int argc) {
  if (argc > 10) {
    printf("max candites number is 9");
    return 1;
  }

  if (argc == 1) {
    printf("usage: ./main <name1 name2 ... name9>\n");
    return 1;
  }

  if (argc == 2) {
    printf("enter at least 2 names\n");
    return 1;
  }

  return 0;
}

long get_voters_count() {
  char input[3];
  printf("voters (2-99): ");
  fgets(input, 3, stdin);
  return strtol(input, NULL, 10);
}

void init_candidates(candidate candidates[], int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    strcpy(candidates[i - 1].name, argv[i]);
    candidates[i - 1].votes = 0;
  }
}

candidate *find_candidate(candidate candidates[], size_t candidates_size, char name[]) {
  for (int i = 0; i < candidates_size; i++) {
    if (strcmp(candidates[i].name, name) == 0) {
      return &candidates[i];
    }
  }
  return NULL;
}

void vote(candidate candidates[], size_t candidates_size) {
  char name[16];
  printf("vote for: ");
  fgets(name, 16, stdin);
  name[strcspn(name, "\n")] = 0;

  candidate *cp = find_candidate(candidates, candidates_size, name);
  if (cp) {
    cp->votes++;
  } else {
    printf("invalid vote :)\n");
  }
}

void find_winners(char *winners_arr[], int *winners_count, candidate candidates[], size_t candidates_size) {
  int max_votes = 0;

  for (int i = 0; i < candidates_size; i++) {
    int c_votes = candidates[i].votes;
    if (c_votes > max_votes) {
      max_votes = c_votes;
    }
  }

  if (!max_votes) {
    return;
  }

  for (int i = 0; i < candidates_size; i++) {
    if (candidates[i].votes == max_votes) {
      winners_arr[i] = candidates[i].name;
      (*winners_count)++;
    }
  }
}

int main(int argc, char *argv[]) {
  int err_code = validate_command(argc);
  if (err_code) {
    return err_code;
  }

  long voters_count = get_voters_count();
  if (voters_count < 2) {
    printf("enter correct voters number\n");
    return 1;
  }

  size_t candidates_size = argc - 1;
  candidate candidates[candidates_size];
  init_candidates(candidates, argc, argv);

  for (int i = 0; i < voters_count; i++) {
    vote(candidates, candidates_size);
  }

  char *winners[candidates_size];
  int winners_count = 0;
  find_winners(winners, &winners_count, candidates, candidates_size);

  if (winners_count) {
    for (int i = 0; i < winners_count; i++) {
      printf("%s\n", winners[i]);
    }
  } else {
    printf("no winners :(\n");
  }
}
