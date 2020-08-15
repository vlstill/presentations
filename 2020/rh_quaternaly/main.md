# Finding bugs in parallel programs with heavy-duty program analysis

Anyone who has ever tried to write a piece of parallel software knows it is not
an easy task.
Parallelism promises to make programs faster, yet it also opens many new
pitfalls and makes testing programs much harder.
For example, writing and reading a variable in parallel from two threads may
seem to work, but will backfire sooner or later unless the variable is somehow
protected -- for example by a lock or by using some language primitives to make
the accesses atomic.
The worst part about these problems is that they often manifest themselves only
under specific timing of the involved threads or on a particular platform.
Therefore, a program might run smoothly most of the time, but might fail once a
few minutes or even once a few months.
A service failing once a few months on customer's server is surely among
developers' nightmares.
Knowing this problem exists, can we help developers find problems in parallel
software?
We believe heavy-duty program analysis is one option which offers some help.

## The problem

To meaningfully use parallelism, threads of a parallel program often need to
communicate.
In this case, it is the responsibility of programmers that all of the
communicating threads have a consistent view of the memory and therefore can
work correctly.
For example, consider an addition to a doubly-linked list.
The thread that performs the addition will create a new node, set its value, and
set the pointers to the previous and next node inside the node-to-be-added.
So far no problem can happen in parallel execution -- the new node is not yet
visible to the other threads, it is not linked into the list.
However, consider that at this point another thread performs insertion to the
same place in the linked list.
Now the list has changed, but the pointers in the node-to-be-added do not
reflect this change.
If the node is inserted anyway, we will get an inconsistent list -- we might
miss some of its elements when iterating over it, or even find different
elements when iterating forward and when iterating backwards.
The root cause of this problem is that the addition of a node to the linked list
is not an atomic operation, that is, it can be completed partially, and other
threads might be affected by this partial result.

There is no one solution to our example problem.
It might be possible to avoid accessing the list from multiple threads altogether.
It might also be sufficient to add a lock that protects accesses to the list or
make the list use locks internally.
Such locks will prevent two threads from inserting at once into the same list.
It might also be necessary to design the whole program in such a way it can use
some of high-performance lock-free data structures for the communication.
In all but the first case, we are entering the realm of parallel programming, and
we need to consider all its implications and risks.

One of the significant difficulties with parallel programs is that they are hard
to test.
This problem is caused by their dependence on timing -- for example, our linked
list example might work just fine if it so happened that the insertions are
never executed at the same time.
For this reason, tests might not fail for a buggy program, or they might fail
only sometimes, making debugging harder.
Therefore, there is a need for tools that can help the with testing of parallel
programs.

## Heavy-duty program analysis

Many techniques that can improve testing of parallel programs were introduced,
ranging from relatively fast code analysis techniques similar to compiler
warnings, going to more involved like various thread sanitisers that can detect
unsynchronised access to the same memory region from multiple threads, and
finally to heavy-duty analyses that essentially explore all possible executions
of the parallel program.
While the first two categories are relatively simple to use by developers,
heavy-duty tools are still mostly academical projects which come with specific
limitations.
However, we believe their promises should not be ignored, even if they are far
from silver bullets.

Heavy-duty tools promise to check that the program does not do anything wrong
(e.g., trigger an assert, access freed memory) regardless of the timing of
threads.
They can also take into account more advanced behaviour of the system, for
example, relaxed memory present in most contemporary processors.
Heavy-duty tools do this using many different basic techniques (e.g., stateless
model checking, explicit-state model checking, symbolic model checking, symbolic
execution), but in essence, they explore all the possibilities for the timing of
threads that can lead to different outcomes.
The main difference between these techniques is in their basic approach to the
complexity of the analysis.
Therefore, each of these techniques comes with their limitations, and it is
important to keep in mind that they are trying to solve a problem that is
provably not solvable for every case.
There will always be programs for which tools will fail or require much more
resources than available.
There are also similar tools that can explore all behaviours of the program
based on all its possible inputs, and tools that combine both capabilities.

## The DIVINE analyser

DIVINE is one of the heavy-duty analysis tools that can be applied to parallel
programs.
It is developed in the Parallel and Distributed Systems Laboratory at Faculty of
Informatics on Masaryk University in Brno, Czech Republic.
DIVINE specialises on analysis of programs written in C++ (and C) and can
handle both sequential and parallel programs.
It can detect various bugs including assertion violations, access out of memory
bounds or to freed memory, use of uninitialised memory, and memory leaks.
To run DIVINE, it needs a test and it can only detect problems that can happen
in *some execution* of the test -- i.e., it can try all possible ways timing of
threads can influence run of the test.
This enables an entirely new way of writing tests of parallel programs -- for
example, instead of having to exercise the data structure with millions of
elements inserted from several threads in the hope of triggering an elusive bug,
it is sufficient to try it only with a few elements from two or three threads.
DIVINE's ability to explore all possible outcomes will mean such a test is
sufficient (provided it exercises all features of the data structure, for
example, growing of a thread-safe hashset).
Indeed, due to the computational complexity of program analysis, it is desirable
to write tests for DIVINE as small as possible.

We will now look at some interesting recent additions to DIVINE with regards to
parallel programs.

### Relaxed Memory[^rmm]

In the struggle to construct more and more powerful processors, the processor
designers sometimes make decisions that make programmers' lives harder.
One of them is the use of relaxed memory to speed up memory access -- processors
use caches, out-of-order execution, and speculative execution to mask the
latency of the main memory.
On most architectures, the presence of these mechanisms is observable by
parallel programs.
Maybe you have heard about the Meltdown and Spectre security vulnerabilities --
they are caused by the same mechanisms that result in relaxed memory.
However, while Meltdown and Spectre affect the security of programs and operating
systems, relaxed memory affects only parallel programs, but can cause them to
crash or produce incorrect results.

