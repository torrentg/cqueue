# TODO

1. Solve issues identified in the [code review](https://codereview.stackexchange.com/questions/281005/simple-c-circular-queue)
    * Avoid object contruction on memory allocation
    * Construct/destroy items only on push/pop
    * Rename cqueue to circular_queue
    * Considere to add `constexpr` support
2. Considere to add `shrink_to_fit` method
3. Execute profiler

    ```sh
    g++ -pg -std=c++20 -g -o cqueue-mem cqueue-mem.cpp && \
    ./cqueue-mem && \
    gprof ./cqueue-mem gmon.out > cqueue-mem.gmon
    ```

4. Execute valgrind massif

    ```sh
    g++ -std=c++20 -g -o cqueue-mem cqueue-mem.cpp && \
    valgrind --tool=massif --time-unit=B --threshold=0.0 ./cqueue-mem \
    ms_print --threshold=0.0 massif.out.xxxxxx > massif.out.cqueue
    ```
