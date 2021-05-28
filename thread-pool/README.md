Thread pool
======================

Requirements
-------------
The project was built and tested via:
- openjdk 11.0.10
- gradle 5.0

How to run tests
-----------------
```
gradle test
```

List of tests:
1. ThreadPool creation and verification of the created threads
2. One task submit and verification of the result
3. Multiple tasks submit (50 tasks, 5 thread by default)
4. One continueWith call
5. Multiple continueWith calls
6. ThreadPool threads release during task execution
