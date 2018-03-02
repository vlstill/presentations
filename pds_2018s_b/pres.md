---
vim: spell spelllang=en fo+=t tw=80
title: The C/C++ Concurrency
subtitle: "and Effective Stateless Model Checking for C/C++ Concurrency"
author:
    - presented by Vladimír Štill
header-includes:
    - \input{defs}
    - \institute{}
lang: english
date: 5th March 2018
...

## Concurrency in C and C++

### Pre C11/C++11

- no support for concurrency in the language and standard library
- no standard-defined behaviour for parallel programs

### C11/C++11

- standard defines behaviour of parallel programs
- standard library defines:
  - thread manipulation: threads, futures
  - synchronization: mutexes, condition variables, **atomics**

## C11 Variable Access Modes in Parallel Programs I

**not-atomic/unordered**

- cannot be used for synchronization
- two threads can access non-atomic variable at the same time and at least one
  of them modifies it -- *data race* $\rightarrow$ *undefined behaviour*

  . . .

- `volatile`{.c} does not change anything

## C11 Variable Access Modes in Parallel Programs II

```{.c}
#include <stdatomic.h> // <atomic> in C++
#include <pthread.h>
#include <assert.h>

_Atomic int x; // or atomic_int x;
               // std::atomic< int > x; in C++
void *worker1( void *_ ) {
    atomic_fetch_add( &x, 1 ); // atomic, synchronizes
    return NULL;               // C++: x.fetch_add( 1 );
}
int main() {
    pthread_t t1;
    pthread_create( &t1, NULL, worker1, NULL );
    ++x; // atomic, synchronizes
    pthread_join( t1, NULL );
    assert( x == 2 ); // OK
}
```

## C11 Variable Access Modes in Parallel Programs III

**atomic access**

- for variables declared with `atomic_*` type or with `_Atomic`{.c} qualifier
- by default all access is fully atomic
  - all accesses to all atomic variables have a single total order in each run
  - i.e. interleaving semantics

  . . .

- atomic ordering can be relaxed by specifying *memory order*

## Memory Model

- semantics of memory access
  - of hardware: `x86`/`x86_64`, ARM, POWER
  - of programming language: C11/C++11, Java, C#

- what primitives/instructions are supported (fences, CAS, RMW…)
- what synchronization is guaranteed
- what reordering can be observed

## C11 Memory Model

- designed to allow high-performance even of very relaxed hardware (POWER/ARM)
- different levels of synchronization: **memory order**

    ```{.cpp}
    x.fetch_add( 1, std::memory_order_relaxed ); // C++
    ```

- ranges from *almost no guarantees* to *interleaving semantics*

## C11 Memory Orders I

- **relaxed** -- only guarantees that operations *on the single location* are
  ordered
  - safe counters
  - termination indication (from signal handler, …)
  * can be reordered with other operations

## C11 Memory Orders II

- **release** -- for write operations
* **acquire** -- for read operations

* together ensure all previous writes will be visible when a release write is
  observed by an acquire store (simplified)
  - *previous* is anything happening control-flow-before in the writing thread

```{.cpp}
int x;
std::atomic< bool > f;

void t1() {
  x = 42;
  f.store( true, std::memory_order_release );
}
void t2() {
  while ( !f.load( std::memory_order_acquire ) ) { }
  assert( x == 42 ); // OK
}
```

## C11 Memory Orders III

- **acquire+release** -- for read-modify-write/compare-and-swap operations

* **sequential consistency** -- all SC operations are in a total order
  - the default
  - atomic in other programming languages are usually SC

. . .

- SC can be very expensive
- not all data structures need SC
  - acquire/release usually sufficient for queues
  - relaxed for just counting/flagging objects
- mutexes synchronize as SC

## The Two Main Approaches

### Operational-Semantics Based Analysis

- how program executes on an abstract machine?
- interleavings
- instruction reordering simulation
- tools: DIVINE, CBMC, Nidhugg, …


### Axiomatic-Semantic Based Analysis

- where is a read allowed to take value from?
- what reordering is allowed?
- executions graphs
- tools: Herd, RCMC, …

# Effective Stateless Model Checking\newline for C/C++ Concurrency\newline\newline\small\makebox[0.4\textwidth][c]{Michalis Kokologiannakis}\makebox[0.4\textwidth][c]{Ori Lahav}\newline\makebox[0.4\textwidth][c]{Konstantinos Sagonas}\makebox[0.4\textwidth][c]{Viktor Vafeiadis}

## Introduction

### Stateless Model Checking

- for parallel programs, often in real-world languages
- originally based on operations (interleaving) semantics
- explores state space
- does not store closed set

  . . .

- cannot handle non-terminating programs
- can explore some states repeatedly

. . .

### Dynamic Partial Order Reduction

- eliminates some traces, reduces redundant exploration
- usually based on some notion of equivalence of traces
- there are optimal techniques for certain equivalences

## Execution Graphs

the technique from the paper is based on execution graphs, not interleaving

- records memory operations
- values of writes
- origin of value for reads
- dependencies

\input{exg00}

- $sb$ = sequenced before; $rf$ = read-from
