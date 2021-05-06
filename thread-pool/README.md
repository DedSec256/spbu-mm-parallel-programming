Producers and consumers
======================

Requirements
-------------
The project was built and tested via:
- openjdk 11.0.10
- gradle 5.0

How to build
------------
```
gradle jar
```

The main artifact of the build is the `producers-consumers-{version}.jar` library that located in the `{repo_folder}/producers-consumers/build/libs` folder.

How to run tests
-----------------
```
gradle test
```

Currently, only one simple test is implemented: getting a task from the tasks list and execution of the extracted task.

Usage
-----
After building the project, use the following command to run the simulation with the number of producers and consumers specified (you need to run it after the build in the _build/libs_ folder).
```
java -jar producers-consumers-{version}.jar <number of producers> <number of consumers>
```

Then, press the `Enter` key in order to interrupt execution.
