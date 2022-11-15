CXXFLAGS= -std=c++20 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Weffc++

all: example tests coverage profiler

profiler: cqueue-prof.cpp deque-prof.cpp
	$(CXX) -std=c++20 -pg -g -o deque-prof deque-prof.cpp
	$(CXX) -std=c++20 -pg -g -o cqueue-prof cqueue-prof.cpp
	./cqueue-prof && gprof cqueue-prof gmon.out > cqueue-prof.gmon

example: cqueue-example.cpp
	$(CXX) -O2 $(CXXFLAGS) -o cqueue-example cqueue-example.cpp
	./cqueue-example

tests: cqueue-tests.cpp
	$(CXX) -g $(CXXFLAGS) -o cqueue-tests cqueue-tests.cpp
	./cqueue-tests

coverage: cqueue-tests.cpp
	$(CXX) --coverage -O0 $(CXXFLAGS) -o cqueue-coverage cqueue-tests.cpp -lgcov
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
	rm -f cqueue-prof
	rm -f deque-prof
	rm -f *.gcda *.gcno
	rm -rf coverage
	rm -f gmon.out *.gmon
	rm -f massif*
