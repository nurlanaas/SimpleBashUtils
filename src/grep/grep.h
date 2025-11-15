
#ifndef GREP
#define GREP

#include <regex.h>

#include "flags.h"

void file_patterns(char *optarg, char ***patterns, int *patterns_capacity,
                   int *num_patterns);
Flags parser_flags(char ***patterns, int *num_patterns, int *error,
                   int *patterns_capacity, int argc, char *argv[]);
void regex_massive(char **patterns, int flag_i, int num_patterns, int error,
                   regex_t *regex);
void divide_line(regex_t *regex, char *line, int num_patterns, int flag_n,
                 int num_line, int printf_file, char *file_name);
void processing_file(char *file_name, regex_t *regex, int printf_file,
                     Flags flags, int num_patterns);

#endif