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

## Usage

Drop off [`cqueue.hpp`](cqueue.hpp) in your project and start using the cqueue.

Read [`example.cpp`](example.cpp) file to see how to use it.

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

## Authors

* **Gerard Torrent** - _Initial work_ - [torrentg](https://github.com/torrentg/)

## License

This project is licensed under the GNU LGPL v3 License - see the [LICENSE](LICENSE) file for details.
