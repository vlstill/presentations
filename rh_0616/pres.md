---
title: Compilation, Code Transformations, and Executable Counterexamples for DIVINE
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \usetikzlibrary{fit,backgrounds}
lang: english
date: 15th June 2016
# aspectratio: 169
...

## Verification of C++ programs {.t}

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
      \node[state, right = of cpp, anchor = west
           , rounded corners, minimum width = 5em] (libclang) {\clang{} API};
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

      \node[below] at (cc.north) {\textbf{\divine{} CC}};

      \node[state, right = 2.4em of lart, anchor = west, node distance = 5em
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
      \node[below] at (divine.north) {\textbf{\divine}};

      \node[state, above = 5em of divine, minimum width = 15em]
          (ltl) {Verified property: safety, LTL};

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
        \divine{} compiler using \clang{} API and \llvm{} linker
        \begin{itemize}
            \item isolated from system libraries
            \item isolated from (most of) system defines
        \end{itemize}
    }
    \only<3>{
        Libraries
        \begin{itemize}
            \item \divine{} has to have complete implementation of libraries
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
            \item command line arguments, file system
            \item conversion of \llvm{} to the internal representation
        \end{itemize}
    }
    \only<5>{
        Counterexample generator
        \begin{itemize}
            \item algorithms will produce a counterexample as a list of choices
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

## \divine{} CC

\divine{} 3 used a wrapper over system's \clang{} for compilation into \llvm

*   depends on *the particular version* of \clang{}
    *   the same version as used to build \divine{}
*   can accidentally use system libraries

. . .

now \divine{} 4 uses fully integrated \clang{} API based compiler

*   C++ API exports most of features of \clang{}
    *   primarily intended for syntactic manipulation and refactoring tools and
        for \clang{} itself
*   uses \clang's VFS for in-memory compilation and isolation
    *   isolated from system headers (unless explicitly imported: `-I`)
*   isolated from most of system defines (like `__unix__`)
*   does not depend on any part of the system compiler

## Instrumentations in \divine{} Loader

In \divine{} 4 the interpreter needs to be notified when

*   a memory location visible to multiple threads is accessed
*   a cycle in the state space can occur
    *   safely approximated by cycles in the control flow

. . .

instrumenting the code allows more flexible interaction between \divine{} and
static analysis in LART

*   some interrupt points can be omitted
*   it is easier to specify where the thread was interrupted
*   it is possible to generate executable counterexamples

## Instrumentations in \divine{} Loader

Loader also adds metadata for each function:

*   function pointer
*   number of arguments
*   argument types
*   frame size
*   instruction metadata

. . .

Annotations to facilitate debugging/counterexamples

*   printed when a function is entered or left (including unwinding)

## Executable Counterexamples

the bitcode already contains C and C++ library implementation

*   native implementation of the \divine{} runtime has to be added
*   simulates counterexample found by \divine
*   no parallelism, instead manages and switches separate stacks
*   support now planned only for `x86_64` on Linux
    *   depends on calling conventions, syscalls
*   work in progress

## Future Work

*   Finish executable counterexamples
*   \divine{} CC which works as a drop-in replacement for \clang{}/GCC

    ```{.sh}
    $ CC=divine.cc CXX=divine.cc make
    ```
*   optimized instrumentations for the loader
    *   statically detect some cases when interrupts cannot happen
        *   local variables
        *   cycles which must terminate

*   calculate frame layout in the loader
    *   includes allocation of \llvm{} registers into memory slots
    *   can save memory

## Students

Peter Hutta

*   found suspected memory leak in `boost` parallel queue
    *   not yet tracked to the code

. . .

Jakub Kadaši

*   works on \divine{} VFS in order to allow verification of `lvmetad`

. . .

Katarína Kejstová

*   works on \divine{} VFS
*   also on \divine{} 4: shadow maps -- metadata for heap memory
    * pointer and value-initialization tracking

. . .

Jan Mrázek

*   will present Caching in Control-Explicit Data-Symbolic Model Checking
*   also works on \divine{} 4 on \divine{} runtime
