CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c11

 GREP_C = grep.c add_pattern.c
 GREP_OBJ = $(patsubst %.c, %.o, $(GREP_C))


s21_grep: $(GREP_OBJ)
	$(CC) $(CFLAGS) $(GREP_OBJ) -o s21_grep

grep.o: grep.c
	$(CC) $(CFLAGS) -c $< -o $@

add_pattern.o: add_pattern.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o s21_grep