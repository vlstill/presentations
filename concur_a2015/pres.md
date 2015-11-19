---
title: Transformations and Analysis of LLVM for Model Checking
titleshort: LLVM Transformations for Model Checking
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
lang: english
date: 20th November 2015
...

## Introduction

\begin{latex}
    \center
    \begin{tikzpicture}[>=stealth',shorten >=1pt,auto,node distance=4em,initial text=, ->]
        \tikzstyle{every state} = [rectangle, minimum size = 4.5em]
        \path[use as bounding box] (-1.5,0.8) rectangle (9,-6);

        \node[state] (c) at (0, 0) {\begin{minipage}{8em}\center
                \textbf{C/C++}\\
                with threads\\
                (.c/.cpp)
            \end{minipage}
        };
        \node[state] (llvm) at (0, -3) {\begin{minipage}{8em}\center
                \textbf{LLVM bitcode}\\
                (.bc)
            \end{minipage}
        };

        \node[state] (prop) at (6, 0) {\begin{minipage}{8em}\center
                \textbf{Property}\\
                assertion, memory safety, LTL
            \end{minipage}
        };

        \node[state] (divine) at (6, -3) {\begin{minipage}{8em}\center
                \textbf{DIVINE}\\
                model checker
            \end{minipage}
        };

        \node[state, minimum size = 1.5em]
        (ok) at (4.5, -5) {\color{paradisegreen}\textbf{OK}};
        \node[state, minimum size = 1.5em]
        (nok) at (7.5, -5) {\color{red}\textbf{Counterexample}};

        \path (c) edge (llvm);
        \node at (0.1,-1.5) [anchor=west] {\begin{minipage}{8em}
                clang\\+ DIVINE libs
            \end{minipage}};
        \path (llvm) edge (divine);
        \path (prop) edge (divine);
        \path (divine) edge (ok) edge (nok);

        \path<2->[onslide={<2> line width = 3pt}] (llvm) edge[loop below]
        node[align=center, text width=10em]{LLVM transformation and optimization} (llvm);
    \end{tikzpicture}

    \bigskip
    \makebox[\textwidth][c]{
        \only<1>{Model checking C++ with DIVINE}
        \only<2>{Adding LLVM-to-LLVM transformations by LART}
    }
\end{latex}

## LART

LLVM Abstraction and Refinement Tool

*   cannot do any abstraction or refinement (yet)

. . .

*   generic platform for LLVM-to-LLVM transformations
*   uses LLVM C++ API to run transformation and optimization passes

. . .

*   currently implements:
    *   weak memory models transformation (from MEMICS 2015)
    *   several simple parallel-safe optimizations
    *   transformations for SV-COMP

# SV-COMP

## SV-COMP, Concurrency Category

DIVINE is an explicit state model checker

*   not really well suited for SV-COMP
*   no smart handling of nondeterminism
    *   explicit enumeration of nondeterministic choice
*   exhaustive search of relevant interleavings
    *   but it uses smart reduction techniques to eliminate many
        uninteresting interleavings

. . .

however, in concurrency category of SV-COMP:

*   little or no nondeterminism
*   most programs all reasonably small

. . .

and DIVINE is quite fast and memory efficient!

## How to Verify SV-COMP Models with DIVINE?

**bold** = LART LLVM transformation

1.  compile model with `clang` using `divine compile`
    *   this compiles the model and C library functions, `pthreads`,…

. . .

2.  **add atomic sections to atomic functions**
    *   translation from SV-COMP atomic functions to DIVINE's equivalent
    *   add atomic masks to atomic functions

. . .

3.  **change all reads and writes of globals to be volatile**
    *   SV-COMP models often have undefined behavior

. . .

4.  run LLVM `opt` to optimize resulting LLVM bitcode (`-Oz`)

## How to Verify SV-COMP Models with DIVINE?

5.  **try to eliminate or constrain nondeterministic choices**
    *   bools are OK
    *   nondeterministic pointers can be either `NULL` or `1` (which is also not
        a valid pointer)
        *   we found no models in SV-COMP concurrency category where this causes bad answer
    *   for other types try to constrain the choice
        *   two common patterns recognised: cast to `bool` and modulo constant value
        *   more precise tracking could be done by bounded symbolic execution

. . .

6.  **disable `malloc` failure**
    *   in DIVINE every call to `malloc` can return `NULL` (nondeterminism)
    *   SV-COMP, however, works in idealized world where there is infinite
        amound of memory…

## How to Verify SV-COMP Models with DIVINE?

7.  run DIVINE, check only for assertions
    *   there are often other problems
        *   calls with wrong number of parameters
        *   missing return statements ($\rightarrow$ undefined value)
    *   use state space compression

. . .

**Results**

We expect to get more then $900$ points in concurrency.

*   time is the limiting factor

# Weak Memory Models

## Weak Memory Models

*   the order of reads and writes in the code does not need to match the order of their execution
    *   compiler optimizations
    *   out-of-order execution, cache hierarchy

. . .

*   it is hard to reason about memory models
*   parallelism is hard even under **Sequential Consistency**
    *   reads and writes are immediate and cannot be reordered
    *   not realistic, expensive to enforce

. . .

*   newer revisions of C/C++ have support for specifying atomic variables with
    memory access ordering
    *   same for Java, LLVM bitcode,…

. . .

*   verifiers often assume sequential consistency
    *   so does DIVINE

## Total Store Order

*   similar to the memory model used by x86_64
*   the order of execution of stores is guaranteed to match their order in
    machine code
    *   compiler might still reorder stores
*   independent loads can be reordered

## Weak Memory Models {.fragile}

\begin{latex}
\begin{lstlisting}[belowskip=0pt]
int x = 0, y = 0;
\end{lstlisting}

\begin{minipage}[t]{0.45\textwidth}
\begin{lstlisting}[numbers=left,aboveskip=0pt]
void thread0() {
    y = 1;
    cout << "x = " << x;
  }
\end{lstlisting}
\end{minipage}%
\hfill%
\begin{minipage}[t]{0.45\textwidth}
\begin{lstlisting}[numbers=left,aboveskip=0pt]
void thread1() {
    x = 1;
    cout << "y = " << y;
}
\end{lstlisting}
\end{minipage}
\end{latex}

\textbf{Total Store Order can be simulated using store buffers:}

\begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                   , semithick
                   , scale=0.65
                   ]
  \draw [-] (-10,0) -- (-6,0) -- (-6,-2) -- (-10,-2) -- (-10,0);
  \draw [-] (-10,-1) -- (-6,-1);
  \draw [-] (-8,0) -- (-8,-2);
  \node () [anchor=west] at (-10,0.5) {main memory};
  \node () [anchor=west] at (-10,-0.5)  {\texttt{0x04}};
  \node () [anchor=west] at (-8,-0.5)  {\texttt{0x08}};
  \node () [anchor=west] at (-10,-1.5)  {\texttt{x = 0}};
  \node () [anchor=west] at (-8,-1.5)  {\texttt{y = 0}};

  \node () [anchor=west] at (-10,-3.5) {store buffer for thread 0};
  \node () [anchor=west] at (0,-3.5) {store buffer for thread 1};

  \draw [-] (-10,-4) -- (-4, -4) -- (-4,-5) -- (-10,-5) -- (-10,-4);
  \draw [-] (0,-4) -- (6, -4) -- (6,-5) -- (0,-5) -- (0,-4);
  \draw [-] (-8,-4) -- (-8,-5);
  \draw [-] (-6,-4) -- (-6,-5);
  \draw [-] (2,-4) -- (2,-5);
  \draw [-] (4,-4) -- (4,-5);

  \node<2-> () [anchor=west] at (-10,-4.5)  {\texttt{0x08}};
  \node<2-> () [anchor=west] at (-8,-4.5)  {\texttt{1}};
  \node<2-> () [anchor=west] at (-6,-4.5)  {\texttt{32}};

  \node<4-> () [anchor=west] at (0,-4.5)  {\texttt{0x04}};
  \node<4-> () [anchor=west] at (2,-4.5)  {\texttt{1}};
  \node<4-> () [anchor=west] at (4,-4.5)  {\texttt{32}};

  \node () [] at (-4, 0.5) {thread 0};
  \draw [->] (-4,0) -- (-4,-2);
  \node () [anchor=west, onslide={<2> font=\bf, color=red}] at (-3.5, -0.5) {\texttt{store y 1;}};
  \node () [anchor=west, onslide={<3> font=\bf, color=red}] at (-3.5, -1.5) {\texttt{load x;}};

  \node () [] at (2, 0.5) {thread 1};
  \draw [->] (2,0) -- (2,-2);
  \node () [anchor=west, onslide={<4> font=\bf, color=red}] at (2.5, -0.5) {\texttt{store x 1;}};
  \node () [anchor=west, onslide={<5> font=\bf, color=red}] at (2.5, -1.5) {\texttt{load y;}};

  \draw<2-> [->, dashed] (-0.5,-0.5) to[in=0, out=0] (-4,-4.5);
  \draw<3-> [->, dashed] (-9,-2) to[in=0, out=-90, out looseness=0.7] (-1.3,-1.5);
  \draw<4-> [->, dashed] (5.5,-0.5) to[in=0, out=0] (6,-4.5);
  \draw<5-> [->, dashed] (-7,-2) to[in=0, out=-90, out looseness=0.5] (4.7,-1.5);

