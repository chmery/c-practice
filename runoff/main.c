#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// candidate with more than 50% wins
// if no one has more than 50%, eliminate worst
// if cannot eliminate worst due to tie, invalidate election

#define CANDIDATE_NAME_LENGTH 32

struct candidate {
  const char *name;
  int votes;
};

struct ballot {
  int curr_preference;
  int valid;
  struct candidate **candidates;
};

// UNION??
// struct ballots {
//   size_t count;
//   size_t item_size;
//   struct ballot *items;
// };

struct voting_results {
  struct candidate *worst_candidate;
  struct candidate *best_candidate;
};

struct array {
  size_t count;
  size_t item_size;
  void *items;
};

struct array new_array(size_t count, size_t item_size) {
  struct array a = {.count = count, .item_size = item_size, .items = calloc(count, item_size)};
  return a;
}

struct candidate *candidates_at(struct array *array, int i) {
  if (!i) {
    return (struct candidate *)array->items;
  }

  return (struct candidate *)array->items + i;
}

struct ballot *ballots_at(struct array *array, int i) {
  if (!i) {
    return (struct ballot *)array->items;
  }

  return (struct ballot *)array->items + i;
}

void init_candidates(struct array *candidates, char *argv[]) {
  for (unsigned i = 0; i < candidates->count; i++) {
    struct candidate *c = candidates_at(candidates, i);
    c->name = argv[i + 1];
    c->votes = 0;
  }
}

struct candidate *input_to_candidate(char *input, struct array *candidates) {
  for (unsigned i = 0; i < candidates->count; i++) {
    struct candidate *c = candidates_at(candidates, i);
    if (strcmp(input, c->name) == 0) {
      return c;
    }
  }

  return NULL;
}

int candidate_in_ballot(struct candidate *candidate, struct ballot *ballot, int candidates_count) {
  for (int i = 0; i < candidates_count; i++) {
    if (ballot->candidates[i] && ballot->candidates[i] == candidate) {
      return 1;
    }
  }
  return 0;
}

void free_ballots(struct array *ballots) {
  for (unsigned i = 0; i < ballots->count; i++) {
    struct ballot *b = ballots_at(ballots, i);
    free(b->candidates);
  }
  free(ballots->items);
}

void filter_out_invalid_ballots(struct array *ballots, int candidates_count, int invalid_count) {
  size_t valid_ballots_count = (sizeof(struct ballot) * (ballots->count - invalid_count)) / sizeof(struct ballot);
  struct ballot *valid_ballots = calloc(valid_ballots_count, sizeof(*valid_ballots));

  int skipped = 0;
  for (unsigned i = 0; i < ballots->count; i++) {
    struct ballot *ballot = ballots_at(ballots, i);
    if (ballot->valid) {
      struct ballot *vb = &valid_ballots[i - skipped];
      vb->valid = 1;
      vb->candidates = calloc(candidates_count, sizeof(struct candidate *));
      memcpy(vb->candidates, ballot->candidates, sizeof(struct candidate *) * candidates_count);
    } else {
      skipped++;
    }
  }

  free_ballots(ballots);
  ballots->count = valid_ballots_count;
  ballots->items = valid_ballots;
}

void init_ballots(struct array *ballots, struct array *candidates, int voters_count) {
  int invalid_count = 0;
  for (int i = 0; i < voters_count; i++) {
    printf("\n");

    struct ballot *ballot = ballots_at(ballots, i);
    ballot->candidates = calloc(candidates->count, sizeof(ballot->candidates));
    ballot->valid = 1;

    for (unsigned rank = 0; rank < candidates->count; rank++) {
      char name[CANDIDATE_NAME_LENGTH];
      printf("rank %i: ", rank + 1);
      scanf("%31s", name);

      struct candidate *candidate = input_to_candidate(name, candidates);
      int is_invalid = !candidate || candidate_in_ballot(candidate, ballot, candidates->count);

      if (is_invalid) {
        printf("invalid vote\n");
        invalid_count++;
        ballot->valid = 0;
        break;
      }

      ballot->candidates[rank] = candidate;
    }
  }

  filter_out_invalid_ballots(ballots, candidates->count, invalid_count);
}

int can_eliminate(struct array *candidates, struct candidate *worst_candidate) {
  int worst_candidates_count = 0;
  for (unsigned i = 0; i < candidates->count; i++) {
    struct candidate *candidate = candidates_at(candidates, i);
    if (candidate->votes == worst_candidate->votes) {
      worst_candidates_count++;
    }
  }
  return worst_candidates_count == 1;
}

void sum_votes(struct array *ballots) {
  for (unsigned i = 0; i < ballots->count; i++) {
    struct ballot *b = ballots_at(ballots, i);
    b->candidates[b->curr_preference]->votes++;
  }
}

void clear_votes(struct array *candidates) {
  for (unsigned i = 0; i < candidates->count; i++) {
    struct candidate *candidate = candidates_at(candidates, i);
    candidate->votes = 0;
  }
}

void eliminate_candidate(struct candidate *candidate, struct array *ballots) {
  printf("\neliminating: %s\n", candidate->name);

  for (unsigned i = 0; i < ballots->count; i++) {
    struct ballot *ballot = ballots_at(ballots, i);
    if (strcmp(candidate->name, ballot->candidates[ballot->curr_preference]->name) == 0) {
      ballot->curr_preference++;
    }
  }
}

struct voting_results get_voting_results(struct array *candidates) {
  struct voting_results results = {0};

  for (unsigned i = 0; i < candidates->count; i++) {
    struct candidate *candidate = candidates_at(candidates, i);
    if (!candidate->votes) {
      continue;
    }

    if (!results.worst_candidate && !results.best_candidate) {
      results.worst_candidate = candidate;
      results.best_candidate = candidate;
    }

    if (candidate->votes < results.worst_candidate->votes) {
      results.worst_candidate = candidate;
    } else if (candidate->votes > results.best_candidate->votes) {
      results.best_candidate = candidate;
    }
  }

  return results;
}

const char *find_winner(struct array *ballots, struct array *candidates, int candidates_count) {
  sum_votes(ballots);
  struct voting_results results = get_voting_results(candidates);

  int majority_achieved = (results.best_candidate->votes / (float)ballots->count) > 0.50;
  if (majority_achieved) {
    return results.best_candidate->name;
  }

  if (can_eliminate(candidates, results.worst_candidate)) {
    eliminate_candidate(results.worst_candidate, ballots);
    clear_votes(candidates);
    return find_winner(ballots, candidates, candidates_count);
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 10) {
    printf("input 2 names at least\n");
    return 1;
  }

  int voters_count = 0;
  printf("voters: ");
  scanf("%i", &voters_count);

  if (voters_count < 1) {
    printf("gib voters\n");
    return 1;
  }

  int candidates_count = argc - 1;
  struct array candidates = new_array(candidates_count, sizeof(struct candidate));
  struct array ballots = new_array(voters_count, sizeof(struct ballot));
  init_candidates(&candidates, argv);
  init_ballots(&ballots, &candidates, voters_count);

  if (!ballots.count) {
    printf("\nall votes invalid\n");
    return 1;
  }

  const char *winner = find_winner(&ballots, &candidates, candidates_count);
  if (winner) {
    printf("\nwinner: %s\n", winner);
  } else {
    printf("\nelection unresolved\n");
  }

  free(candidates.items);
  free_ballots(&ballots);

  return 0;
}