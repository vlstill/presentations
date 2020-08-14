# Preventing bugs in parallel programs with heavy-duty program analysis

Anyone who have ever tried to write a piece of parallel software knows it is not an easy task.
Parallelism promises to make programs faster, yet it also opens many new pitfals and makes testing programs much harder.
For example, writing and reading a variable in parallel from two threads may seem to work, but will backfire sooner or later unless the variable is somehow protected -- for example by a lock or by using some language primitives to make the accesses atomic.
The worst part about these prolems is that they often manifest themselves only under certain timing of the involved threads.
Therefore a program might run smoothly most of the time, but might fail once a few minutes or even once a few months.
A service failing once a few months on customer's server is surely among developers' nightmares.
Knowing this problem exists, can we help developers find problems in parallel software?
We believe heavy-duty program analysis is one option which offers some help.

## The problem

In order to meaninfully use paralelism, threads of a parallel program usually need to communicate.
However, it is the responsibility of programmers that all of the communicating threads have a consistent view of the memory.
For example, consider an addition to a doubly-linked list.
The thread that performs the addition will create a new node, set its value and set the pointers to the previous and next node inside the node-to-be-added.
So far no problem can happen in parallel execution -- the new node is not yet visible to the other threads, it is not linked into the list.
However, consider that at this point another thread performs insertion to the same place in the linked list.
Now the list have changed, but the pointers in the node-to-be-added do not reflect this change.
If the node is inserted anyway, we will get and inconsistent list -- we might miss some of its elements when iterating over it, or even find different elements when iterating forward and when iterating backwards.
The root cause of this problem is that the addition of node to the linked list is not an atomic operation, that is, it can be completed partially and other theads might be affected by this partial result.

There is no one solution to our example problem.
It might be possible to avoid accessing the list from multiple thread alltogether.
It might also be sufficient to add a lock that protects accesses to the list, or make the list use locks internaly.
Such locks will prevent execution of insertion by two or more threads at once.
It might also be necessery to design the whole program in such a way it can use some of high-performace lock-free data structures for the communication.
In all but the first case, we are entering the realm of parallel programming and we need to consider all its implications and risks.

One of the major difficulties with parallel programs is that they are hard to test.
This problem is caused by their dependence on timing -- for example, our linked list example might work just fine if it so happened that the additions are never executed at the same time.
For this reason, tests might not fail for a buggy program, or they might fail only sometimes, making debugging harder.
Therefore, there is a great need for tools that can help with testing of parallel programs.

## Heavy-duty program analysis

Many techniques that can improve testing of parallel programs were introduced, ranging from relatively fast code analysis techniques similar to compiler warnings, going to more involved like various thread sanitisers that can detect unsynchronised access to the same memory region from multiple threads and finally to heavy-duty analyses that essentialy explore all possible executions of the parallel program.
While the first two categories are relatively simple to use by developers, heavy-duty tools are still mostly academical projects which come with specific limitations.
However, we believe their promises should not be ignored, even if they are far from silver bullets.

Heavy-duty tools promise to check that the program does not do anything wrong (e.g., trigger an assert, access freed memory) regardless of the timing of threads.
They do this using many different techniques (e.g., stateless model checking, explicit-state model checking, symbolic model checking, symbolic execution), but in essence they explore all the possibilities for timing of threads that can lead to different outcomes.
There are also similar tools that can explore all behaviours of the program based on all its possible inputs, and tools that combine both capabilities.

## …