---
vim: spell spelllang=en fo+=t tw=80
title: "Lazy `x86`-TSO Memory Model\\newline for C++ Verification"
author: Vladimír Štill
header-includes:
    - \input{defs}
lang: english
date: 26th February 2018
...

## Verification of Parallel Programs I

- design of parallel programs is hard
- easy to make mistakes -- data races, deadlocks

  . . .

- **memory behaviour is very complex**

  - effects of caches, out-of-order and speculative execution

  . . .

```{.cpp}
int x, y = 0;
void thread0() {
    y = 1;
    int a = x;
}

void thread1() {
    x = 1;
    int b = y;
}
```

- is it possible to end with `a == 0 && b == 0`? \pause{} **yes**

##  Verification of Parallel Programs II

- C and C++

  . . .

- program is translated into LLVM intermediate language

- LLVM is executed by the model checker

- exploration of \mbox{\only<-2>{all possible}\only<3->{\sout{all possible}}} runs of the program

  . . .

  - actually of some representants of classes of equivalent runs

  . . .

- detect assertions, memory errors, compiler traps, …

## Relaxed Memory Example

```{.cpp}
int x, y = 0;
void thread0() {              void thread1() {
    y = 1;                        x = 1;
    int a = x;                    int b = y;
}                                 int c = x;
                              }

```

. . .

\begin{latex}
    \makebox[\textwidth][c]{
    \begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                       , semithick
                       , scale=0.65
                       ]

      \useasboundingbox (-10,1) (7.3,-6);

      \draw [-] (-10,0) rectangle (-7,-6);
      \draw [-] (-10,-1) -- (-7,-1)
                (-10,-2) -- (-7,-2)
                (-10,-3) -- (-7,-3)
                (-10,-4) -- (-7,-4)
                (-10,-5) -- (-7,-5);
      \draw [-] (-9,0) -- (-9,-6);
      \node () [anchor=west] at (-10,0.5) {memory};
      \node () [anchor=west] at (-10,-2.5)  {\texttt{x}};
      \node () [anchor=west] at (-9,-2.5) {\only<-8>{\texttt{0}}\only<9->{\texttt{1}}};

      \node () [anchor=west] at (-10,-3.5)  {\texttt{y}};
      \node () [anchor=west] at (-9,-3.5)  {\texttt{\only<-9>0\only<10->1}};

      \node () [anchor=center] at (-2,-5.5) {store buffer of t. 0};
      \node () [anchor=center] at (4,-5.5) {store buffer of t. 1};

      \draw [-] (-4,-4) rectangle (0,-5);
      \draw [-] (2,-4) rectangle (6,-5);
      \draw [-] (-2,-4) -- (-2,-5);
      \draw [-] (4,-4) -- (4,-5);

      \node<3-9> () [anchor=west] at (-4,-4.5)  {\texttt{y}};
      \node<3-9> () [anchor=west] at (-2,-4.5)  {\texttt{1}};

      \node<5-8> () [anchor=west] at (2,-4.5)  {\texttt{x}};
      \node<5-8> () [anchor=west] at (4,-4.5)  {\texttt{1}};

      \node () [] at (-4, 0.5) {thread 0};
      \draw [->] (-4,0) -- (-4,-2.3);
      \node () [anchor=west, onslide={<3> font=\bf, color=red}] at (-3.5, -0.5) {\texttt{y = 1;}};
      \node () [anchor=west, onslide={<4> font=\bf, color=red}] at (-3.5, -1.5) {\texttt{load x;}};

      \node () [] at (2, 0.5) {thread 1};
      \draw [->] (2,0) -- (2,-3.3);
      \node () [anchor=west, onslide={<5> font=\bf, color=red}] at (2.5, -0.5) {\texttt{x = 1;}};
      \node () [anchor=west, onslide={<6> font=\bf, color=red}] at (2.5, -1.5) {\texttt{load y;}};
      \node () [anchor=west, onslide={<7> font=\bf, color=red}] at (2.5, -2.5) {\texttt{load x;}};

      \draw<3-7> [->, dashed] (0.3,-0.5) to[in=0, out=0] (0,-4.5);
      \draw<4-7> [->, dashed] (-7,-2.5) to[in=0, out=0, out looseness = 3, in looseness=0.5] (-0.7,-1.5);
      \draw<5-7> [->, dashed] (6.3,-0.5) to[in=0, out=0] (6,-4.5);
      \draw<6-7> [->, dashed] (-7,-3.5) to[in=0, out=0, out looseness = 0.2, in looseness = 0.7] (5.3,-1.5);
      \draw<7-7> [->, dashed] (6,-4.5) to[in=0, out=0] (5.3,-2.5);

      \draw<-2> [->] (-4,-0.3) to (-3.4,-0.3);
      \draw<3> [->] (-4,-0.7) to (-3.4,-0.7);
      \draw<4-> [->] (-4,-1.7) to (-3.4,-1.7);

      \draw<-4> [->] (2,-0.3) to (2.6,-0.3);
      \draw<5> [->] (2,-0.7) to (2.6,-0.7);
      \draw<6> [->] (2,-1.7) to (2.6,-1.7);
      \draw<7-> [->] (2,-2.7) to (2.6,-2.7);
  \end{tikzpicture}
  }
