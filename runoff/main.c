#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// candidate with more than 50% wins
// if no one has more than 50%, eliminate worst
// if cannot eliminate worst due to tie, invalidate election

#define CANDIDATE_NAME_LENGTH 32

typedef struct {
  int curr_preference;
  int valid;
  char *candidates[];
} ballot;

typedef struct {
  const char *name;
  int votes;
} candidate;

// typedef struct {
//   size_t count;
//   size_t size;
//   void *items;
// } array;

// typedef struct {
//   char *candidates[9];
//   int count;
// } eliminated;

// add eliminated candidate name to array
// check each ballot's curr_preference if in eliminated array
// if yes, increment until not in array

// eliminated.candidates[eliminated.count] = candidate_name;
// eliminated.count++;

// what if curr_preference++ is enough? most likely it is but not 100% sure

// if all votes invalid segfault
// if no valid votes return

// void eliminate_candidate(const char *candidate_name, ballot ballots[], int ballot_count) {
//   printf("eliminating: %s\n", candidate_name);
//   for (int i = 0; i < ballot_count; i++) {
//     if (!ballots[i].valid) {
//       continue;
//     }

//     if (strcmp(candidate_name, ballots[i].candidates[ballots[i].curr_preference]) == 0) {
//       ballots[i].curr_preference++;
//     }
//   }
// };

void find_min_max_votes_candidate(candidate candidates[], int candidates_count, candidate *worst, candidate *best) {
  int min, max = candidates[0].votes;

  for (int i = 0; i < candidates_count; i++) {
    if (candidates[i].votes < min) {
      min = candidates[i].votes;
    } else if (candidates[i].votes > max) {
      max = candidates[i].votes;
    }
  }

  for (int i = 0; i < candidates_count; i++) {
    if (candidates[i].votes == min) {
      *worst = candidates[i];
    } else if (candidates[i].votes == max) {
      *best = candidates[i];
    }
  }
}

int can_eliminate(candidate candidates[], int candidates_count, int min_votes) {
  int min_votes_candidates_count = 0;
  for (int i = 0; i < candidates_count; i++) {
    if (candidates[i].votes == min_votes) {
      min_votes_candidates_count++;
    }
  }

  return min_votes_candidates_count == 1;
}

void add_vote_for_candidate(char candidate_name[], candidate candidates[], int candidates_count) {
  for (int i = 0; i < candidates_count; i++) {
    if (strcmp(candidate_name, candidates[i].name) == 0) {
      candidates[i].votes++;
      break;
    }
  }
}

// void sum_votes(ballot ballots[], candidate candidates[], int ballots_count, int candidates_count) {
//   for (int i = 0; i < ballots_count; i++) {
//     ballot b = ballots[i];
//     if (b.valid) {
//       add_vote_for_candidate(b.candidates[b.curr_preference], candidates, candidates_count);
//     }
//   }
// }

int is_valid_vote(char *candidate_name, int candidates_count, candidate candidates[], ballot *ballot) {
  int candidate_exists = 0;
  int is_duplicate = 0;

  for (int i = 0; i < candidates_count; i++) {
    if (strcmp(candidate_name, candidates[i].name) == 0) {
      candidate_exists = 1;
      break;
    }
  }

  for (int i = 0; i < candidates_count; i++) {
    if (!ballot->candidates[i]) {
      break;
    }

    if (strcmp(ballot->candidates[i], candidate_name) == 0) {
      is_duplicate = 1;
      break;
    }
  }

  return candidate_exists && !is_duplicate;
}

void clear_votes(candidate candidates[], int candidates_count) {
  for (int i = 0; i < candidates_count; i++) {
    candidates[i].votes = 0;
  }
}

void init_candidates(candidate candidates[], int candidates_count, char *argv[]) {
  for (int i = 0; i < candidates_count; i++) {
    candidates[i].name = argv[i + 1];
    candidates[i].votes = 0;
  }
}

ballot *init_ballots(candidate candidates[], int voters_count, int candidates_count) {
  size_t ballot_size = sizeof(ballot) + sizeof(char *) * candidates_count;
  size_t ballots_size = ballot_size * voters_count;
  ballot *ballots = malloc(ballots_size);

  int invalid_count = 0;
  for (int i = 0; i < voters_count; i++) {
    printf("\n");

    size_t step = ballot_size * i / sizeof(ballot);
    ballot *b = ballots + step;
    b->curr_preference = 0;
    b->valid = 1;

    for (int rank = 0; rank < candidates_count; rank++) {
      char name[CANDIDATE_NAME_LENGTH];
      printf("rank %i: ", rank + 1);
      scanf("%s", name);

      if (!is_valid_vote(name, candidates_count, candidates, b)) {
        printf("invalid vote\n");
        b->valid = 0;
        // TODO: experiment with this solution
        // set pointers addresses to 0
        // memset(b.candidates[0], 0, CANDIDATE_NAME_LENGTH * candidates_count);
        invalid_count++;
        break;
      }

      b->candidates[rank] = malloc(CANDIDATE_NAME_LENGTH);
      strcpy(b->candidates[rank], name);
    }
  }

  if (invalid_count == voters_count) {
    return NULL;
  }

  size_t valid_ballots_size = ballots_size - (ballot_size * invalid_count);
  ballot *valid_ballots = malloc(valid_ballots_size);

  int skipped = 0;
  for (int i = 0; i < voters_count; i++) {
    if (ballots[i].valid) {
      memcpy(&valid_ballots[i - skipped], &ballots[i], ballot_size);
    } else {
      skipped++;
    }
  }

  free(ballots);
  return valid_ballots;
}

// const char *find_winner(ballot ballots[], candidate candidates[], int ballots_count, int valid_ballots_count,
//                         int candidates_count) {
//   candidate worst_candidate;
//   candidate best_candidate;

//   sum_votes(ballots, candidates, ballots_count, candidates_count);

//   // FIX: it should look for rank1 candidates only
//   find_min_max_votes_candidate(candidates, candidates_count, &worst_candidate, &best_candidate);

//   int majority_achieved = (best_candidate.votes / (float)valid_ballots_count) > 0.50;
//   if (majority_achieved) {
//     return best_candidate.name;
//   }

//   if (can_eliminate(candidates, candidates_count, worst_candidate.votes)) {
//     eliminate_candidate(worst_candidate.name, ballots, ballots_count);
//     clear_votes(candidates, candidates_count);
//     return find_winner(ballots, candidates, ballots_count, valid_ballots_count, candidates_count);
//   }

//   return NULL;
// };

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 10) {
    printf("input 2 names at least\n");
    return 1;
  }

  // rename to voters_count, ballots_count will differ
  int voters_count = 0;
  printf("voters: ");
  scanf("%i", &voters_count);

  if (voters_count < 1) {
    printf("gib voters\n");
    return 1;
  }

  int candidates_count = argc - 1;
  candidate candidates[candidates_count];
  init_candidates(candidates, candidates_count, argv);
  // array ballots = {0};
  // (ballot)ballots.items;

  ballot *ballots = init_ballots(candidates, voters_count, candidates_count);

  if (!ballots) {
    printf("all votes invalid\n");
    return 1;
  }

  // ballots_count = ballots_size / ballot_size;

  // const char *winner = find_winner(ballots, candidates, ballots_count, candidates_count);
  // if (winner) {
  //   printf("winner: %s\n", winner);
  // } else {
  //   printf("election unresolved\n");
  // }

  // return 0;
}