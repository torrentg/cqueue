#include "cqueue.hpp"

#include <chrono>
#include <iostream>

#define INITIAL_SIZE 8
#define NUM_ITERATIONS 1000000
#define BATCH_SIZE 20

// g++ -std=c++20 -g -O3 -o cqueue-prof cqueue-prof.cpp
// valgrind --leak-check=full --show-leak-kinds=all ./cqueue-prof
// valgrind --tool=massif --time-unit=B --threshold=0.0 ./cqueue-prof
// ms_print --threshold=0.0 massif.out.1319585 > massif.out.cqueue

using namespace gto;

int main() {
  cqueue<int> queue;

  for (int i = 0; i < INITIAL_SIZE; i++) {
    queue.push(i);
  }

  auto t1 = std::chrono::steady_clock::now();
  for (int i = 0; i < NUM_ITERATIONS; i++) {
    for (int j = 0; j < BATCH_SIZE; j++) {
      queue.push(i);
    }
    for (int j = 0; j < BATCH_SIZE; j++) {
      queue.pop();
    }
  }
  auto t2 = std::chrono::steady_clock::now();
  std::cout
      << "Elapsed time in microseconds : "
      << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
      << " µs\n";
}