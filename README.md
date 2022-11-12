# cqueue

cqueue is a simple C++20 header-only [circular queue](https://en.wikipedia.org/wiki/Circular_buffer) container.

| Statement | Length | Reserved | Content |
|:--------- |:------:|:--------:|:--------|
| `cqueue<int> queue;` | `0` | `0` | `-` |
| `queue.push(1);` | `1` | `8` | &#10122;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;  |
| `queue.push(2);` | `2` | `8` | &#10122;&#10113;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;  |
| `queue.push(3);` | `3` | `8` | &#10122;&#10113;&#10114;&#9737;&#9737;&#9737;&#9737;&#9737;  |
| `queue.pop();` | `2` | `8` | &#9737;&#10123;&#10114;&#9737;&#9737;&#9737;&#9737;&#9737;  |
| `x = queue[1]; // = 3` | `2` | `8` | ` = buffer[(1+1)%8]` |
| `for (int i = 4; i <= 8; ++i)`<br/>&nbsp;&nbsp;&nbsp;&nbsp;`queue.push(i);` | `7` | `8` | &#9737;&#10123;&#10114;&#10115;&#10116;&#10117;&#10118;&#10119;  |
| `queue.push(9);` | `8` | `8` | &#10120;&#10123;&#10114;&#10115;&#10116;&#10117;&#10118;&#10119; |
| `queue.push(10);` | `9` | `16` | &#10123;&#10114;&#10115;&#10116;&#10117;&#10118;&#10119;&#10119;&#10121;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737; |
| `queue.clear();` | `0` | `16` | &#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737; |

cqueue is similar to a std::vector

* Random access iterators
* Dynamic memory management
* Constant time on access, insert and erase operations
* No external dependences
* Not thread-safe

... but it behaves like a queue

* push() add an element at the end
* pop() remove the first element
* Cannot insert or remove items in the middle

... where

* Items are stored 'modulus' _n_ (`queue[pos] = buffer[(mFront+pos)%mReserved]`)
* Memory new/delete calls are minimized

... having some extras

* Access index always is checked
* push_front() support
* pop_back() support

... and some lacks

* Comparison operators currently not supported
* Restricted to C++20 compilers

## Motivation

Memory management (alloc/free) done by `std::deque` is very intense in queue-like operations (push/pop). [deque-mem.cpp](deque-mem.cpp) and [cqueue-mem.cpp](cqueue-mem.cpp) implements a trivial
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
