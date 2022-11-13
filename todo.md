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
5. ~~Rely on member-value-initialization instead on explicit value on constructor.~~
6. ~~Define MAX_CAPACITY~~
7. ~~Let emplace() and push() return a reference~~
8. ~~push() -> push_back(), pop() -> pop_back(), emplace -> emplace_back()~~
9. ~~Create emplace_front()~~
10. ~~DEFAULT_RESERVE -> MIN_ALLOCATE~~
11. ~~Solve error in shrink_to_fit(). This method will just not shrink at all if there are less than DEFAULT_RESERVED elements in the queue (mLength), even if you have a huge amount of memory reserved (mReserved)!~~
12. resizeIfRequired() + resize(): solve case n == mReserved.
13. solve case when resize(0) 
14. Write the safer pos == size() ? size() : pos + 1
15. ~~using allocator_traits = std::allocator_traits<allocator_type>;~~
16. Use a single implementation for iterators.
17. The iterator size comparison operators can be reduced to a single <=>.
18. Convert identifiers from camelCase to snake_case.
19. Review [[nodiscard]] usage.
20. Consider adding cbegin() and cend().
21. Consider adding reversed begin/end iterator functions.
22. ~~Default-initialise member mAllocator - allocator_type mAllocator = {};~~
23. ~~Add -Weffc++ as compiler option~~
24. resize() private -> public + review standar.
25. push(T &&val) -> push(T val)
26. Constructor cqueue(const cqueue &other) forwards to cqueue{other, allocator_traits::select_on_container_copy_construction(other.get_allocator())}
27. Review cqueue(cqueue &&other, const_alloc_reference alloc) implementation
28. Because operator=() uses copy-and-swap, there's no need to test for self-assignment.
29. swap() itself functions perfectly well for self-swap, so we can eliminate the condition there.
30. ~~Use 'trailing return type' (return type = auto)~~
31. Use deallocator in resize()
32. Add reference to review2 + contributors
33. Add support for propagate_on_container_move_assignment + propagate_on_container_copy_assignment
