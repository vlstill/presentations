---
title: Compilation, code transformations, and executable counterexamples for DIVINE
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \usetikzlibrary{fit,backgrounds}
lang: english
date: 15th June 2016
# aspectratio: 169
...

## Verification of C++ program {.t}

\begin{latex}
    \bigskip
    \makebox[\textwidth][c]{
    \scriptsize
    \begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                       , semithick
                       , state/.style={ rectangle, draw=black, very thick,
                         minimum height=2em, minimum width = 4em, inner
                         sep=2pt, text centered, node distance = 1.8em }
                       ]


      \node[state] (cpp) {C++ code};
      \node[state, right of = cpp, anchor = west, node distance = 5em
           , rounded corners, minimum width = 5em] (libclang) {LibClang};
      \node[state, above = of libclang, minimum width = 5em
           , onslide = {<3> fill = paradisegreen!50}] (lib) {Libraries};
      \node[state, right = of libclang, align = center, rounded corners]
            (lart) {\lart};
      \node[above of = lart, node distance = 6em] (phantom) {};

      \begin{pgfonlayer}{background}
      \node[state, rounded corners, inner sep = 0.8em
           , fit = (libclang) (lib) (lart) (phantom)
           , onslide = {<2> fill = paradisegreen!50}] (cc) {};
        \end{pgfonlayer}

      \node[below] at (cc.north) {\divine{} CC};

      \node[state, right of = lart, anchor = west, node distance = 5em
           , rounded corners, onslide = {<4> fill = paradisegreen!50}] (load) {Loader};
      \node[state, right = of load, rounded corners] (verify) {Verify};
      \node[state, below = of verify.south east, anchor = north east, rounded corners
           , align = center, onslide = {<5> fill = paradisegreen!50}]
          (cegen) {Counterexample\\generator};

      \node[above of = load, node distance = 2em] (phantom2) {};

      \begin{pgfonlayer}{background}
          \node[state, rounded corners, inner sep = 0.8em
               , fit = (load) (phantom2) (verify) (cegen)] (divine) {};
      \end{pgfonlayer}
      \node[below] at (divine.north) {\divine};

      \node[state, above of = divine,
            node distance = 11em, minimum width = 15em] (ltl) {Verified property: safety, LTL};

      \node[state, right = of verify] (valid) {\color{paradisegreen!70!black}OK};
      \node[state, right = of cegen, align = center, onslide = {<6> fill = paradisegreen!50}]
          (ce) {\color{red!70!black}Counterexample:\\program with\\encoded choices};


      \path (ltl.south) edge[out=270, in=90] (divine.north)
            (cpp) edge (libclang)
            (libclang) edge (lart)
            (lart) edge (load)
            (load) edge (verify)
            (verify) edge (valid)
            (verify) edge [in=60,out=270] (cegen)
            (cegen) edge (ce)
            (lib) edge (libclang)
            ;
    \end{tikzpicture}
    }

    \only<2>{
        \divine{} compiler using LibClang and \llvm{} linker
        \begin{itemize}
            \item isolated from system libraries
            \item isolated from (most of) system defines
            \item completely in-memory compilation
            \item does not depend on any part of the system compiler
        \end{itemize}
    }
    \only<3>{
        Libraries
        \begin{itemize}
            \item PDClib for C99
            \item libc++ and libc++ ABI for C++14 (newly ported to \divine{})
            \item pthreads, \divine{} runtime,…
        \end{itemize}
    }
    \only<4>{
        \divine{} Loader
        \begin{itemize}
            \item \textbf{\llvm{} instrumentation}: interrupt points, metadata,
            annotations
            \item conversion of \llvm{} to the internal representation
        \end{itemize}
    }
    \only<5>{
        Counterexample generator
        \begin{itemize}
            \item algorithm produces counterexample as a list of choices
                \begin{itemize}
                    \item thread switches, nondeterminism
                \end{itemize}
            \item embed choices into the program
            \item add implementation of \divine{} runtime
        \end{itemize}
    }
    \only<6>{
        Counterexample
        \begin{itemize}
            \item can be executed as normal binary
            \item can be debugged using existing debuggers
            \item does not depend on system libraries
        \end{itemize}
    }

    \vspace*{10em}
\end{latex}

## Instrumentations in \divine{} Loader

In \divine{} 4 the interpreter needs to be notified when

*   memory location visible by multiple threads is accessed
*   cycle in the control flow can occur

instrumenting the code allows more flexible interaction between \divine{} and
static analysis in LART

*   some interrupt points can be omitted
*   it is easier to specify where the thread was interrupted
*   it is possible to generate executable counterexamples

## Instrumentations in \divine{} Loader

Loader also adds metadata for each function:

*   function pointer, number of arguments, argument types, frame size…
*   instruction metadata

. . .

Annotations to facilitate debugging/counterexamples

*   output when a function is entered or left (including exceptions)

## Executable Counterexamples

the bitcode already contains C and C++ library implementation

*   native implementation of \divine{} runtime has to be added
*   simulates counterexample found by \divine
*   no parallelism, instead manages and switches separate stacks
*   currently support planned only for `x86_64` on Linux
    *   depends on calling conventions, syscalls
*   work in progress

## Future Work

*   Finish executable counterexamples
*   \divine{} CC which works as a drop-in replacement for \clang{}/GCC

    ```{.sh}
    $ CC=divine.cc CXX=divine.cc make
    ```
*   optimized instructions for loader
    *   detect some cases when interrupts cannot happen statically
        *   local variables
        *   cycles which must terminate

*   calculate frame layout in the loader
    *   includes allocation of \llvm{} registers into memory slots
    *   can save memory

. . .

\bigskip\bigskip
\hfill Thanks for your attention!

