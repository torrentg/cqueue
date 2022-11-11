# TODO

1. Solve issues identified in the [code review](https://codereview.stackexchange.com/questions/281005/simple-c-circular-queue)
    * Rename cqueue to circular_queue
2. Execute profiler

    ```sh
    g++ -pg -std=c++20 -g -o cqueue-mem cqueue-mem.cpp && \
    ./cqueue-mem && \
    gprof ./cqueue-mem gmon.out > cqueue-mem.gmon
    ```

3. Execute valgrind massif

    ```sh
    g++ -std=c++20 -g -o cqueue-mem cqueue-mem.cpp && \
    valgrind --tool=massif --time-unit=B --threshold=0.0 ./cqueue-mem \
    ms_print --threshold=0.0 massif.out.xxxxxx > massif.out.cqueue
    ```

4. Considere to add reverse iterators
