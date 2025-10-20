FLAGS = -Wall -Wextra -Werror -std=c11

all: clean s21_grep

s21_grep: s21_grep.c
	gcc $(FLAGS) s21_grep.c -o s21_grep
	
clang:
	clang-format -n *.c
	clang-format -i *.c

rebuild:
	make all

clean:
	rm -rf s21_grep
test:
	make all
	sh test_grep.sh
	