\end{latex}

## Why Relaxed Memory?

- memory is significantly slower than processor cores
- processor has caches to speed up execution

  . . .

- optimizations of cache coherency protocols\
  $\rightarrow$ observable effects

  . . .

- reordering of instructions might be also observable (not on `x86`)

  . . .

- overall behaviour described by a **(relaxed) memory model**

. . .

- now: `x86`-TSO memory model

  . . .

  - stores are performed to store buffer
  - core-local FIFO buffers
  - entries flushed eventually to the memory

## Memory-Model-Aware Analysis I

- encode the memory model into the program
- verify it using a verifier without memory model support
  - e.g. DIVINE, a lot of other verifiers
  - program transformation instead of modification of the verifier

. . .

```{.cpp}
x = 1;
int a = y;
```

$\rightsquigarrow$

```{.cpp}
_store( &x, 1 );
int a = _load( &y );
```

- `_load`, `_store` simulate the memory model
- (more complex in practice)

## Memory-Model-Aware Analysis II

**program transformation**

- can be improved with static analysis
- memory model independent
- most complexity is technical

. . .

\bigskip

**memory operations**

- memory model dependent
- rather complex (theoretically & technically)
- impact efficiency a lot $\rightarrow$ the main aim of my work
  - efficient data structures (time & memory)
  - amount of nondeterminism
- bounded reordering of (effects of) instructions

## State Space Explosion {.t}

\begin{latex}
\vspace*{-1.8em}%
\includegraphics[width=0.85\textwidth]{explosion}

\vspace*{-23em}
\visible<2->{
    \hspace*{17em}
    \begin{minipage}{0.4\textwidth}
    flushes not needed if noone reads given value
    \end{minipage}}
\end{latex}

## Critical Observations

#.  not all memory is actually accessible by more than one thread (*shared*)

    . . .

#.  not all shared memory is actually *accessed* by more that one thread

    . . .

#.  even memory accessed by more than one threads is usually not accessed by *all
    of them all the time*


. . .

- DIVINE's state space reduction uses these observations

  . . .

- but relaxed memory simulation has to be adapted to support this

## Lazy `x86`-TSO

instead of flushing store buffers nondeterministically, flush them only when
needed

- i.e. when someone tries to load given address
- need to simulate all outcomes → nondeterminism in load

  . . .

- how to handle other entries in store buffer?

  . . .

- memory barriers and compare-and-swap/read-modify-write not fully lazy

  . . .

  - flushing of local store buffer can nondeterministically flush entries from
    other buffers
  - fully lazy barriers would show down DiOS

## Lazy `x86`-TSO -- Current Status I

the lazy simulation of `x86`-TSO store buffers mostly done

- one known missing corner case
  - in sequence of stores to unrelated addresses
  - solution will probably increase laziness and therefore performance

- probably some space for speed improvement

## Lazy `x86`-TSO -- Current Status II

current delays caused by interaction with state space reductions

  . . .

  - store buffers look like shared memory for reduction
  - ensure reduction does not see every operation as visible

    . . .

  - not everything in memory model implementation can be hidden -- "real"
    loads/stores

## Lazy `x86`-TSO -- Future

#. finish implementation
#. add more tests and benchmarks
#. compare with other tools
#. publish

    . . .

#. SV-COMP demo category?
#. $\text{(optimize \& improve \& publish)}^+$

. . .

\bigskip\bigskip\hfill That is all… Thank You!

<!--
## Lazy `x86`-TSO {.t}

\vspace*{-1.8em}
\includegraphics[scale=0.34]{lazy}
-->
