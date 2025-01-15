#include <chrono>
#include <deque>
#include <iostream>

#define INITIAL_SIZE 8
#define NUM_ITERATIONS 1000000
#define BATCH_SIZE 20

// g++ -std=c++20 -g -O3 -o deque-prof deque-prof.cpp
// valgrind --leak-check=full --show-leak-kinds=all ./deque-prof
// valgrind --tool=massif --time-unit=B --threshold=0.0 ./deque-prof
// ms_print --threshold=0.0 massif.out.1319585 > massif.out.deque

using namespace std;

int main() {
  deque<int> queue;

  for (int i = 0; i < INITIAL_SIZE; i++) {
    queue.push_back(i);
  }

  auto t1 = std::chrono::steady_clock::now();
  for (int i = 0; i < NUM_ITERATIONS; i++) {
    for (int j = 0; j < BATCH_SIZE; j++) {
      queue.push_back(i);
    }
    for (int j = 0; j < BATCH_SIZE; j++) {
      queue.pop_front();
    }
  }
  auto t2 = std::chrono::steady_clock::now();
  std::cout
      << "Elapsed time in microseconds : "
      << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
      << " µs\n";
}