\end{tikzpicture}

## TSO Under-approximation in DIVINE

*   we use bounded store buffer to under-approximate TSO

*   TSO simulation is implemented as an LLVM-to-LLVM transformation
    *   no need to change DIVINE to use weak memory models
    *   no need to change verified source code
    *   store buffer size can be configured in the transformation

## The Transformation to TSO

*   every `load`, `store` and memory intrinsic[^mi] is replaced by function
    which simulates TSO

*   for each thread of the original program, a thread which flushes its store
    buffer is added

*   for an atomic instruction, the store buffer is first flushed and then the
    instruction is executed without modification

[^mi]: `llvm.memcpy`, `llvm.memmove`, `llvm.memset`

## Improvements Over MEMICS Version

*   store buffer can be bypassed for load of thread-local memory location
    *   the thread-locality is recognized dynamically by DIVINE

*   manipulations with local variables whose address is never taken are not
    transformed
    *   saves runtime check and entering atomic section (which can cause state
        to be produced)

*   also other optimizations (later) help

## Improvements Over MEMICS Version

| model        | MEMICS  | $+$ load private       | $+$ local | SC
|--------------|---------|------------------------|-----------|----
| `fifo-1`     | $44$ M  | $5.6$ M ($7.9\times$)  |           | $7$ K
| `fifo-2`     | $338$ M | $51$ M ($6.6\times$)   | | $7$ K
| `fifo-3`     | $672$ M | $51$ M ($13\times$)    | | $7$ K
| `simple-1`   | $538$ K | $19$ K ($28\times$)    | | 251
| `peterson-2` | $103$ K | $40$ K ($2.6\times$)   | | $1.4$ K
| `pt_mutex-2` | $1.6$ M | $12$ K ($135\times$)   |  | 98

