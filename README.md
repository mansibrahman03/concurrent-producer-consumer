Producer-Consumer Implementation with Ring Buffer

Project Overview:
This project implements the classic producer-consumer problem using POSIX threads (pthreads) and mutex locks in C. The implementation uses a ring buffer (circular buffer) as the shared data structure between producer and consumer threads to demonstrate thread synchronization and concurrent programming techniques.

Features:
- Multi-threaded architecture with separate producer and consumer threads
- Thread-safe operations using mutex locks
- Condition variables to efficiently manage thread waiting states
- Circular buffer implementation to maximize memory usage
- Configurable sleep times for both producer and consumer
- Input-based control of data processing flow

Technical Implementation:
- Synchronization Primitives: Uses pthread mutex and condition variables
- Buffer Structure: Fixed-size ring buffer with 10 slots
- Thread Communication: Producer signals consumer when data is available, consumer signals producer when buffer space is available
- Safe Termination: Special "quit" message for clean program termination
- Error Checking: Thorough mutex error checking for robustness

How It Works:
1. Producer reads input values from stdin (value, producer sleep time, consumer sleep time, print code)
2. Producer acquires mutex lock, checks if buffer has space
3. If buffer is full, producer waits on condition variable
4. Producer adds message to buffer, signals consumer, releases lock
5. Consumer acquires mutex lock, checks for available data
6. If buffer is empty, consumer waits on condition variable
7. Consumer processes data, signals producer, releases lock
8. Process continues until end of input

Educational Value:
- Critical section protection
- Deadlock prevention
- Producer-consumer workflow
- Condition variables for thread signaling
- Thread-safe data structures

Authors:
- Mansib Rahman
- Nina Luo