Relaxed memory manifests itself differently on different hardware platforms, for
the sake of simplicity, we will consider the x86-64 processors manufactured by
Intel and AMD.
These processors power most of the modern laptops, desktops, and servers (other
processors, e.g., high-performance ARM, are often even more relaxed).
On an x86-64 processor, when a program stores data to a certain memory location,
the processor does not wait for the store to finish before it executes next
instruction.
Instead, it saves the stored value and its address internally into a *store
buffer* that holds it until the store is committed to the memory.
If the same CPU core that saved it reads the given location, it will get the
value from the store buffer -- therefore, single-threaded programs behave as
expected.
However, if the same location is accessed by another thread running on other
core while the store is in the store buffer, the new value is not yet visible to
the other thread.
This can lead to very unintuitive behaviour.
For example, consider a very simple program with two threads $T_1$ and $T_2$ and
two global variables $x$ and $y$ (initialized to 0).
Thread $T_1$ performs two actions -- it assigns 1 to $x$ ($x ← 1$) and reads $y$
(read $y$).
Thread $T_2$ has the variables switched -- it performs $y ← 1$ and read $y$.
The question is if both reads can read 0.
If we tried simulating these threads, we would probably conclude this cannot
happen -- at least one of the assignments has to happen before both of the
reads, and therefore at least one of the variables has to be 1.
However, thanks to the store buffers, both reads can return 0 on x86-64.
For example, we can first execute all actions of $T_1$: $x ← 1$; read $y$,
clearly, the read returns 0.
However, on x86-64, it is now possible the value 1 for variable $x$ is not yet
saved in the memory but instead is in the store buffer corresponding to $T_1$.
Therefore the memory still contains value 0 for $x$.
Now we execute $T_2$: $y ← 1$; read $x$.
Unless the store $x ← 1$ was already propagated to the memory, the read would
return 0 -- a value which seem to be impossible from the inspection of the
program.
**TODO: image?**

Programmes have to use some sort of synchronisation to to prevent relaxed
behaviour from breaking their programs.
One option is to use locks -- locks not only prevent two parts of the program
that use the same lock from running at the same time, but they also ensure that
all modifications performed before a lock is released will be visible after it
is acquired, even if each operation happens in different threads.
However, locks can slow the program significantly, especially if they are used
for long stretches of code or very often.
An alternative approach is to use atomic accesses provided by the platform or
programming language -- these are faster than locks, but slower than
unsynchronised access and can only operate on a single memory area of limited
size (e.g., 8 bytes on 64-bit platforms).
Atomic accesses are often used to implement high-performance thread-safe data
structures that can be accessed from many threads at once.

Especially if the programmer chooses to use atomic accesses, they will have to
consider the possible ordering of events very carefully and always keep in mind
it does not behave as expected.
Furthermore, testing program behaviour under relaxed memory is especially hard.
Not only we have all the problems already mentioned for parallel programs, but
an improperly synchronised program can also be very susceptible to minor
modifications.
For example, a tool that tracks memory accesses to detect unsynchronised
parallel access can also easily mask relaxed behaviour and related errors.

In 2018, we published an extension of DIVINE that allows it to analyse
programs running under the x86-TSO memory model that describes relaxed behaviour
of x86-64 processors and therefore should encompass behaviour of both current
and future x86-64 processors.
With this extension, DIVINE can find bugs caused by relaxed behaviour that would
manifest on these processors.
While DIVINE is undoubtedly not the only tool that can analyse programs running
under relaxed memory, we have shown that its performance is comparable to the
best tools that handle x86-TSO and that each kind of tool seems to have
different strengths and weaknesses (it can handle different kinds of programs
well).
We believe that DIVINE, with its wide range of bugs it can detect and with good
support for C++, can be a useful tool for analysis of thread-safe data
structures.

[^rmm]: Based on publication "Model Checking of C++ Programs Under the x86-TSO
Memory Model" by Vladimír Štill and Jiří Barnat
(<http://dx.doi.org/10.1007/978-3-030-02450-5_8a>,
<https://divine.fi.muni.cz/2018/x86tso>)

### Detecting nonterminating parts of programs[^lnterm]

Another interesting problem in parallel software is termination.
It often happens that one action waits until other finishes, for example, getting
an element from thread-safe queue might wait for an element to become available
or entering a critical section must wait until another thread leaves it.
Furthermore, we are often not interested in termination of the whole program --
it might be a daemon or server (or its event loop in the test).

Interestingly, research in this area is much less intensive than in the case of
relaxed memory.
Most approaches to termination focus on sequential programs, on specialised
modelling languages for parallel programs.
Even the approaches that target parallel programs in realistic programming
languages are often focused on  the termination of the whole programs.

In 2019, we published a paper about an extension of DIVINE that allows it
to find nonterminating parts of programs.
To know which parts of the program must terminate, we use the notion of
*resource sections* -- essentially a piece of code with specified entry and exit
point, for example, a function or its part.
With these resource sections marked in the program, the extended DIVINE can
check that the program cannot get stuck inside any of these resource sections.
Some of the resource sections can be marked automatically by DIVINE.
These include waiting from locks, critical sections of locks, waiting for
condition variables and waiting for thread joins.
The user is also able to insert new resource sections in their code by simple
annotations, for example, an author of a thread-safe queue with blocking dequeue
operation might want to mark it as a resource section.



[^lnterm]: Based on publication "Local Nontermination Detection for Parallel C++
Programs" by Vladimír Štill and Jiří Barnat
(<http://dx.doi.org/10.1007/978-3-030-30446-1_20>,
<https://divine.fi.muni.cz/2019/lnterm>)


