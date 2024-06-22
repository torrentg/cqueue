# cqueue

cqueue is a C++20 header-only [circular queue](https://en.wikipedia.org/wiki/Circular_buffer) container.

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
| `queue.push(10);` | `9` | `16` | &#10123;&#10114;&#10115;&#10116;&#10117;&#10118;&#10119;&#10120;&#10121;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737; |
| `queue.clear();` | `0` | `16` | &#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737;&#9737; |

cqueue is similar to a std::vector

* Random access iterators
* Dynamic memory management
* Constant time on access, insert and erase operations
* No external dependences
* Not thread-safe

... but it behaves like a queue

* `push()` add an element at the end
* `pop()` remove the first element
* Cannot insert or remove items in the middle

... where

* Items are stored 'modulus' _n_ (`queue[pos] = buffer[(mFront+pos)%mReserved]`)
* Memory new/delete calls are minimized

... having some extras

* Access index always is checked
* `push_front()` support
* `pop_back()` support

... and some lacks

* Comparison operators currently not supported
* Restricted to C++20 compilers

## Motivation

Memory management (alloc/free) done by `std::deque` is very intense in queue-like operations (push/pop). [deque-prof.cpp](deque-prof.cpp) and [cqueue-prof.cpp](cqueue-prof.cpp) implements a trivial
use case of a queue where we push _n_ items and pop _n_ items repeatedly.

Using [`std::deque`](https://en.cppreference.com/w/cpp/container/deque):

<pre><code>> valgrind --leak-check=full --show-leak-kinds=all ./deque-prof
...
==10003== HEAP SUMMARY:
==10003==     in use at exit: 0 bytes in 0 blocks
==10003==   total heap usage: <mark>156,253 allocs, 156,253 frees, 80,073,280 bytes allocated</mark>
...
</code></pre>

Using [`gto:cqueue`](https://github.com/torrentg/cqueue):

<pre><code>> valgrind --leak-check=full --show-leak-kinds=all ./cqueue-prof
...
==10024== HEAP SUMMARY:
==10024==     in use at exit: 0 bytes in 0 blocks
==10024==   total heap usage: <mark>4 allocs, 4 frees, 72,928 bytes allocated</mark>
...
</code></pre>

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

| Name | Contribution |
|:-----|:-------------|
| [Gerard Torrent](https://github.com/torrentg/) | Initial work<br/>Code maintainer|
| [G. Sliepen](https://codereview.stackexchange.com/users/129343/g-sliepen) | [Code review (I)](https://codereview.stackexchange.com/questions/281005/simple-c-circular-queue)<br/>[Code review (II)](https://codereview.stackexchange.com/questions/281152/a-not-so-simple-c20-circular-queue) |
| [Toby Speight](https://codereview.stackexchange.com/users/75307/toby-speight) | [Code review (II)](https://codereview.stackexchange.com/questions/281152/a-not-so-simple-c20-circular-queue) |
| [372046933](https://github.com/372046933) | [Fix compile](https://github.com/torrentg/cqueue/pull/3)<br/>[Fix performance issue](https://github.com/torrentg/cqueue/pull/4) |


## License

This project is licensed under the GNU LGPL v3 License - see the [LICENSE](LICENSE) file for details.
