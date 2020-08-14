# Finding bugs in parallel programs with heavy-duty program analysis

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
They can also take into account more advanced behaviour of the system, for example relaxed memory present in most contemporary processors.
Heavy-duty tools do this using many different basic techniques (e.g., stateless model checking, explicit-state model checking, symbolic model checking, symbolic execution), but in essence they explore all the possibilities for timing of threads that can lead to different outcomes.
There are also similar tools that can explore all behaviours of the program based on all its possible inputs, and tools that combine both capabilities.

## The DIVINE analyser

DIVINE is one of the heavy-duty analysis tools that can be applied to parallel programs.
It is developed in the Parallel and Distributed Systems Laboratory at Facutly of Informatics on Masaryk University in Brno, Czech Republic.
DIVINE specialises on analysis of programs written in C++ (and C), and can handle both sequential and parallel programs.
It can detect various bugs including assertion violations, access out of memory bounds or to freed memory, use of uninitialized memory, and memory leaks.
In order to run DIVINE, it needs a test and it can only detect problems that can happen in *some execution* of the test -- i.e., it can try all possible ways timing of threads can influence run of the test.
This enables a completely new way of writing tests of parallel programs -- for example, instead of having to exercise the data structure with millions of elements inserted from a number of threads in hope of triggering an illusive bug, it is sufficient to try it only with a few elements from two or three threads, DIVINE's ability to explore all posible outcomes will mean such a test is sufficient.
Indeed, due to the computational complexity of program nanysisis, it is desirable to write tests for DIVINE as small as possible.

We will now look at some interesting additions to DIVINE with regards to parallel programs.

### Relaxed Memory[^rmm]

In the struggle to construct more and more powerful processors, the processor designers sometimes do decisions that make programmers' lives harder.
One of them is the use of relaxed memory to speed up memory access -- processors use caches, out-of-order and speculative execution to mask latency of the main memory and on most architectures the presence of these mechanism is observable by parallel programs.
Maybe you have heared about the Spectre security vulnerability -- it is caused by the same mechanism that result in relaxed memory.
However, while Spectre affects security of programs and operating system, relaxed memory affects only parallel programs, but can cause them to crash or produce incorrect results.

Relaxed memory manifests itself differently on different hardware, for the sake of simplicity, we will consider the x86-64 processors manufactured by Intel and AMD that power most of modern laptops, desktops, and servers (other processors, e.g., high-performance ARM, are often even more relaxed).
On x86-64 processors when a program stores data to a certain memory location, the processor does not wait for the store to finish before it executes other instructions.
Instead, it saves the stored value and its address internally into a *store buffer* that holds it until the store is commited to the memory.
If the same thread later reads given location, it will get the value from the store buffer -- therefore, single-threaded programs behave as expected.
However, if the same location is accessed by another thread while the store is in the store buffer, the new value is not yet visible to the other thread.
This can lead to high unintuitive behaviour.
For example, consider a very simple program with two threads $T_1$ and $T_2$ and two global variables $x$ and $y$ (initialized to 0).
Thread $T_1$ performs two actions -- it assigns 1 to $x$ ($x ← 1$) and reads $y$ (read $y$).
Thread $T_2$ has the variables switched -- it performs $y ← 1$ and read $y$.
The question is if it can happen that both reads read 0.
If we tried simulating these threads, we would probably conclude it cannot -- at least one of the assignments has to happen before both of the reads and therefore at least one of the variables has to be 1.
However, thanks to the store buffers, both reads can return 0 on x86-64.
For example, we can first execute all actions of $T_1$: $x ← 1$; read $y$, clearly, the read returns 0.
However, on x86-64 it is now possible the value 1 for variable $x$ is not yet saved in the memory but instead is in the store buffer corresponding to $T_1$ and therefore the memory still contains value 0 for $x$.
Now we execute $T_2$: $y ← 1$; read $x$.
Unless the store $x ← 1$ was already propagated to the memory, the read will return 0 -- a value which seem to be imposible from the inspection of the program.
**TODO: image?**

To prevent the relaxed behaviour from breaking their programs, programmes have to use some sort of synchronization.
One options is to use locks -- locks not only prevent two parts of the program that use the same lock from running at the same time, they also ensure that all modifications performed before a lock is released will be visible after it is acquired, even if each operation happens in different threads.
However, locks can slow the program significantly, especially if they are used for long stretches of code or very often.
An alternative approach is to use atomic accesses provided by the platform or programming language -- these are faster then locks, but slower that unsynchronized access and can only operate on a single memory location of limited size (e.g., 8 bytes on 64 bit platforms).
Atomic accesses are often used to implement high-performance thread-safe data structures that can be accessed from many threads at once.

Especially if the programmer chooses to use atomic accesses, they will have to consider possible ordering of events very carefully and always keep in mind it does not behave as expected.
Furthemore, testing program behaviour under relaxed memory is especially hard -- not only we have all the problems already mentioned for parallel programs, but the program can be also very sensitive to minor modifications -- for example to tool that tracks memory accesses to detect unsynchronized parallel access can also easily mask relaxed behaviour.

In 2018, we have published an extension of DIVINE that allows it to analyse programs running under the x86-TSO memory model that describes relaxed behaviour of x86-64 processors.
With this extension, DIVINE can find bugs caused by relaxed behaviour that would manifest on these processors.
While DIVINE is surely not the only tool that can analyse programs running under relaxed memory, we have shown that its performace is comparable to the best tools that handle x86-TSO and that each kind of tool seems to have different strenghts and weaknesses (it can handle different kinds of programs well).
We believe that DIVINE, with its wide range of bugs it can detect and with good support for C++, can be a useful tool for analysis of thread-safe data structures.

[^rmm]: Based on publication "Model Checking of C++ Programs Under the x86-TSO Memory Model" by Vladimír Štill and Jiří Barnat (http://dx.doi.org/10.1007/978-3-030-02450-5_8)

### Detecting nonterminating parts of programs[^lnterm]

[^lnterm]: Based on publication "Local Nontermination Detection for Parallel C++ Programs" by Vladimír Štill and Jiří Barnat (http://dx.doi.org/10.1007/978-3-030-30446-1_20)

## Using DIVINE to find bugs in parallel program

The simplest case is that one has a component of a program written in C or C++ and a test for it.
Such a component might be for example a data structure that allows access from multiple threads at the same time.
The component, its tests, and all its dependencies must be compiled by DIVINE, therefore, all this code must be written in C or C++.
This requirement has two reasons: first, DIVINE can only analyse test code it can execute itself, therefore, it has to have access to all components needed by the test.
Second, DIVINE works neither on the level of compiled binaries, nor on the level of source language.
Instead it uses LLVM IR -- an intermediate representation used also by the clang compiler.
The advantage of LLVM IR is that is is significantly simpler to handle by an analysis tool than either machine code or a programming language.
Therefore, DIVINE has to compile the component, its test and their dependencies to LLVM.

Once the components are compiled, DIVINE can analyse them.
This can be rather time- and resource-intensive task -- while DIVINE uses several optimizations to limit the 

### 

## …