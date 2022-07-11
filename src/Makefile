WWW = -Wall -Wextra -Werror -std=c11
LINUX=-lsubunit -lrt -lpthread -lm
LIBS=-lcheck
LFLAGS=-fprofile-arcs -ftest-coverage
OS=$(shell uname -s)

all: clean test

re: clean all 

test: s21_decimal.a
ifeq ($(OS), Darwin)
	gcc $(WWW) test.c s21_decimal.a -o test $(LIBS)
else
	gcc $(WWW) test.c s21_decimal.a -o test $(LIBS) $(LINUX)
endif
	./test

gcov_report: clean_test
ifeq ($(OS), Darwin)
	gcc $(WWW) -fprofile-arcs -ftest-coverage test.c s21_*.c -o test $(LIBS)
else
	gcc $(WWW) -fprofile-arcs -ftest-coverage test.c s21_*.c -o test $(LIBS) $(LINUX)
endif
	./test
	lcov -t "test" -o test.info -c -d .
	genhtml -o report test.info

s21_decimal.a: build_s21_decimal 

build_s21_decimal:
	gcc -c s21_*.c
	ar rcs s21_decimal.a s21_*.o
	ranlib s21_decimal.a

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem *.c

install_brew:
	cd ~
	curl -fsSL https://rawgit.com/kube/42homebrew/master/install.sh | zsh
	brew install lcov

install_valgrind:
	brew install --HEAD LouisBrunner/valgrind/valgrind

install_cpch:
	brew install cppcheck

cpplint:
	cp ../materials/linters/CPPLINT.cfg .
	python3 ../materials/linters/cpplint.py --extensions=c *.h
	python3 ../materials/linters/cpplint.py --extensions=c *.c
	rm -rf CPPLINT.cfg

clean_obj:
	rm -rf *.o

clean_lib: 
	rm -rf *.a

clean_test:
	rm -rf *.gcda
	rm -rf *.gcno
	rm -rf *.info
	rm -rf test
	rm -rf report

clean: clean_lib clean_lib clean_test clean_obj
	rm -rf s21_test

print:
	gcc $(WWW) *.c 