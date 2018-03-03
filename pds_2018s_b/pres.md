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
- originally based on operational (interleaving) semantics
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

## Execution Graphs I

the technique from the paper is based on execution graphs, not interleaving

- records memory operations
- values of writes
- origin of value for reads
- dependencies

\input{exg00}

- $sb$ = sequenced before; $rf$ = read-from

## Execution Graphs II

not all execution graphs are valid = *consistent*

\bigskip\input{exg01}

## Execution Graph Generation {.t}

```{.cpp}
a = x;   ||   x = 1;
b = x;   ||
```

\begin{tikzpicture}[>=latex,line join=bevel,node distance = 7em]

  \draw[draw=none, use as bounding box] (-7em,0.7em) rectangle (7em, -13em);

  \node (w0) at (0,0) {$W(x, 0)$};
  \node<3-> (r0) [below left of = w0] {$R(x)$};
  \node<4-7,9-> (r1) [below of = r0] {$R(x)$};
  \node<5-> (w1) [below right of = w0] {$W(x, 1)$};

  \definecolor{green}{rgb}{0.0,0.6,0.0};
  \draw<3-> [->] (w0) edge[bend right] node[left]{$sb$} (r0);
  \draw<3-7,10> [->, green, dashed] (w0) edge node[black, left]{$rf$} (r0);

  \draw<4-7> [->, green, dashed] (w0) edge node[black, left]{$rf$} (r1);

  \draw<4-7,9-> [->] (r0) edge node[left]{sb} (r1);
  \draw<5-> [->] (w0) edge[bend left] node[right]{$sb$} (w1);

  \draw<8-9> [->, green, dashed] (w1) edge node[black, above]{$rf$} (r0);
  \draw<9-> [->, green, dashed] (w1) edge node[black, above]{$rf$} (r1);
\end{tikzpicture}

- \showon{1}{initialization}
  \showon{2}{in what order should actions be added?}
  \showon{3}{add \texttt{a = x}}
  \showon{4}{add \texttt{b = x}}
  \showon{5}{add \texttt{x = 1}}
  \showon{6}{explore again adding \texttt{x = 1} first?}
  \showon{7}{no… revisit reads}
  \showon{8-9}{option 1: revisit \texttt{a = x} \showon{9}{+ add \texttt{b = x}}}
  \showon{10}{option 2: revisit \texttt{b = x}}

## Exploration Requirements

generate all consistent graphs and

#.  don't generate any inconsistent graphs
#.  don't generate any graph multiple times
#.  don't store generated graphs

## Revisitable Reads

- revisiting reads all the time causes redundant explorations
- only one instance of read needs to be revisitable

\makebox[\textwidth][c]{
\begin{tikzpicture}[>=latex,line join=bevel,node distance = 6em]
    \definecolor{green}{rgb}{0.0,0.6,0.0};

  \begin{scope}
    \node (w0) at (0,0) {$W(x, 0)$};
    \node (r0) [below left of = w0, fill = green!50!white] {$R(x)$};
    \node (w1) [below right of = w0, invisible] {$W(x, 1)$};

    \draw [->] (w0) edge[bend right] node[left]{$sb$} (r0);
    \draw [->, green, dashed] (w0) edge node[black, left]{$rf$} (r0);

    \node (s1) [draw = black, fit = {(w0) (r0) (w1)}] {};
  \end{scope}

    %%%%%%%%%%

  \begin{scope}[visible on=<2->]
    \node (w0n0) at (-8em, -9em) {$W(x, 0)$};
    \node (r0n0) [below left of = w0n0, fill = green!50!white] {$R(x)$};
    \node (w1n0) [below right of = w0n0] {$W(x, 1)$};

    \draw [->] (w0n0) edge[bend right] node[left]{$sb$} (r0n0);
    \draw [->, green, dashed] (w0n0) edge node[black, left]{$rf$} (r0n0);
    \draw [->] (w0n0) edge[bend left] node[right]{$sb$} (w1n0);

    \node (s1n0) [draw = black, fit = {(w0n0) (r0n0) (w1n0)}] {};

    \draw [->, densely dotted] (s1) edge node[left]{no revisit} (s1n0);
  \end{scope}

    %%%%%%%%%%

  \begin{scope}[visible on=<3->]
    \node (w0n1) at (8em, -9em) {$W(x, 0)$};
    \node (r0n1) [below left of = w0n1] {$R(x)$};
    \node (w1n1) [below right of = w0n1] {$W(x, 1)$};

    \draw [->] (w0n1) edge[bend right] node[left]{$sb$} (r0n1);
    \draw [->, green, dashed] (w1n1) edge node[black, above]{$rf$} (r0n1);
    \draw [->] (w0n1) edge[bend left] node[right]{$sb$} (w1n1);

    \node (s1n1) [draw = black, fit = {(w0n1) (r0n1) (w1n1)}] {};

    \draw [->, densely dotted] (s1) edge node[right]{revisit} (s1n1);
  \end{scope}

    %%%%%%%%%%
\end{tikzpicture}
}

## Efficient Execution Graph Generation

generate all consistent graphs and

#.  don't generate any inconsistent graphs
#.  don't generate any graph multiple times
#.  don't store generated graphs

\bigskip

**what is needed?**

- (non)revisitable reads

  . . .

- prefix closed execution graphs -- all prefixes of consistent EG are also
  consistent

  - not really for C11/C++11 standard's memory model!
  - paper uses *repaired C11 memory model*

    . . .

  - prefix closed in absence of RMW/CAS & SC atomics
  - special handling for these (can generate duplicities)

## Results

<http://plv.mpi-sws.org/rcmc/paper.pdf>