# Parallel Safe Optimizations

## Parallel Safe Optimizations -- Motivation

```{.cpp}
int x = 0;
void *foo( void *_ ) {
    x = 1;
    assert( x == 1 );
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create( &thread, NULL, &foo, NULL );
    x = 2;
    pthread_join( thread, NULL );
}
```

Can the assertion be triggered?

## Parallel Safe Optimizations -- Motivation

LLVM s `-O0`:

```{.llvm}
define i8* @foo(i8* %_) #0 {
entry:
  %_.addr = alloca i8*
  store i8* %_, i8** %_.addr
  store i32 1, i32* @x
  %0 = load i32, i32* @x
  %tobool = icmp ne i32 %0, 0
  %conv = zext i1 %tobool to i32
  %cmp = icmp eq i32 %conv, 1
  %conv1 = zext i1 %cmp to i32
  call void @__divine_assert(i32 %conv1)
  ret i8* null
}
```

## Parallel Safe Optimizations -- Motivation

LLVM s `-O2`:

```{.llvm}
define noalias i8* @foo(i8* nocapture readnone %_) #0 {
entry:
  store i32 1, i32* @x, align 4, !tbaa !1
  tail call void @__divine_assert(i32 1) #2
  ret i8* null
}
```

. . .

*   optimization changed behavior of the program
*   `x` is not set to be `volatile`, or atomic!

## Parallel Safe Optimizations

*   standard compiler optimizations do not preserve parallel behavior of the
    program
    *   they also do not preserve readability of debugging information
    *   variables can be promoted to registers, function inlining, code
        movement, cycle invariant code motion

*   optimizations can increase state space size

*   it is desirable to verify code with the optimizations intended by user of
    verification tool
    *   both for usability of results and readability of counterexamples

*   but optimizations can help verifier to run faster, with less memory

. . .

*   design optimizations which cannot change verified properties
    *   safety, non-stuttering LTL
*   can tightly cooperate with DIVINE

## Examples of Parallel Safe Optimizations

**Branch Elimination**

*   if branch destination has only one predecessor and branch is not
    conditional, or condition is constant expression, branch can be eliminated
*   clang actually emits LLVM with such branches in debug mode

## Examples of Parallel Safe Optimizations

**Constant Alloca Elimintation**

*   local variables in LLVM -- results of `alloca` instruction
    *   memory for a variable is allocated (on stack) so that it can be passed by
        referrence
    *   often the variable is neither modified nor accessed through pointer

*   if variable is constant and does not escape the functions it can be
    eliminated
    *   use of any load from `alloca` is replaced by value which was originally
        stored into it

*   compared to LLVM's register promotion this does not add registers

## Future

**Atomic Cycles**

*   if a cycle can be proven to terminate and not perform any visible action it
    can be executed inside atomic section
*   without the atomic section DIVINE emits state after each iteration
*   static or dynamic detection of visibility

*   need to employ termination detection

## Future

**Silent loads/stores**

*   DIVINE's notion of `load`/`store` visibility based on pointer tracking
*   a value might be reachable by pointers from more then one thread, but might
    be accessed only by one
*   mark `load`/`store` as silent to avoid visibility checking
*   could also improve generation speed
*   requires good pointer analysis

**Slicing**

*   hard to preserve all properties (memory safety,…)
*   maybe as an approximation

## Future

**Symbolic Data**

*   DIVINE and SymDIVINE will be merged
*   symbolic data manipulation compiled in LLVM
*   state comparison using SMT in DIVINE

*   again, we will need good pointer analysis for parallel code


