

// // // парсим флаги! запоминаем шаблон в pattern = optarg
// // // все шаблоны добавляем в динамический массив
// // // нужно скомпилировать переданный шаблон в pcre_compile
// // // этот скомпилированный шаблон передаем в pcre_exec
// // // проходимся по всем строкам в файле и ищем шаблон, в зависимости от
// флагов выводим

#include "grep.h"

#include <getopt.h>
#include <regex.h>   // регулярные выражения
#include <stdio.h>   // FILE *
#include <stdlib.h>  // free()
#include <string.h>  // strlen()
#include <unistd.h>  // getopt()

#include "add_pattern.h"
#include "flags.h"

void file_patterns(char *optarg, char ***patterns, int *patterns_capacity,
                   int *num_patterns) {
  char pattern[4096];
  FILE *patterns_file = fopen(optarg, "r");
  while (fgets(pattern, 4096, patterns_file) != NULL) {
    if (pattern[strlen(pattern) - 1] == '\n')
      pattern[strlen(pattern) - 1] = '\0';
    add_pattern(pattern, patterns, patterns_capacity, num_patterns);
  }
  fclose(patterns_file);
}

Flags parser_flags(char ***patterns, int *num_patterns, int *error,
                   int *patterns_capacity, int argc, char *argv[]) {
  Flags flags = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int opt;

  while ((opt = getopt(argc, argv, "e:f:ivlcnhso")) != -1 && *error == 0) {
    if (opt == 'e') {
      flags.flag_e = 1;
      add_pattern(optarg, patterns, patterns_capacity, num_patterns);
    }
    if (opt == 'i') flags.flag_i = 1;
    if (opt == 'v') flags.flag_v = 1;
    if (opt == 'l') flags.flag_l = 1;
    if (opt == 'c') flags.flag_c = 1;
    if (opt == 'n') flags.flag_n = 1;
    if (opt == 'h') flags.flag_h = 1;
    if (opt == 's') flags.flag_s = 1;
    if (opt == 'o') flags.flag_o = 1;
    if (opt == 'f') {
      flags.flag_f = 1;
      file_patterns(optarg, patterns, patterns_capacity, num_patterns);
    }
    if (opt == '?') *error = 1;
  }
  return flags;
}

void regex_massive(char **patterns, int flag_i, int num_patterns, int error,
                   regex_t *regex) {
  int flag = 0;  // флаг (например, игнорирование регистра)
  if (flag_i == 1 && error == 0) {
    flag |= REG_ICASE;
  }
  if (error == 0) {
    for (int i = 0; i < num_patterns; i++) {
      regcomp(&regex[i], patterns[i],
              flag);  // regex - адрес первого шаблона. regex[i] - то же самое
                      // что *regex(0 + i), то есть адрес i-ого шаблона
    }
  }
}

void divide_line(regex_t *regex, char *line, int num_patterns, int flag_n,
                 int num_line, int printf_file, char *file_name) {
  regmatch_t match;
  int a[4096][2];
  int count = 0;
  for (int i = 0; i < num_patterns; i++) {
    int rm_eo = 0;
    while (regexec(&regex[i], line + rm_eo, 1, &match, 0) == 0) {
      a[count][0] = rm_eo + match.rm_so;
      a[count][1] = rm_eo + match.rm_eo - 1;
      rm_eo += match.rm_eo;
      count++;
    }
  }
  for (int i = 0; i < count; i++) {  // пузырьковая сортировка
    for (int j = 0; j < count - 1; j++) {
      if (a[j][0] > a[j + 1][0]) {
        int x = a[j][0];
        int y = a[j][1];

        a[j][0] = a[j + 1][0];
        a[j][1] = a[j + 1][1];

        a[j + 1][0] = x;
        a[j + 1][1] = y;
      }
    }
  }
  for (int i = 0; i < count; i++) {
    if (printf_file == 1) printf("%s:", file_name);
    if (flag_n == 1) printf("%d:", num_line);
    for (int j = a[i][0]; j <= a[i][1]; j++) {
      printf("%c", line[j]);
    }
    printf("\n");
  }
}

int erro(FILE *file, char *file_name, int flag_s) {
  int error = 0;
  if (file == NULL) {
    if (flag_s == 0) {
      printf("grep: %s: No such file or directory\n", file_name);
    }
  }
  return error;
}

