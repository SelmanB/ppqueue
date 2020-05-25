# ppqueue
Multithreaded and MT-safe priority queue as a drop-in replacement for `std::priortiy_queue`.

## Algorithm
Every ppqueue object has two associated thread in addition to the creator thread. These threads are constructed and destructed with the object. Heap operations are performed by these threads after O(1) pushing/popping by a calling thread. All interfaces to the object are mutex-protected for MT-safety.

## Compatibility
Standard C++ thread and synchronization structures are used for platform independence. However, it is tested only in a GNU/Linux system. Compatibility with C++11 STL is aimed. All public functions, member types, and swap function are implemented the same way as the original `std::priority_queue`, with the exception of allocator constructors, which are not implemented.

