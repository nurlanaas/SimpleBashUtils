CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c11

CAT_C= $(wildcard *.c)
CAT_OBJ = $(patsubst %.c, %.o, $(CAT_C))

s21_cat: $(CAT_OBJ)
	$(CC) $(CFLAGS) $(CAT_OBJ) -o s21_cat 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o s21_cat 
