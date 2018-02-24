---
vim: spell spelllang=en fo+=t tw=80
title: Lazy `x86`-TSO
subtitle: and why it is not done yet
author: Vladimír Štill
header-includes:
    - \newcommand{\redon}[2]{{\only<#1>{\color{red}}#2}}
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

- we focus on C and C++

  . . .

- program is translated into LLVM intermediate language

- LLVM is executed by the model checker

- exploration of all possible runs of the program

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

## Memory-Model-Aware Analysis -- My Approach I

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

## Memory-Model-Aware Analysis -- My Approach II

**program transformation**

- can be improved with static analysis
- memory model independent

. . .

**memory operations**

- memory model dependent
- rather complex
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

1.  not all memory is actually accessible by more than one thread (*shared*)

    . . .

2.  not all shared memory is actually *accessed* by more that one thread

    . . .

3.  even memory accessed by more than one threads is usually not accessed by *all
    of them all the time*


. . .

- DIVINE's state space reduction uses these observations

  . . .

- but relaxed memory simulation has to be adapted to support this

## Lazy `x86`-TSO

- instead of flushing store buffers nondeterministically, we flush them only
  when needed

  - i.e. when someone tries to load given address
  - need to simulate all outcomes → nondeterminism in load

      . . .

  - how to handle other entries in store buffer?

. . .

- interaction with state-space reduction
  - store buffers are shared memory for reduction
  - need to ensure reduction does not see every operation as visible

    . . .

  - hide what happens inside store buffers from reduction

    . . .

  - corner cases: barriers, full store buffers


<!--
## Lazy `x86`-TSO {.t}

\vspace*{-1.8em}
\includegraphics[scale=0.34]{lazy}
-->
