#include "parser.h"

#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#include "flags.h"

Flags parser(int argc, char *argv[], int *error, int *file_position) {
  Flags flags = {0, 0, 0, 0, 0, 0};

  struct option long_flags[] = {{"number-nonblank", 0, NULL, 'b'},
                                {"number", 0, NULL, 'n'},
                                {"squeeze-blank", 0, NULL, 's'},
                                {0, 0, 0, 0}};

  int flag;
  while ((flag = getopt_long(argc, argv, "bevnstET", long_flags, NULL)) != -1) {
    if (flag == 'b') flags.bflag = 1;
    if (flag == 'n') flags.nflag = 1;
    if (flag == 'e') {
      flags.eflag = 1;
      flags.vflag = 1;
    }
    if (flag == 'v') flags.vflag = 1;
    if (flag == 's') flags.sflag = 1;
    if (flag == 't') {
      flags.tflag = 1;
      flags.vflag = 1;
    }
    if (flag == 'E') flags.eflag = 1;
    if (flag == 'T') flags.tflag = 1;
    if (flag == '?') {
      *error = 1;
    }
  }
  *file_position = optind;
  return flags;
}