#include "processing_file.h"

#include <stdio.h>
#include <string.h>

#include "flags.h"

void flag_v(int vflag, char *prev, int *ch, int *write) {
  if (vflag == 1) {
    if (*ch == '\n' || *ch == 9) {
      *write = 1;
    } else if (*ch > 127) {
      *ch -= 128;  // 0x80 - это 127. вычли его, теперь работаем как с обычным
      // символом только добавляем M-
      printf("M-");
    } else if (*ch >= 32 && *ch <= 126) {
      *write = 1;
    } else if (*ch == 127) {
      printf("^?");
      *prev = *ch;
      *write = 0;
    } else if (*ch < 32) {
      *ch += 64;
      printf("^%c", *ch);
      *prev = *ch;
      *write = 0;
    }
  }
}

void bflag(int bflag, char prev, int ch, int *num, int *bool_num, int eflag,
           int empty_line, int sflag) {
  if (bflag == 1) {
    if (prev == '\n' && ch != '\n') {  // тут только одна \n
      printf("%6d\t", *num);
      *num += 1;
      *bool_num = 1;
    }
    if (prev == '\n' && ch == '\n' && eflag == 1 &&
        (empty_line == 0 || sflag == 0)) {
      printf("      \t");
    }
  }
}

void flag_e(int eflag, int sflag, int *empty_line, int *write, int ch,
            char *prev) {
  if (eflag == 1) {  // если дошли до конца строки
    if (ch == '\n' && ((*empty_line == 1 && sflag == 1) != 1) && *write == 1) {
      printf("$\n");
      *write = 0;
      if (*prev == '\n' && ch == '\n') *empty_line = 1;
      *prev = ch;
    }
  }
}

void tflag(int tflag, int *ch, char *prev, int *write) {
  if (tflag == 1) {
    if (*ch == 9) {
      printf("^%c", 73);
      *prev = *ch;
      *write = 0;
    }
  }
}

void processing_file(const char *file_name, struct Flags *flags, int *error) {
  FILE *fb = fopen(file_name, "r");  // fb - переменная типа указатель на файл,
                                     // который мы открыли в режиме чтения
  if (fb == NULL) {
    *error = 1;
    printf("cat: %s: No such file or directory\n", file_name);
  }
  int ch;
  char prev = '\n';
  int write = 1;
  int num = 1;
  int bool_num = 0;  // написали номер строки
  int empty_line = 0;
  while (*error == 0 && (ch = fgetc(fb)) != EOF) {
    write = 1;
    bool_num = 0;
    if (ch != '\n') {
      empty_line = 0;
    }
    bflag(flags->bflag, prev, ch, &num, &bool_num, flags->eflag, empty_line,
          flags->sflag);
    if (flags->nflag == 1 && bool_num == 0 && flags->bflag == 0 &&
        (empty_line == 0 || flags->sflag == 0)) {
      if (prev == '\n') {  // тут точно подряд две \n\n
        printf("%6d\t", num);
        num++;
        bool_num = 1;
      }
    }
    flag_v(flags->vflag, &prev, &ch, &write);
    if (flags->sflag == 1) {  // сжатие строк
      if (prev == '\n' && empty_line == 1 &&
          ch == '\n') {  //       if (prev == '\n' && empty_line == 1) { //
        write = 0;       // если уже перевели строку +
      }
    }
    flag_e(flags->eflag, flags->sflag, &empty_line, &write, ch, &prev);
    tflag(flags->tflag, &ch, &prev, &write);
    if (write == 1) {
      if (prev == '\n' && ch == '\n') {
        empty_line = 1;
      }
      printf("%c", ch);
      prev = ch;
    }
  }
  if (error == 0) fclose(fb);
}