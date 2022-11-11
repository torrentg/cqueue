#include "cqueue.hpp"

#define INITIAL_SIZE 8
#define NUM_ITERATIONS 1000000
#define BATCH_SIZE 20

// g++ -std=c++20 -g -o cqueue-mem cqueue-mem.cpp
// valgrind --leak-check=full --show-leak-kinds=all ./cqueue-mem
// valgrind --tool=massif --time-unit=B --threshold=0.0 ./cqueue-mem
// ms_print --threshold=0.0 massif.out.1319585 > massif.out.cqueue

using namespace gto;

int main() {
  cqueue<int> queue;

  for (int i = 0; i < INITIAL_SIZE; i++) {
    queue.push(i);
  }

  for (int i = 0; i < NUM_ITERATIONS; i++) {
    for (int j = 0; j < BATCH_SIZE; j++) {
      queue.push(i);
    }
    for (int j = 0; j < BATCH_SIZE; j++) {
      queue.pop();
    }
  }
}