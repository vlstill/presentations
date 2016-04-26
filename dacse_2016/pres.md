---
title: "Verification of Parallel C++ with DIVINE"
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
lang: english
date: 27th April 2016
aspectratio: 169
...

## Introduction

*   Why should we care about verification of (parallel) programs?

\bigskip

*   What is DIVINE and how it can help?

\bigskip

*   What is my work?

## Why Verification?

it is important to check that programs do what they are supposed to

*   testing is important part of software development
*   it can take up to 75 % of software development time

. . .

however testing has its downsides

*   it cannot proof absence of bugs
*   **not very efficient in problem discovery for parallel programs**

## Parallel Programs

Pallel programs are much harder to think about and debug

*   the timing of the execution can influence results
    *   *two therads writting to the same memory loacation*

*   results for the same input can differ
*   some results can be much less probable

. . .

$\Rightarrow$ testing is not deterministic

*   might not discover bug
*   test cannot be repeated with guarantee of the same results
*   hard to debug from failed test

## Explicit State Model Checking

a technique usefull for verification of parallel programs

*   explores all meanighfull interleavings of the program
*   can provide deterministic testing procedure
*   and prove absence of bugs
*   can be very resource consuming

. . .

\begin{latex}
\bigskip
\makebox[\textwidth][c]{
\begin{tikzpicture}[>=stealth',shorten >=1pt,auto,node distance=3em,initial text=, ->]
    \tikzstyle{every state} = [ellipse, minimum size = 1.5em]
    \path[use as bounding box] (-5.2,0.3) rectangle (5.2,-3.2);

    \node[state] (init) {\texttt{x = 0; y = 0}};
    \node<3->[state, below left = of init] (l) {\texttt{x = 1; y = 0}};
    \node<4->[state, below right = of init] (r) {\texttt{x = 0; y = 1}};
    \node<5->[state, below right = of l] (c) {\texttt{x = 1; y = 1}};

    \path<3-> (init) edge node[above left] {$t_1$: \texttt{x := 1}} (l);
    \path<4-> (init) edge node {$t_2$: \texttt{y := 1}} (r);
    \path<5-> (l) edge node[below left] {$t_2$: \texttt{y := 1}} (c);
    \path<6-> (r) edge node {$t_1$: \texttt{x := 1}} (c);
\end{tikzpicture}
}
\end{latex}

## Verification in Practictice?

traditionally, the program has to be translated to a modelling language

*   extra effort (usually manual translation)
*   not everything can be translated exactly
*   translation can introduce or hide errors
*   still usefull in critical systems (modelling before development)

. . .

solution: verify code in some programming language directly!

*   program is verified directly, no need to model it
*   should not limit the programmer to a subset of the language
*   even more compulationaly expensive

## Verification of Unit Tests of Parallel C++ Programs in DIVINE

*   full support for C and C++ language features
*   supports most of the standard C/C++ library and the `pthread` threading library
*   effective verification using many reduction techniques
*   uses the LLVM intermediate representation as an input

. . .

we aim at

*   practical usability for unit testing of parallel programs
*   and therefore adoption by programmers
*   in future also support for programs with inputs

## DIVINE Verification Workflow

\begin{latex}
    \medskip
    \large
    \makebox[\textwidth][c]{
    \begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                       , semithick
                       , scale=1
                       , state/.style={ rectangle, draw=black, very thick,
                         minimum height=2em, minimum width = 4em, inner
                         sep=2pt, text centered, node distance = 2em }
                       ]
      \node[state, minimum width = 5em] (lib) {Libraries};

      \node[state, below = of lib.south west, anchor = north west] (cpp) {C++};
      \node[state, right = 4em of cpp, rounded corners] (clang) {Clang};
      \node[state, right = 4em of clang] (llvm) {LLVM IR};
      \node[state, right = 4em of llvm, rounded corners] (divine) {\divine};

      \node[state, above = of divine.north east, anchor = south east, minimum width = 15em] (ltl) {Verified property: safety, LTL};
      \node[state, below = of divine] (valid) {\color{paradisegreen!70!black}OK};
      \node[state, left = of valid, minimum width = 8em] (ce) {\color{red!70!black}Counterexample};

      \path (ltl.south) edge[out=270, in=90] (divine.north)
            (cpp) edge (clang)
            (clang) edge (llvm)
            (lib) edge [out=0, in=90, looseness = 1] (clang)
            (llvm) edge (divine)
            (divine.south) edge (valid) edge[out=270, in=90] (ce.north)
            ;
    \end{tikzpicture}
    }
\end{latex}

. . .

safety property: assertion safety, memory safety (dereferrence, array bounds, memory leaks), use of uninitialized memory, deadlocks, …

. . .

LTL property: changes in time, i.e.: "after action $A$ occurs, $B$ must happen repeatedly"

## My Work on DIVINE

during bachelor & masters studies

*   memory-efficient representation of program states
*   verification with relaxed memory models
*   general maintanance

. . .

Ph.D. research topic

*   optimizations and transformations of the LLVM intermediate representation
*   to allow faster/more efficient verification
*   to be able to verify more properties
*   general usability of DIVINE

## Summary

*   parallel programming is hard, testing not very efficient
*   model-checking can help in this regards
*   C/C++ code can be verified directly

. . .

\hfill Thank you!
