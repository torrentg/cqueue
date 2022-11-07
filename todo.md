# TODO

1. Use `valgrind massif` to check how memory is used in `std::dequeue`
2. Solve issues identified in the [code review](https://codereview.stackexchange.com/questions/281005/simple-c-circular-queue)
    * Remove `MAX_CAPACITY` limitation
    * Avoid object contruction on memory allocation
    * Construct/destroy items only on push/pop
    * Rename cqueue to circular_queue
    * Use `[[nodiscard]]` when possible
    * More `noexcept` are possible
    * Considere to add `constexpr` support
3. Considere to add `shrink_to_fit` method
4. Execute profiler

    ```sh
    g++ -pg -std=c++20 -g -o cqueue-mem cqueue-mem.cpp && \
    ./cqueue-mem && \
    gprof ./cqueue-mem gmon.out > cqueue-mem.gmon
    ```
