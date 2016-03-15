---
title: Formal Verification of C/C++ Programs
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
lang: english
date: 18th March 2016
aspectratio: 169
...

# The Current Situation

## DIVINE

*   explicit-state model checker for verification of parallel programs
    *   main focus on C/C++ using \llvm
    *   also many other inputs: DVE, CESMI, timed automata
*   verifies safety and LTL properties
    *   specification depends on the input formalism
    *   for C/C++: assertion safety, memory safety, uninitialized variables
        tracking, `pthreads` deadlock detection,…
*   parallel (and distributed) verification
*   efficient state space reduction and compression techniques
*   <https://divine.fi.muni.cz>


## My Work on DIVINE

*   compression of the state space
    *   reduces memory requirements for LLVM verification, roughly $100 -
        500\times$ for reasonably sized programs (efficiency grows with program
        size)
    *   bachelor's thesis, published at SEFM 2015
    \pause
*   export of explicit state space from DIVINE
    *   useful for chaining with other tools
    *   case study for probabilistic verification to appear on ACM SAC 2016
    \pause
*   verification under more realistic memory models
    *   verification which is closer to behaviour or real-world memory
        hierarchies
    *   master's thesis, preliminary version at MEMICS 2015, extended version
        submitted for publication
    \pause
*   extended and fixed state space reductions
    *   up to $3\times$ extra reduction
    *   master's thesis
    \pause
*   code maintenance

## LLVM Transformations

*   LLVM IR can be easily transformed before the verification
*   can be used to extend model checker's abilities, reduce state space

\begin{latex}
    \bigskip
    \makebox[\textwidth][c]{
    \footnotesize
    \begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                       , semithick
                       , state/.style={ rectangle, draw=black, very thick,
                         minimum height=2em, minimum width = 4em, inner
                         sep=2pt, text centered, node distance = 2em }
                       ]
      \node[state, minimum width = 5em] (lib) {Libraries};

      \node[state, below = of lib.south west, anchor = north west] (cpp) {C++};
      \node[state, right = of cpp, rounded corners] (clang) {Clang};
      \node[state, right = of clang] (llvm) {LLVM IR};
      \node[state, onslide={<-1> color=white}, right = of llvm, rounded corners] (lart) {\lart};
      \node[state, onslide={<-1> color=white}, right = of lart] (llvm2) {LLVM IR};
      \node[state, right = of llvm2, rounded corners] (divine) {\divine};

      \node[state, above = of divine.north east, anchor = south east, minimum width = 15em] (ltl) {Verified property: safety, LTL};
      \node[state, below = of divine] (valid) {\color{paradisegreen!70!black}OK};
      \node[state, left = of valid, minimum width = 8em] (ce) {\color{red!70!black}Counterexample};

      \path (ltl.south) edge[out=270, in=90] (divine.north)
            (cpp) edge (clang)
            (clang) edge (llvm)
            (lib) edge [out=0, in=90, looseness = 1] (clang)
            (divine.south) edge (valid) edge[out=270, in=90] (ce.north)
            ;
      \path<-1> (llvm) edge (divine);
      \path<2-> (llvm) edge (lart)
            (lart) edge (llvm2)
            (llvm2) edge (divine);
    \end{tikzpicture}
    }
\end{latex}

\pause\pause

*   case study: verification of weak memory models through LLVM transformation

## Weak Memory Models

*   in CPU a write performed by one thread need not be visible to other thread
    immediately
*   writes can be reordered -- with reads or also with stores
*   verifiers often omit this

. . .

Solution

*   the program is instrumented to simulate delayed/reordered writes
*   adds more nondeterminism to the program
*   LLVM transformation

# Plans

## Plans -- Overview

\begSaveBox{boxplans}

Long Term

*   improve practical usability of model checking for development of parallel
    programs
*   explore the usage of static analysis for pre-processing of programs for
    DIVINE

Short Term (this year)

*   more robust compilation of programs for DIVINE
*   register allocation for LLVM
*   verification of open programs using SMT (merge of SymDIVINE into DIVINE)

\endSaveBox

\usebox\boxplans

## Compilation of Programs for DIVINE

*   currently, DIVINE facilitates a simple wrapper over clang for compilation
    *   together with tweaked LLVM-based linker
*   DIVINE has to provide own implementation of C/C++/`thread`/… libraries
*   system configuration and even system headers can leak into DIVINE
    compilation
*   hard to integrate into nontrivial build processes (makefiles, cmake,…)

. . .

Solution

*   an isolated environment which can access only user-provided sources and
    DIVINE libraries
*   DIVINE compiler which can be used as a drop-in replacement for GCC/clang
*   ideally it would produce both LLVM bitcode for DIVINE and ELF binary
    *   allow build processes which feature code generating programs

## Register Allocation for LLVM

*   LLVM uses Static Single Assignment (registers not reused)
*   wastes memory in DIVINE
*   can prevent state merging (e.g. in optimized busy-waiting cycles)

. . .

Solution

*   allocate registers into slots, reuse slots
*   differs from register allocation in code generator of a compiler
    *   the number of registers is not fixed
    *   needs to consider program semantics

## Verification of Open Programs

*   programs with inputs cannot be fully verified by DIVINE
*   SymDIVINE can do this for simple programs
    -   a proof-of-concept tool for verification of open LLVM programs

. . .

Solution

*   the idea is to merge SymDIVINE into DIVINE using an LLVM transformation
*   the program is to be changed so that is manipulates (parts of) data
    symbolically
*   this hybrid program is then executed by DIVINE which uses special algorithm
    to explore state space of such program

## Plans -- Overview

\usebox\boxplans

\bigskip\bigskip
\hfill Thanks for your attention!

