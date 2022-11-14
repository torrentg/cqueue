#define CATCH_CONFIG_MAIN

#include <limits>
#include "catch.hpp"
#include "cqueue.hpp"

using std::string;
using gto::cqueue;

template <class T>
struct custom_allocator {
  using propagate_on_container_swap = std::true_type;
  typedef T value_type;
  std::size_t numBytes = 0;
  custom_allocator() noexcept {}
  template <class U> custom_allocator (const custom_allocator<U>&) noexcept {}
  T* allocate (std::size_t n) { numBytes += n*sizeof(T); return static_cast<T*>(::operator new(n*sizeof(T))); }
  void deallocate (T* p, std::size_t n) { numBytes -= n*sizeof(T); ::delete(p); }
};

template <class T, class U>
constexpr bool operator== (const custom_allocator<T>&, const custom_allocator<U>&) noexcept { return false; }

template <class T, class U>
constexpr bool operator!= (const custom_allocator<T>&, const custom_allocator<U>&) noexcept { return true; }

TEST_CASE("cqueue") {

  SECTION("sizeof") {
    CHECK(sizeof(cqueue<int>) == sizeof(int *) + 4*sizeof(std::size_t));
    CHECK(sizeof(cqueue<int, custom_allocator<int>>) == sizeof(int *) + 5*sizeof(std::size_t));
  }

  SECTION("max_capacity") {
    CHECK(cqueue<int>::max_capacity() == std::numeric_limits<std::ptrdiff_t>::max());
  }

  SECTION("constructor-exception") {
    CHECK_THROWS(cqueue<long long>(std::numeric_limits<std::size_t>::max()));
  }

  SECTION("default constructor") {
    cqueue<int> queue;
    CHECK(queue.capacity() == 0);
    CHECK(queue.size() == 0);
    CHECK(queue.reserved() == 0);
    CHECK(queue.empty());
    CHECK_THROWS(queue.front());
    CHECK_THROWS(queue.back());
    CHECK(queue.begin() == queue.end());
    CHECK(queue.pop() == false);
    CHECK(queue.pop_back() == false);
    CHECK_THROWS(queue[0]);
    queue.clear();
    CHECK(queue.size() == 0);
  }

  SECTION("copy-constructor") {
    cqueue<int> queue1(10);
    queue1.push(1);
    queue1.push(2);
    queue1.push(3);
    queue1.pop();
    CHECK(queue1.capacity() == 10);
    CHECK(queue1.size() == 2);
    CHECK(queue1[0] == 2);
    CHECK(queue1[1] == 3);
    cqueue<int> queue2(queue1);
    CHECK(queue2.capacity() == 10);
    CHECK(queue2.size() == 2);
    CHECK(queue2[0] == 2);
    CHECK(queue2[1] == 3);
  }

  SECTION("copy-constructor-with-allocator") {
    cqueue<int> queue1(10);
    queue1.push(1);
    queue1.push(2);
    queue1.push(3);
    queue1.pop();
    CHECK(queue1.capacity() == 10);
    CHECK(queue1.size() == 2);
    CHECK(queue1[0] == 2);
    CHECK(queue1[1] == 3);
    cqueue<int> queue2(queue1, std::allocator<int>());
    CHECK(queue2.capacity() == 10);
    CHECK(queue2.size() == 2);
    CHECK(queue2[0] == 2);
    CHECK(queue2[1] == 3);
  }

  SECTION("move-constructor") {
    cqueue<int> queue1(10);
    queue1.push(1);
    queue1.push(2);
    queue1.push(3);
    queue1.pop();
    CHECK(queue1.capacity() == 10);
    CHECK(queue1.size() == 2);
    CHECK(queue1[0] == 2);
    CHECK(queue1[1] == 3);
    cqueue<int> queue2(std::move(queue1));
    CHECK(queue2.capacity() == 10);
    CHECK(queue2.size() == 2);
    CHECK(queue2[0] == 2);
    CHECK(queue2[1] == 3);
  }

  SECTION("move-constructor-with-allocator") {
    cqueue<int> queue1(10);
    queue1.push(1);
    queue1.push(2);
    queue1.push(3);
    queue1.pop();
    CHECK(queue1.capacity() == 10);
    CHECK(queue1.size() == 2);
    CHECK(queue1[0] == 2);
    CHECK(queue1[1] == 3);
    cqueue<int> queue2(std::move(queue1), std::allocator<int>());
    CHECK(queue2.capacity() == 10);
    CHECK(queue2.size() == 2);
    CHECK(queue2[0] == 2);
    CHECK(queue2[1] == 3);
  }

  SECTION("move-constructor-with-custom-allocator") {
    cqueue<int,custom_allocator<int>> queue1(10, custom_allocator<int>());
    queue1.push(1);
    queue1.push(2);
    queue1.push(3);
    queue1.pop();
    CHECK(queue1.capacity() == 10);
    CHECK(queue1.size() == 2);
    CHECK(queue1[0] == 2);
    CHECK(queue1[1] == 3);
    cqueue<int,custom_allocator<int>> queue2(std::move(queue1), custom_allocator<int>());
    CHECK(queue2.capacity() == 10);
    CHECK(queue2.size() == 2);
    CHECK(queue2[0] == 2);
    CHECK(queue2[1] == 3);
  }

  SECTION("copy-assignment") {
    cqueue<int> queue1(10);
    queue1.push(1);
    queue1.push(2);
    queue1.push(3);
    queue1.pop();
    CHECK(queue1.capacity() == 10);
    CHECK(queue1.size() == 2);
    CHECK(queue1[0] == 2);
    CHECK(queue1[1] == 3);
    cqueue<int> queue2;
    queue2.push(99);
    CHECK(queue2[0] == 99);
    queue2 = queue1;
    CHECK(queue1.capacity() == queue1.capacity());
    CHECK(queue2.size() == queue1.size());
    CHECK(queue2[0] == queue1[0]);
    CHECK(queue2[1] == queue1[1]);
  }

  SECTION("copy-assignment") {
    cqueue<int> queue1(10);
    queue1.push(1);
    queue1.push(2);
    queue1.push(3);
    queue1.pop();
    CHECK(queue1.capacity() == 10);
    CHECK(queue1.size() == 2);
    CHECK(queue1[0] == 2);
    CHECK(queue1[1] == 3);
    cqueue<int> queue2;
    queue2.push(99);
    CHECK(queue2[0] == 99);
    queue2 = std::move(queue1);
    CHECK(queue2.capacity() == 10);
    CHECK(queue2.size() == 2);
    CHECK(queue2[0] == 2);
    CHECK(queue2[1] == 3);
    CHECK(queue1.size() == 1);
    CHECK(queue1[0] == 99);
  }

  SECTION("capacity") {
    {
      cqueue<int> queue;
      CHECK(queue.capacity() == 0);
      CHECK(queue.reserved() == 0);
      CHECK(queue.size() == 0);
    }
    {
      cqueue<int> queue(20);
      CHECK(queue.reserved() == 0);
      for (int i = 0; i < 8; i++) {
        queue.push(i);
      }
      CHECK(queue.size() == 8);
      CHECK(queue.reserved() == 8);
      int *ptr = &queue[0];
      queue.reserve(8);
      CHECK(queue.reserved() == 8);
      CHECK(&queue[0] == ptr);
    }
    {
      cqueue<int> queue(10000000);
      CHECK(queue.capacity() == 10000000);
      CHECK(queue.reserved() == 0);
      CHECK(queue.size() == 0);
    }
  }

  SECTION("reserved") {
    {
      cqueue<int> queue(10000000);
      CHECK(queue.reserved() == 0);
      queue.push(1);
      CHECK(queue.reserved() == 8); // default reserve = 8
      for (int i = 0; i < 10; i++)
        queue.push(i);
      CHECK(queue.reserved() == 16); // growth factor = 2
    }
    {
      cqueue<int> queue(5);
      CHECK(queue.reserved() == 0);
      queue.push(1);
      CHECK(queue.reserved() == 5); // default reserve limited by capacity
    }
    {
      cqueue<int> queue(10);
      CHECK(queue.reserved() == 0);
      queue.push(1);
      CHECK(queue.reserved() == 8); // default reserve = 8
      for (int i = 0; i < 8; i++)
        queue.push(i);
      CHECK(queue.reserved() == 10); // growth factor limited by capacity
    }
  }

  SECTION("empty") {
    cqueue<int> queue;
    CHECK(queue.empty());
    CHECK(queue.size() == 0);
    queue.push(1);
    CHECK(!queue.empty());
    CHECK(queue.size() == 1);
  }

  SECTION("size") {
    cqueue<int> queue;
    CHECK(queue.empty());
    for (int i = 1; i <= 10; i++) {
      queue.push(i);
      CHECK(queue.size() == static_cast<std::size_t>(i));
    }
  }

  SECTION("front") {
    {
      cqueue<int> queue;
      CHECK_THROWS(queue.front());
      queue.push(1);
      queue.push(2);
      CHECK(queue.front() == 1);
    }
    {
      cqueue<int> queue;
      const cqueue<int> &xqueue = const_cast<const cqueue<int>&>(queue);
      CHECK_THROWS(xqueue.front());
      queue.push(1);
      queue.push(2);
      CHECK(xqueue.front() == 1);
    }
  }

  SECTION("back") {
    {
      cqueue<int> queue;
      CHECK_THROWS(queue.back());
      queue.push(1);
      queue.push(2);
      CHECK(queue.back() == 2);
    }
    {
      cqueue<int> queue;
      const cqueue<int> &xqueue = const_cast<const cqueue<int>&>(queue);
      CHECK_THROWS(xqueue.back());
      queue.push(1);
      queue.push(2);
      CHECK(xqueue.back() == 2);
    }
  }

  SECTION("push_back-ref") {
    cqueue<int> queue;
    CHECK(queue.empty());
    queue.push_back(1);
    CHECK(queue.back() == 1);
    queue.push_back(2);
    CHECK(queue.back() == 2);
  }

  SECTION("push_back-move") {
    cqueue<string> queue(10);
    queue.push_back("1");
    queue.push_back(std::string("2"));
    string aux = "3";
    queue.push_back(std::move(aux));
    CHECK(queue.size() == 3);
    CHECK(queue.reserved() == 8);
    CHECK(queue.front() == "1");
    CHECK(queue.back() == "3");
  }

  SECTION("push_front-ref") {
    {
      cqueue<string> queue;
      queue.push_front("1");
      CHECK(queue.size() == 1);
      CHECK(queue[0] == "1");
    }
    {
      cqueue<string> queue;
      queue.push("1");
      queue.push("2");
      string aux = "99";
      queue.push_front(aux);
      CHECK(queue.size() == 3);
      CHECK(queue[0] == "99");
      CHECK(queue[1] == "1");
      CHECK(queue[2] == "2");
    }
    {
      cqueue<string> queue;
      queue.push("1");
      queue.push("2");
      queue.pop();
      string aux = "99";
      queue.push_front(aux);
      CHECK(queue.size() == 2);
      CHECK(queue[0] == "99");
      CHECK(queue[1] == "2");
    }
  }

  SECTION("push_front-move") {
    {
      cqueue<int> queue;
      queue.push_front(1);
      CHECK(queue.size() == 1);
      CHECK(queue[0] == 1);
    }
    {
      cqueue<int> queue;
      queue.push(1);
      queue.push(2);
      queue.push_front(99);
      CHECK(queue.size() == 3);
      CHECK(queue[0] == 99);
      CHECK(queue[1] == 1);
      CHECK(queue[2] == 2);
    }
    {
      cqueue<int> queue;
      queue.push(1);
      queue.push(2);
      queue.pop();
      queue.push_front(99);
      CHECK(queue.size() == 2);
      CHECK(queue[0] == 99);
      CHECK(queue[1] == 2);
    }
  }

  SECTION("emplace_back") {
    {
      cqueue<string> queue(10);
      auto x1 = queue.emplace_back("1");
      auto x2 = queue.emplace_back("2");
      CHECK(queue.size() == 2);
      CHECK(queue.reserved() == 8);
      CHECK(queue.front() == "1");
      CHECK(queue.back() == "2");
    }
    { // emplace == emplace_back
      cqueue<string> queue(10);
      auto x1 = queue.emplace("1");
      auto x2 = queue.emplace("2");
      CHECK(queue.size() == 2);
      CHECK(queue.reserved() == 8);
      CHECK(queue.front() == "1");
      CHECK(queue.back() == "2");
    }
  }

  SECTION("emplace_front") {
    cqueue<string> queue(10);
    auto x1 = queue.emplace_front("1");
    auto x2 = queue.emplace_front("2");
    CHECK(queue.size() == 2);
    CHECK(queue.reserved() == 8);
    CHECK(queue.front() == "2");
    CHECK(queue.back() == "1");
  }

  SECTION("exception-on-resize") {
    static bool fail = false;
    class myclass {
      void assign(int n) {
        if (fail && n == 3) {
          throw std::exception();
        } else {
          num = n;
        }
      }
      public:
        int num = 0;
        myclass(int x = 0) : num(x) {}
        myclass(const myclass &o) { assign(o.num); }
        myclass(myclass &&o) { assign(o.num); }
        myclass& operator=(const myclass &o) = default;
        myclass& operator=(myclass &&o) = default;
        void swap(myclass &o) { return std::swap(num, o.num); }
    };

    cqueue<myclass> queue(10);
    queue.push(myclass(1));
    queue.push(myclass(2));
    queue.push(myclass(3));  // <- will fail when resize
    queue.push(myclass(4));
    queue.push(myclass(5));
    queue.push(myclass(6));
    queue.push(myclass(7));
    queue.push(myclass(8));

    CHECK(queue.size() == 8);
    CHECK(queue.reserved() == 8);
    for (std::size_t i = 0; i < 8; i++) {
      CHECK(queue[i].num == static_cast<int>(i + 1));
    }

    fail = true;
    CHECK_THROWS(queue.push(myclass(9)));
    CHECK(queue.size() == 8);
    CHECK(queue.reserved() == 8);
    for (std::size_t i = 0; i < 8; i++) {
      CHECK(queue[i].num == static_cast<int>(i + 1));
    }
  }

  SECTION("subscript") {
    cqueue<int> queue;
    const cqueue<int> &xqueue = const_cast<const cqueue<int>&>(queue);
    CHECK_THROWS(queue[0]);
    CHECK_THROWS(xqueue[0]);
    for (int i = 0; i < 10; i++) {
      queue.push(i);
      CHECK(queue[static_cast<size_t>(i)] == i);
      CHECK(xqueue[static_cast<size_t>(i)] == i);
      CHECK_THROWS(queue[static_cast<size_t>(i) + 1]);
      CHECK_THROWS(xqueue[static_cast<size_t>(i) + 1]);
    }
  }

  SECTION("pop") {
    cqueue<int> queue(5);
    CHECK(!queue.pop());
    queue.push(1);
    CHECK(queue.size() == 1);
    queue.pop();
    CHECK(queue.empty());
    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.push(4);
    queue.push(5);
    CHECK(queue.size() == 5);
    queue.pop();
    CHECK(queue.size() == 4);
    CHECK(queue.front() == 2);
    CHECK(queue.back() == 5);
  }

  SECTION("pop_back") {
    cqueue<int> queue(5);
    CHECK(!queue.pop_back());
    queue.push(1);
    CHECK(queue.size() == 1);
    queue.pop_back();
    CHECK(queue.empty());
    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.push(4);
    queue.push(5);
    CHECK(queue.size() == 5);
    queue.pop_back();
    CHECK(queue.size() == 4);
    CHECK(queue.front() == 1);
    CHECK(queue.back() == 4);
  }

  SECTION("begin") {
    cqueue<int> queue;
    CHECK(queue.begin() == queue.end());
    queue.push(1);
    auto it = queue.begin();
    CHECK(*it == 1);
    const cqueue<int> &xqueue = const_cast<const cqueue<int>&>(queue);
    auto xit = xqueue.begin();
    CHECK(*xit == 1);
  }

  SECTION("end") {
    cqueue<int> queue;
    queue.push(1);
    auto it = queue.begin();
    CHECK(it < queue.end());
    ++it;
    CHECK(it == queue.end());
    ++it;
    CHECK(it == queue.end());
    const cqueue<int> &xqueue = const_cast<const cqueue<int>&>(queue);
    auto xit = xqueue.begin();
    //CHECK(xit < xqueue.end());
    ++xit;
    CHECK(xit == xqueue.end());
    ++xit;
    CHECK(xit == xqueue.end());
  }

  SECTION("iterator") {
    cqueue<string> queue;
    queue.push("1");
    queue.push("2");
    queue.push("3");
    {
      auto it = queue.begin();
      CHECK(!it->empty());
      CHECK(!(*it).empty());
      CHECK(it[0] == "1");
      CHECK(it[1] == "2");
      CHECK(it[2] == "3");
      CHECK_THROWS(it[3]);
      CHECK_THROWS(it[-1]);
      ++it;
      CHECK(it[-1] == "1");
      CHECK(it[0] == "2");
      CHECK(it[1] == "3");
    }
    {
      auto it = queue.begin();
      CHECK(it++ == queue.begin());
      CHECK(*it == "2");
      CHECK(*(++it) == "3");
      CHECK(++it == queue.end());
      CHECK(++it == queue.end());
    }
    {
      auto it = queue.end();
      CHECK(it-- == queue.end());
      CHECK(*it == "3");
      CHECK(*(--it) == "2");
      it = queue.begin();
      CHECK(--it == --queue.begin());
      CHECK(--it == --queue.begin());
    }
    {
      auto it1 = queue.begin();
      auto it2 = ++queue.begin();
      CHECK(it1 == it1);
      CHECK(it1 != it2);
      CHECK(it1 <  it2);
      CHECK(it2 >  it1);
      CHECK(it1 <= it1);
      CHECK(it1 <= it2);
      CHECK(it1 >= it1);
      CHECK(it2 >= it1);
    }
    {
      auto it1 = queue.begin();
      auto it3 = ++(++queue.begin());
      it1 += 2;
      CHECK(it1 == it3);
      it1 += 1000;
      CHECK(it1 == queue.end());
    }
    {
      auto it1 = queue.begin();
      auto it3 = ++(++queue.begin());
      it3 -= 2;
      CHECK(it1 == it3);
      it1 -= 1000;
      CHECK(it1 == --queue.begin());
    }
    {
      auto it1 = queue.begin();
      auto it3 = ++(++queue.begin());
      CHECK(it1 + 2 == it3);
      CHECK(2 + it1 == it3);
      CHECK(it1 + 1000 == queue.end());
      CHECK(it3 - 2 == it1);
      CHECK(2 - it3 == it1);
      CHECK(it1 - 1000 == --queue.begin());
    }
    {
      auto it1 = queue.begin();
      auto it3 = ++(++queue.begin());
      CHECK(it3 - it1 == +2);
      CHECK(it1 - it3 == -2);
    }
  }

  SECTION("const_iterator") {
    cqueue<string> xqueue;
    xqueue.push("1");
    xqueue.push("2");
    xqueue.push("3");
    const cqueue<string> &queue = xqueue;
    CHECK(queue.begin() == xqueue.cbegin());
    CHECK(queue.end() == xqueue.cend());
    {
      auto it = queue.begin();
      CHECK(!it->empty());
      CHECK(!(*it).empty());
      CHECK(it[0] == "1");
      CHECK(it[1] == "2");
      CHECK(it[2] == "3");
      CHECK_THROWS(it[3]);
      CHECK_THROWS(it[-1]);
      ++it;
      CHECK(it[-1] == "1");
      CHECK(it[0] == "2");
      CHECK(it[1] == "3");
    }
    {
      auto it = queue.begin();
      CHECK(it++ == queue.begin());
      CHECK(*it == "2");
      CHECK(*(++it) == "3");
      CHECK(++it == queue.end());
      CHECK(++it == queue.end());
    }
    {
      auto it = queue.end();
      CHECK(it-- == queue.end());
      CHECK(*it == "3");
      CHECK(*(--it) == "2");
      it = queue.begin();
      CHECK(--it == --queue.begin());
      CHECK(--it == --queue.begin());
    }
    {
      auto it1 = queue.begin();
      auto it2 = ++queue.begin();
      CHECK(it1 == it1);
      CHECK(it1 != it2);
      CHECK(it1 <  it2);
      CHECK(it2 >  it1);
      CHECK(it1 <= it1);
      CHECK(it1 <= it2);
      CHECK(it1 >= it1);
      CHECK(it2 >= it1);
    }
    {
      auto it1 = queue.begin();
      auto it3 = ++(++queue.begin());
      it1 += 2;
      CHECK(it1 == it3);
      it1 += 1000;
      CHECK(it1 == queue.end());
    }
    {
      auto it1 = queue.begin();
      auto it3 = ++(++queue.begin());
      it3 -= 2;
      CHECK(it1 == it3);
      it1 -= 1000;
      CHECK(it1 == --queue.begin());
    }
    {
      auto it1 = queue.begin();
      auto it3 = ++(++queue.begin());
      CHECK(it1 + 2 == it3);
      CHECK(2 + it1 == it3);
      CHECK(it1 + 1000 == queue.end());
      CHECK(it3 - 2 == it1);
      CHECK(2 - it3 == it1);
      CHECK(it1 - 1000 == --queue.begin());
    }
    {
      auto it1 = queue.begin();
      auto it3 = ++(++queue.begin());
      CHECK(it3 - it1 == +2);
      CHECK(it1 - it3 == -2);
    }
  }

  SECTION("range-loop") {
    {
      cqueue<int> queue;
      queue.push(1);
      queue.push(2);
      queue.push(3);
      for (auto &item : queue) {
        CHECK((0 < item && item < 4));
      }
      const cqueue<int> &xqueue = const_cast<const cqueue<int>&>(queue);
      for (auto &item : xqueue) {
        CHECK((0 < item && item < 4));
      }
    }
  }

  SECTION("reserve") {
    {
      cqueue<int> queue(100);
      CHECK(queue.reserved() == 0);
      queue.reserve(10);
      CHECK(queue.reserved() == 10);
      queue.reserve(8);
      CHECK(queue.reserved() == 10);
      CHECK_THROWS(queue.reserve(1000));
    }
  }

  SECTION("shrink_to_fit") {
    cqueue<int> queue(20);
    CHECK(queue.reserved() == 0);
    queue.shrink_to_fit();  // nothing to shrink
    CHECK(queue.reserved() == 0);
    queue.push(1);
    CHECK(queue.reserved() == 8);
    queue.shrink_to_fit();  // not shrinked because len <= 8
    CHECK(queue.reserved() == 8);
    for (size_t i = 2; i <= 12; i++) {
      queue.push(static_cast<int>(i));
    }
    CHECK(queue.size() == 12);
    CHECK(queue.reserved() == 16);
    queue.shrink_to_fit();  // shrink mem
    CHECK(queue.size() == 12);
    CHECK(queue.reserved() == 12);
    for (size_t i = 0; i < 12; i++) {
      CHECK(queue[i] == static_cast<int>(i + 1));
    }
    queue.shrink_to_fit();  // already shrinked
    CHECK(queue.reserved() == 12);
    CHECK(queue.size() == 12);
    queue.clear();
    queue.shrink_to_fit();
    CHECK(queue.reserved() == 0);
    CHECK(queue.empty());
    { // case when capacity < 8
      cqueue<int> queue(5);
      queue.push(1);
      CHECK(queue.reserved() == 5);
      CHECK(queue.size() == 1);
      queue.shrink_to_fit();
      CHECK(queue.reserved() == 5);
    }
  }

  SECTION("clear") {
    cqueue<int> queue;
    queue.push(1);
    queue.push(2);
    CHECK(!queue.empty());
    queue.clear();
    CHECK(queue.empty());
  }

  SECTION("swap") {
    cqueue<int> queue1(2);
    cqueue<int> queue2(10);

    queue1.push(1);

    CHECK(queue1.capacity() == 2);
    CHECK(queue1.size() == 1);
    CHECK(queue1.reserved() == 2);
    CHECK(queue1.front() == 1);
    CHECK(queue1.back() == 1);

    queue2.push(1);
    queue2.push(2);
    queue2.push(3);
    queue2.pop();

    CHECK(queue2.capacity() == 10);
    CHECK(queue2.size() == 2);
    CHECK(queue2.reserved() == 8);
    CHECK(queue2.front() == 2);
    CHECK(queue2.back() == 3);

    queue1.swap(queue2);

    CHECK(queue2.capacity() == 2);
    CHECK(queue2.size() == 1);
    CHECK(queue2.reserved() == 2);
    CHECK(queue2.front() == 1);
    CHECK(queue2.back() == 1);

    CHECK(queue1.capacity() == 10);
    CHECK(queue1.size() == 2);
    CHECK(queue1.reserved() == 8);
    CHECK(queue1.front() == 2);
    CHECK(queue1.back() == 3);

    // auto-swap
    auto ptr = &queue1.front();
    queue1.swap(queue1);
    CHECK(ptr == &queue1.front());
    CHECK(queue1.capacity() == 10);
    CHECK(queue1.size() == 2);
    CHECK(queue1.reserved() == 8);
    CHECK(queue1.front() == 2);
    CHECK(queue1.back() == 3);

    // container swap
    {
      cqueue<int, custom_allocator<int>> queue1;
      cqueue<int, custom_allocator<int>> queue2;
      queue1.push(1);
      queue1.swap(queue2);
      CHECK(queue1.empty());
      CHECK(queue2.size() == 1);
    }
  }

  SECTION("capacity = 2 (without realloc)") {
    cqueue<int> queue(2);
    // content = []
    CHECK(queue.capacity() == 2);
    CHECK(queue.size() == 0);
    CHECK(queue.reserved() == 0);
    CHECK(queue.empty());
    CHECK_THROWS(queue.front());
    CHECK_THROWS(queue.back());
    CHECK(queue.pop() == false);
    CHECK_THROWS(queue[0]);
    // content = [1,.]
    queue.push(1);
    CHECK(queue.size() == 1);
    CHECK(queue.reserved() == 2);
    CHECK(queue.front() == 1);
    CHECK(queue.back() == 1);
    // content = [1,2]
    queue.push(2);
    CHECK(queue.size() == 2);
    CHECK(queue.front() == 1);
    CHECK(queue.back() == 2);
    CHECK(queue.reserved() == 2);
    CHECK_THROWS(queue.push(3));
    CHECK(queue.size() == 2);
    // content = [.,2]
    CHECK(queue.pop());
    CHECK(queue.size() == 1);
    CHECK(queue.front() == 2);
    CHECK(queue.back() == 2);
    // content = [3,2]
    queue.push(3);
    CHECK(queue.size() == 2);
    CHECK(queue.reserved() == 2);
    CHECK(queue.front() == 2);
    CHECK(queue.back() == 3);
    // content = [3,.]
    CHECK(queue.pop());
    CHECK(queue.size() == 1);
    CHECK(queue.reserved() == 2);
    CHECK(queue.front() == 3);
    CHECK(queue.back() == 3);
    // content = [.,.]
    CHECK(queue.pop());
    CHECK(queue.size() == 0);
    CHECK(queue.reserved() == 2);
    // content = [.,.]
    CHECK(queue.size() == 0);
    CHECK(queue.size() == 0);
    CHECK(queue.reserved() == 2);
  }

  SECTION("capacity = 10 (with 1 realloc)") {
    cqueue<int> queue(10);
    // content = [1,2,3,4,5,6,7,8]
    CHECK(queue.capacity() == 10);
    for (int i = 1; i <= 8; i++) {
      queue.push(i);
      CHECK(queue.size() == static_cast<std::size_t>(i));
      CHECK(queue.reserved() == 8);
      CHECK(queue.front() == 1);
      CHECK(queue.back() == i);
    }
    // content = [1,2,3,4,5,6,7,8,9,.]
    queue.push(9);
    CHECK(queue.size() == 9);
    CHECK(queue.reserved() == 10);
    CHECK(queue.front() == 1);
    CHECK(queue.back() == 9);
    // content = [.,2,3,4,5,6,7,8,9,.]
    CHECK(queue.pop());
    CHECK(queue.size() == 8);
    CHECK(queue.reserved() == 10);
    CHECK(queue.front() == 2);
    CHECK(queue.back() == 9);
    // content = [.,2,3,4,5,6,7,8,9,10]
    queue.push(10);
    CHECK(queue.size() == 9);
    CHECK(queue.reserved() == 10);
    CHECK(queue.front() == 2);
    CHECK(queue.back() == 10);
    // content = [11,2,3,4,5,6,7,8,9,10]
    queue.push(11);
    CHECK(queue.size() == 10);
    CHECK(queue.reserved() == 10);
    CHECK(queue.front() == 2);
    CHECK(queue.back() == 11);
    // content = [11,.,3,4,5,6,7,8,9,10]
    CHECK(queue.pop());
    CHECK(queue.size() == 9);
    CHECK(queue.reserved() == 10);
    CHECK(queue.front() == 3);
    CHECK(queue.back() == 11);
    // content = [.,.,.,.,.,.,.,.,.,.]
    queue.clear();
    CHECK(queue.size() == 0);
    CHECK(queue.reserved() == 10);
  }

  SECTION("memory-grow") {
    cqueue<int> queue;
    // content = [7,8,1,2,3,4,5,6]
    queue.push(9);
    queue.push(9);
    queue.push(1);
    queue.pop();
    queue.pop();
    for (int i = 2; i <= 8; i++) {
      queue.push(i);
    }
    CHECK(queue.size() == 8);
    CHECK(queue.reserved() == 8);
    // memory grow + reposition
    queue.push(9);
    // content = [1,2,3,4,5,6,7,8,9,.,.,.,.,.,.,.]
    CHECK(queue.front() == 1);
    CHECK(queue.back() == 9);
    CHECK(queue.size() == 9);
    CHECK(queue.reserved() == 16);
    for (int i = 0; i < 9; i++) {
      CHECK(queue[static_cast<size_t>(i)] == i + 1);
    }
  }

}
