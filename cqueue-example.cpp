#include <iostream>
#include <string>
#include <string_view>
#include <algorithm>
#include "cqueue.hpp"

// g++ -std=c++20 -o cqueue-example cqueue-example.cpp 

static constexpr std::string_view prefix = "  ";

template<typename T>
std::string to_string(const gto::cqueue<T> &queue) {
  std::string ret = "{";
  ret += "capacity=" + std::to_string(queue.capacity()) + ", ";
  ret += "reserved=" + std::to_string(queue.reserved()) + ", ";
  ret += "size=" + std::to_string(queue.size()) + ", ";
  ret += "content=[";
  for (std::size_t i = 0; i < queue.size(); i++) {
    ret += (i > 0 ? ", " : "");
    ret += std::to_string(queue[i]);
  }
  ret += "]}";
  return ret;
}

void example1() {
  std::cout << "example1()" << std::endl;
  gto::cqueue<int> queue;
  std::cout << prefix << ::to_string(queue) << std::endl;
  queue.push(1);
  queue.push(2);
  queue.push(3);
  std::cout << prefix << to_string(queue) << std::endl;
  queue[1] = 99;
  std::cout << prefix << to_string(queue) << std::endl;
  std::cout << prefix << "queue[1] = " << queue[1] << std::endl;
  queue.pop();
  std::cout << prefix << to_string(queue) << std::endl;
  std::cout << prefix << "front = " << queue.front() << std::endl;
  std::cout << prefix << "back = " << queue.back() << std::endl;
  std::cout << prefix << "empty = " << (queue.empty() ? "true" : "false") << std::endl;
  queue.clear();
  std::cout << prefix << to_string(queue) << std::endl;
  std::cout << prefix << "empty = " << (queue.empty() ? "true" : "false") << std::endl;
}

void example2() {
  std::cout << "example2()" << std::endl;
  gto::cqueue<int> queue(1'000'000);
  std::cout << prefix << to_string(queue) << std::endl;
  queue.push(1);
  queue.push(2);
  for (int i = 3; i < 13; i++) {
    queue.push(i);
  }
  std::cout << prefix << to_string(queue) << std::endl;
}

void example3() {
  std::cout << "example3()" << std::endl;
  gto::cqueue<int> queue;
  for (int i = 1; i <= 8; i++) {
    queue.push(i);
  }
  std::cout << prefix << to_string(queue) << std::endl;
  queue.pop();
  queue.pop();
  queue.push(9);
  queue.push(10);
  // memory content is [9, 10, 2, 3, 4, 5, 6, 7, 8]
  std::cout << prefix << to_string(queue) << std::endl;
  queue.push(11);
  // memory content is [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, ., ., ., ., ., .]
  std::cout << prefix << to_string(queue) << std::endl;
}

void example4() {
  std::cout << "example4()" << std::endl;
  gto::cqueue<int> queue;
  for (int i = 1; i <= 8; i++) {
    queue.push(i);
  }
  std::cout << prefix << to_string(queue) << std::endl;
  for (auto &item : queue) {
    item++;
  }
  std::cout << prefix << to_string(queue) << std::endl;
}

void example5() {
  std::cout << "example5()" << std::endl;
  gto::cqueue<int> queue;
  for (int i = 8; i >= 1; i--) {
    queue.push(i);
  }
  queue.pop();
  queue.push(99);
  std::cout << prefix << to_string(queue) << std::endl;
  std::sort(queue.begin(), queue.end());
  std::cout << prefix << to_string(queue) << std::endl;
}

void example6() {
  std::cout << "example6()" << std::endl;
  gto::cqueue<int> queue(2);
  queue.push(1);
  std::cout << prefix << to_string(queue) << std::endl;
  try {
    auto x = queue[4];
    x++;
  } catch(const std::exception &e) {
    std::cout << prefix << "exception: " << e.what() << std::endl;
  }
  queue.push(2);
  std::cout << prefix << to_string(queue) << std::endl;
  try {
    queue.push(3);
  } catch(const std::exception &e) {
    std::cout << prefix << "exception: " << e.what() << std::endl;
  }
}

int main() {
  example1();
  example2();
  example3();
  example4();
  example5();
  example6();
  return 0;
}
