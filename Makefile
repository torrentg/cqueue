CXXFLAGS= -std=c++20 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Weffc++

all: example tests coverage mem

mem: cqueue-mem.cpp deque-mem.cpp
	g++ -std=c++20 -g -o deque-mem deque-mem.cpp
	g++ -std=c++20 -g -o cqueue-mem cqueue-mem.cpp

example: cqueue-example.cpp
	g++ -O2 $(CXXFLAGS) -o cqueue-example cqueue-example.cpp
	./cqueue-example

tests: cqueue-tests.cpp
	g++ -g $(CXXFLAGS) -o cqueue-tests cqueue-tests.cpp
	./cqueue-tests

coverage: cqueue-tests.cpp
	g++ --coverage -O0 $(CXXFLAGS) -o cqueue-coverage cqueue-tests.cpp -lgcov
	./cqueue-coverage
	mkdir coverage
	lcov --no-external -d . -o coverage/coverage.info -c
	lcov --remove coverage/coverage.info '*/catch.hpp' -o coverage/coverage.info
	lcov --remove coverage/coverage.info '*/cqueue-tests.cpp' -o coverage/coverage.info
	genhtml -o coverage coverage/coverage.info

clean: 
	rm -f cqueue-tests
	rm -f cqueue-coverage
	rm -f cqueue-example
	rm -f cqueue-mem 
	rm -f deque-mem
	rm -f *.gcda *.gcno
	rm -rf coverage
	rm -f *gmon*
	rm -f massif*