void processing_line(Flags flags, int printf_file, int *num_line,
                     int num_patterns, regex_t *regex, char *line,
                     char *file_name, int *num_c, int *num_l) {
  int line_match = 0;
  int line_printf = 1;
  *num_line += 1;
  for (int j = 0; j < num_patterns; j++) {
    if (regexec(&regex[j], line, 0, NULL, 0) == 0) {
      line_match = 1;  // в строке есть рег
    }
  }

  if (flags.flag_v == 1) line_match = !line_match;  // произвели инверсию!
  if (flags.flag_c == 1 || flags.flag_l == 1)
    line_printf = 0;  // нужно печатать строку или нет

  if (line_printf == 1 && line_match == 1 && flags.flag_o == 0) {
    if (printf_file == 1) printf("%s:", file_name);
    if (flags.flag_n == 1) printf("%d:", *num_line);
    printf("%s", line);  // когда файл кончается, перевода на новую строку нет
    if (line[strlen(line) - 1] != '\n') printf("\n");

  } else if (flags.flag_o == 1 && line_printf == 1 && line_match == 1) {
    if (flags.flag_v == 1 && line_match == 1 && line_printf == 1) {
      if (printf_file == 1) printf("%s:", file_name);
      if (flags.flag_n == 1) printf("%d:", *num_line);
      printf("%s", line);
      if (line[strlen(line) - 1] != '\n') printf("\n");
    }
    if (flags.flag_v == 0) {
      divide_line(regex, line, num_patterns, flags.flag_n, *num_line,
                  printf_file, file_name);
    }
  }
  if (flags.flag_c == 1 && line_match == 1 && *num_l == 0) *num_c += 1;
  if (flags.flag_l == 1 && line_match == 1) *num_l = 1;
}
void processing_file(char *file_name, regex_t *regex, int printf_file,
                     Flags flags, int num_patterns) {
  FILE *file = fopen(file_name, "r");
  int error = erro(file, file_name, flags.flag_s);

  char line[1000];
  int num_line = 0;
  int num_c = 0;
  int num_l = 0;
  while (error == 0 && fgets(line, 1000, file) != NULL) {  // строки
    processing_line(flags, printf_file, &num_line, num_patterns, regex, line,
                    file_name, &num_c, &num_l);
  }
  if (flags.flag_c == 1) {
    if (printf_file == 1) printf("%s:", file_name);
    printf("%d\n", num_c);
  }
  if (flags.flag_l == 1 && num_l == 1) printf("%s\n", file_name);
  if (error == 0) fclose(file);
}

int main(int argc, char *argv[]) {
  char **patterns = NULL;
  int num_patterns = 0;
  int patterns_capacity = 1;
  int error = 0;
  char *files[argc];
  int file = 0;
  while (file + 1 < argc && argv[file + 1][0] != '-') {
    files[file] = argv[file + 1];
    file++;
  }
  optind = file + 1;
  // парсим флаги
  Flags flags = parser_flags(&patterns, &num_patterns, &error,
                             &patterns_capacity, argc, argv);
  int flag = 0;
  if (patterns == NULL && error == 0 && file == 0) {
    add_pattern(argv[optind], &patterns, &patterns_capacity, &num_patterns);
    optind++;
  } else if (patterns == NULL && error == 0 && file > 1) {
    add_pattern(files[0], &patterns, &patterns_capacity, &num_patterns);
    flag = 1;
  }
  // создаем массив регулярных выражений, компилируем шаблоны и заполняем массив
  regex_t regex[num_patterns];
  regex_massive(patterns, flags.flag_i, num_patterns, error, regex);
  // обрабатываем файлы
  int printf_file = 0;
  if (flag == 0 && argc - optind + file >= 2 && flags.flag_h == 0)
    printf_file = 1;
  if (flag == 1 && file - 1 >= 2) printf_file = 1;

  for (int i = flag; i < file; i++) {
    processing_file(files[i], regex, printf_file, flags, num_patterns);
  }
  for (int i = optind; i < argc; i++) {  // проходим по файлам
    if (flag == 0)
      processing_file(argv[i], regex, printf_file, flags, num_patterns);
  }
  // очищаем память
  for (int i = 0; i < num_patterns; i++) {
    free(patterns[i]);
  }
  free(patterns);
  for (int i = 0; i < num_patterns; i++) {
    regfree(&regex[i]);
  }
  return 0;
}
