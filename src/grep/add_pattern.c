

#include <stdlib.h>
#include <string.h>

void add_pattern(char *pattern, char ***patterns, int *patterns_capacity,
                 int *num_patterns) {
  if (*patterns == NULL) *patterns = malloc(1 * sizeof(char *));
  if (*num_patterns >= *patterns_capacity) {
    *patterns_capacity *= 2;
    *patterns = realloc(*patterns, *patterns_capacity * sizeof(char *));
  }
  (*patterns)[*num_patterns] = malloc(strlen(pattern) + 1);
  memcpy((*patterns)[*num_patterns], pattern, strlen(pattern) + 1);
  *num_patterns += 1;
}