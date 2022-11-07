# cqueue

cqueue is a simple C++20 header-only [circular queue](https://en.wikipedia.org/wiki/Circular_buffer) container.

cqueue is similar to a std::vector

* Random access iterators
* Dynamic memory management
* Constant time on access, insert and erase operations
* No external dependences
* Not thread-safe

... but it behaves like a queue

* push() add an element at the end
* pop() remove the first element
* Items in the middle cannot be removed

... where

* Items are stored sequentially 'modulus' _n_ (where _n_ is the current reserved storage size)
* Memory new/delete calls are minimized

... having some extras

* Access index always is checked
* push_front() support
* pop_back() support

... and some lacks

* Custom allocator currently not supported
* Comparison operators currently not supported
* Limited constexpr support

## Motivation

The memory usage done by `std::deque`:

* Number of calls to alloc/free
* Number of bytes allocated/deallocated

[deque-mem.cpp](deque-mem.cpp) and [cqueue-mem.cpp](cqueue-mem.cpp) implements a trivial
use case of a queue where we push _n_ items and pop _n_ items repeatedly.

Using [`std::deque`](https://en.cppreference.com/w/cpp/container/deque):

<pre><code>> valgrind --leak-check=full --show-leak-kinds=all ./deque-mem
...
==1323828== HEAP SUMMARY:
==1323828==     in use at exit: 0 bytes in 0 blocks
==1323828==   total heap usage: <mark>15,628 allocs, 15,628 frees, 8,073,280 bytes allocated</mark>
...
</code></pre>

Using [`gto:cqueue`](https://github.com/torrentg/cqueue):

<pre><code>> valgrind --leak-check=full --show-leak-kinds=all ./cqueue-mem
...
==1323952== HEAP SUMMARY:
==1323952==     in use at exit: 0 bytes in 0 blocks
==1323952==   total heap usage: <mark>6 allocs, 6 frees, 73,024 bytes allocated</mark>
...
</code></pre>

## Performance

TODO
## Usage

Drop off [`cqueue.hpp`](cqueue.hpp) in your project and start using the cqueue.

Read [`cqueue-example.cpp`](cqueue-example.cpp) file to see how to use it.

## Testing

```bash
# example
make example

# unit tests
make tests

# code coverage
make coverage
firefox coverage/index.html &
```

## Contributors

* **Gerard Torrent** - _Initial work_ - [torrentg](https://github.com/torrentg/)
* G. Sliepen - [Code review (I)](https://codereview.stackexchange.com/questions/281005/simple-c-circular-queue) - [G. Sliepen](https://codereview.stackexchange.com/users/129343/g-sliepen)

## License

This project is licensed under the GNU LGPL v3 License - see the [LICENSE](LICENSE) file for details.
