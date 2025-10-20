FLAGS = -Wall -Wextra -Werror -std=c11

all: clean s21_cat

s21_cat: s21_cat.c
	gcc $(FLAGS) s21_cat.c -o s21_cat
	
clang:
	clang-format -n *.c
	clang-format -i *.c

rebuild:
	make all

clean:
	rm -rf s21_cat
test:
	make all
	sh test_cat.sh
	
