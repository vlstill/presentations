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
    - \setbeamersize{text margin left=1.2em, text margin right=1.2em}
lang: english
date: 26th July 2017
...

## DIVINE -- Verification of C++ programs

**DIVINE is a tool for testing and verification of C/C++ programs**

*   memory safety, assertion safety, parallelism errors

*   easy error injection

*   full support for C and C++, partial support for POSIX

*   using clang/LLVM compiler infrastructure

. . .

\bigskip

**Contribution of This Work**

*   full support for C++ exceptions

*   with minimal changes to the verification core of DIVINE

*   re-using existing implementation of exception matching in the C++ runtime

## DIVINE -- Verification of C++ programs

\begin{latex}
\medskip
\makebox[\textwidth][c]{
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

  \node[state, below = 3em of code, rounded corners] (clang) {compiler};
  \node[state, below = 1.5em of clang.south west, anchor = north west] (runtime) {runtime};
  \node[state, right = of clang] (llvm) {\llvm IR};
  \node[state, right = of llvm, rounded corners, minimum width = 8em] (lart) {instrumentation};
  \node[state, right = of lart] (illvm) {\divm IR};
  \node[state, below = 1.5em of illvm.south west, anchor = north west, rounded corners, minimum width = 8em] (verifier) {verification core};
  \node[above = 0.5em of lart] (pverify) {};

  \node[state, below = 2em of verifier.south east] (valid) {\color{green!40!black}Valid};
  \node[state, below = 2em of verifier.south west, minimum width = 8em] (ce) {\color{red}Counterexample};

  \begin{pgfonlayer}{background}
      \node[state, fit = (pverify) (clang) (runtime) (llvm) (lart) (illvm) (verifier),
            inner sep = 0.8em, thick, rounded corners, dashed] (verify) {};
  \end{pgfonlayer}

  \node[below = 0.2em] at (verify.north) {DIVINE};

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
}
\end{latex}

## Motivation

**C++ exceptions**

*   ubiquitous in real-world C++

*   disabling exceptions can change behaviour (`new`{.cpp})

*   runtime support required, cannot be handled by the compiler itself

. . .

\bigskip

**Off-the-Self Components**

*   using LLVM and clang helps a lot for C/C++ support

*   DIVINE also re-uses C and C++ standard libraries
*   allows for more precise verification then re-implementing C++ support

. . .

*   exceptions support is complex

*   re-implementation would risk imprecisions, would be large, or require
    changes to the verification core

## How Exceptions Work

\begin{columns}
\begin{column}{0.40\textwidth}

```{.cpp .numberLines}
X::~X() { }
void g() {
  throw std::exception();
}
void f() {
  X x;
  g();
}

int main() {
  try {
    f();
  } catch ( ... ) {
    /* ... */
  }
}
```

\end{column}
\begin{column}{0.50\textwidth}

\begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                   , semithick
                   , style={ node distance = 2em }
                   , state/.style={ rectangle, draw=black, very thick,
                     minimum height = 2em, minimum width = 10em, inner
                     sep=2pt, text centered, node distance = 0em }
                   ]

  \useasboundingbox (0,0) rectangle (15em, 20em);

  \node<1-7>[state, anchor = south] at (10em, 0em) (main) {\texttt{main:12}};
  \node<2>[state, above = of main] (f6) {\texttt{f:6}};
  \node<3-5>[state, above = of main] (f7) {\texttt{f:7}};
  \node<4-5>[state, above = of f6] (g3) {\texttt{g:3}};
  \node<5>[state, above = of g3] (throw) {\texttt{throw}};
  \node<6-7>[state, above = of main] (f8) {\texttt{f:8} (cleanup)};
  \node<7>[state, above = of f6] (dX) {\texttt{X::\textasciitilde X:1}};
  \node<8>[state, anchor = south] at (10em, 0em) (main14) {\texttt{main:14}};

  \node<5-7> at (10em, 10em) {\textbf{unwinding}};

  \draw<1>(2em, 5em) -- (-9em, 5em);
  \draw<2>(2em, 12.5em) -- (-10em, 12.5em);
  \draw<3>(2em, 11.2em) -- (-10em, 11.2em);
  \draw<4>(2em, 16.3em) -- (0em, 16.3em);
  \draw<6>(2em, 10em) -- (-12em, 10em);
  \draw<7>(2em, 18.8em) -- (-7em, 18.8em);
  \draw<8>(2em, 2.5em) -- (-6em, 2.5em);
\end{tikzpicture}
\end{column}
\end{columns}

## Running C++ Program

\begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                   , semithick
                   , style={ node distance = 2em }
                   , state/.style={ rectangle, draw=black, very thick,
                     minimum height=1.7em, minimum width = 4.4em, inner
                     sep=2pt, text centered, node distance = 2em }
                   ]
  \node[state, minimum width = 5em, onslide = {<3> fill = paradisegreen}] (code) {\only<3>{\color{white}}C++ code};

  \node[state, below = of code, rounded corners, onslide = {<3> fill = paradisegreen}] (clang) {\only<3>{\color{white}}clang};

  \node[state, right = of clang, onslide = {<3> fill = paradisegreen}] (bc) {\only<3>{\color{white}}bitcode};

  \node[state, rounded corners, right = of bc, minimum width = 10em] (llvm) {LLVM + linker};

  \node[state, minimum width = 5em, above = of llvm, onslide = {<3> fill = paradisegreen}] (libcxxabi) {\only<3>{\color{white}}libc++abi};
  \node[state, minimum width = 5em, left = 1em of libcxxabi, onslide = {<3> fill = paradisegreen}] (libcxx) {\only<3>{\color{white}}libc++};
  \node[state, minimum width = 5em, right = 1em of libcxxabi] (libunwind) {libunwind};

  \node[state, below = of llvm, minimum width = 7em] (machinecode) {Executable};
  \node[state, below = of machinecode, minimum width = 7em] (machine) {Machine + OS};

  \path (code) edge (clang)
        (libcxx.south) edge (llvm)
        (libcxxabi) edge (llvm)
        (libunwind) edge (llvm)
        (clang) edge (bc)
        (bc) edge (llvm)
        (llvm) edge (machinecode)
        (machinecode) edge[dashed] (machine)
        (libcxxabi) edge[dashed, red, out = 45, in = 135, onslide={<1>
        color=white}] (libunwind)
        (libunwind) edge[dashed, red, out = 270, in = 0, onslide={<1> color=white}] (machine)
        ;
\end{tikzpicture}

*   the code is compiled and linked to the standard library (`libc++`), runtime
    library (`libc++abi`), and the unwinder (`libunwind`)

    . . .

*   the runtime library \text{\color{red}depends} on the unwinder which
    \text{\color{red}depend} on the machine and OS

    . . .

*   \textbf{\color{paradisegreen}green} components are re-used in DIVINE

## Analyzing C++ Program with DIVINE

\begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                   , semithick
                   , style={ node distance = 2em }
                   , state/.style={ rectangle, draw=black, very thick,
                     minimum height=1.7em, minimum width = 4.4em, inner
                     sep=2pt, text centered, node distance = 2em }
                   ]
  \node[state, minimum width = 5em] (code) {C++ code};

  \node[state, below = of code, rounded corners] (clang) {clang};

  \node[state, right = of clang] (bc) {bitcode};

  \node[state, rounded corners, right = of bc, minimum width = 10em, onslide = {<1,3> fill=paradisegreen}] (llvm) {\only<1,3>{\color{white}}LLVM instrumentation};

  \node[state, minimum width = 5em, above = of llvm] (libcxxabi) {libc++abi};
  \node[state, minimum width = 5em, left = 1em of libcxxabi] (libcxx) {libc++};
  \node[state, minimum width = 5em, right = 1em of libcxxabi, onslide = {<2,3> fill=paradisegreen}] (libunwind) {\only<2,3>{\color{white}}libunwind};

  \node[state, below = of llvm, minimum width = 7em] (machinecode) {bitcode};
  \node[state, below = of machinecode, minimum width = 7em] (machine) {DiVM};

  \path (code) edge (clang)
        (libcxx.south) edge (llvm)
        (libcxxabi) edge (llvm)
        (libunwind) edge (llvm)
        (clang) edge (bc)
        (bc) edge (llvm)
        (llvm) edge (machinecode)
        (machinecode) edge[dashed] (machine)
        (libcxxabi) edge[dashed, red, out = 45, in = 135] (libunwind)
        (libunwind) edge[dashed, red, out = 270, in = 0] (machine)
        ;
\end{tikzpicture}

**DIVINE/DiVM-specific components**

*   LLVM-based preprocessing

    . . .

*   DiVM-based implementation of `libunwind`

    . . .

*   approximately 700 lines of new modular code

## LLVM Transformation

*   exceptions require metadata about stack frames, catch blocks and cleanups
    for destructors

    *   normally describe the machine code
    *   DIVINE needs metadata for LLVM bitcode

    . . .

*   metadata format depends on the implementation of the C++ runtime library

    . . .

*   output of the transformation is LLVM bitcode with additional metadata stored
    in global constants

*   C++ specific encoding of catch and cleanup locations

## The Unwinder (`libunwind`)

*   used to manipulate the execution stack

*   depends on the platform, calling conventions (e.g. Linux on `x86`)

*   new unwinder for DiVM

*   uses metadata from the transformation

*   provides metadata for the `libc++abi` callbacks which search for the location to
    restore control flow to

*   would also work with other languages

## Evaluation & Conclusion

*   reusable and modular implementation of C++ exceptions
*   substantial improvement in verification fidelity

    . . .

*   minimal investment: $\sim 700$ lines of code

    . . .

*   minimal overhead: $2.6\,\%$ (compared to an older style of implementation which
    required changes to the verification core)

\center\
\
[`divine.fi.muni.cz`](https://divine.fi.muni.cz)\
`paradise-fi/divine` on [GitHub](https://github.com/paradise-fi/divine)\
\
more data & code:\
[`divine.fi.muni.cz/2017/exceptions`](https://divine.fi.muni.cz/2017/exceptions)
