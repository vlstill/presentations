---
vim: spell spelllang=en tw=80 fo+=t
title: Using Off-the-Shelf Exception Support Components in C++ Verification
author:
    - \textbf{Vladimír Štill}
    - Petr Ročkai
    - Jiří Barnat
header-includes:
    - \usepackage{divine}
    - \usepackage{sansmathfonts}
    - \newcommand{\TODO}[1]{{\color{red}#1}}
    - \newcommand{\redon}[2]{{\only<#1>{\color{red}}#2}}
lang: english
date: 26th July 2017
...

## Introduction -- DIVINE

DIVINE is a tool for testing and verification of C and C++ programs

*   memory safety, assertion safety, parallelism errors
*   easy nondeterminism, error injection
*   aims at full support for C and C++, partial support for POSIX
*   using clang/LLVM compiler infrastructure

\begin{latex}
\center
\footnotesize
\begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                   , semithick
                   , style={ node distance = 2em }
                   , state/.style={ rectangle, draw=black, very thick,
                     minimum height=1.7em, minimum width = 4.4em, inner
                     sep=2pt, text centered, node distance = 2em }
                   ]
  \node[state, minimum width = 6em] (code) {C++ code};
  \node[state, minimum width = 10.4em, right = 13.6em of code] (prop) {property and options};

  \node[state, below = 2.7em of code, rounded corners] (clang) {compiler};
  \node[state, below = 1.3em of clang.south west, anchor = north west] (runtime) {runtime};
  \node[state, right = of clang] (llvm) {\llvm IR};
  \node[state, right = of llvm, rounded corners, minimum width = 8em] (lart) {instrumentation};
  \node[state, right = of lart] (illvm) {\divm IR};
  \node[state, below = 1.3em of illvm.south west, anchor = north west, rounded corners, minimum width = 8em] (verifier) {verification core};
  \node[above = 0.5em of lart] (pverify) {};

  \node[state, below = 1.6em of verifier.south east] (valid) {\color{green!40!black}Valid};
  \node[state, below = 1.6em of verifier.south west, minimum width = 8em] (ce) {\color{red}Counterexample};

  \begin{pgfonlayer}{background}
      \node[state, fit = (pverify) (clang) (runtime) (llvm) (lart) (illvm) (verifier),
            inner sep = 0.8em, thick, rounded corners, dashed] (verify) {};
  \end{pgfonlayer}

  \node[below = 0.2em] at (verify.north) {\texttt{divine verify}};

  \path (prop.348) edge[|*] (verifier.north)
        (prop.192) edge[|*] (lart.north)
        (code) edge (clang)
        (runtime) edge (clang)
        (clang) edge (llvm)
        (llvm) edge (lart)
        (lart) edge (illvm)
        (illvm) edge[|*] (verifier.north)
        (verifier) edge (valid) edge (ce)
        ;
\end{tikzpicture}
\end{latex}

. . .

\vspace{-\bigskipamount}

*   the contribution of this paper is full support for C++ exceptions

## Motivation

**C++ exceptions**

*   exceptions are ubiquitous in real-world C++
*   C++ can be used without exceptions, but a lot of code uses them
*   behavior of some functionality changes if exceptions are disabled (`new`{.cpp})
*   exceptions require runtime support, cannot be solved by the compiler itself

. . .

**Off-the-Self Components**

*   using LLVM and clang helps a lot for C/C++ support
*   DIVINE also re-uses C and C++ standard libraries

. . .

*   exceptions support is complex
*   re-implementation would risk imprecisions, would be large, or require
    changes to the verification core

## How Exceptions Work

TODO

## Running C++ Program

\begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                   , semithick
                   , style={ node distance = 2em }
                   , state/.style={ rectangle, draw=black, very thick,
                     minimum height=1.7em, minimum width = 4.4em, inner
                     sep=2pt, text centered, node distance = 2em }
                   ]
  \node[state, minimum width = 5em] (code) {C++ code};
  \node[state, minimum width = 5em, right = 1em of code] (libcxx) {libc++};
  \node[state, minimum width = 5em, right = 1em of libcxx] (libcxxabi) {libc++abi};

  \node[state, below = 2.8em of code, rounded corners, below = of libcxx] (clang) {clang};

  \node[state, right = of clang] (bc) {bitcode};
  \node[state, rounded corners, right = of bc] (llvm) {LLVM};
  \node[state, below = of llvm, minimum width = 7em] (machinecode) {Machine Code};

  \path (code) edge (clang)
        (libcxx.south) edge (clang)
        (libcxxabi) edge (clang)
        (clang) edge (bc)
        (bc) edge (llvm)
        (llvm) edge (machinecode)
        ;
\end{tikzpicture}

TODO

## Analyzing C++ Program with DIVINE

TODO

## Contribution

*   analysis of existing implementations and identification of re-usable
    components
*   implementation of DiVM-specific components
    *   LLVM-based preprocessing
    *   DiVM-based implementation of `libunwind`
    *   approximately 700 lines of new modular code

## LLVM Transformation

*   exceptions require metadata about stack frames, catch blocks and cleanups
    for destructors
*   normally, metadata are generated for the machine code, we need metadata
    describing LLVM bitcode
*   metadata are specific for given implementation of C++ runtime library
    *   in our case `libc++abi`
*   output of the transformation is LLVM bitcode with additional metadata stored
    in global constants
*   C++ specific encoding of catch and cleanup locations

## The Unwinder (`libunwind`)

*   unwinder is used to manipulate execution stack
*   dependent on the platform, calling conventions (e.g. Linux on `x86`)
*   our unwinder is for DiVM
*   uses metadata from the transformation
*   provides metadata for the `libc++abi` callbacks which search for location to
    restore control flow to
*   would also work with other languages

## Evaluation

*   substantial improvement in verification fidelity
*   $2.6\,\%$ slowdown compared to DIVINE 3 style of implementation which
    requires changes to the verification core

TODO

## Conclusion

*   reusable and modular implementation of C++ exceptions
*   minimal investment: $\sim 700$ lines of code
*   minimal overhead: $2.6\,\%$

\center\
\
[`divine.fi.muni.cz`](https://divine.fi.muni.cz)\
`paradise-fi/divine` on [GitHub](https://github.com/paradise-fi/divine)\
\
more data & code:\
[`divine.fi.muni.cz/2017/exceptions`](https://divine.fi.muni.cz/2017/exceptions)
