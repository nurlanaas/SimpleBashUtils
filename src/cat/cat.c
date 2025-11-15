#include <stdio.h>

#include "flags.h"
#include "parser.h"
#include "processing_file.h"

int main(int argc, char* argv[]) {
  int error = 0;
  int file_position;
  Flags flags = parser(argc, argv, &error, &file_position);
  if (error == 1) {
    printf("Wrong option");
  }
  if (error == 0) {
    for (int i = file_position; i < argc; i++) {
      if (error == 0) processing_file(argv[i], &flags, &error);
    }
  }
}
