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

## My Work on DIVINE

*   compression of the state space
    *   bachelor's thesis, published in SEFM 2015
*   export of explicit state space from DIVINE
    *   useful for chaining with other tools
    *   case study for probabilistic verification to appear in ACM SAC 2016
*   verification under more realistic memory models
    *   master's thesis, preliminary version in MEMICS 2015, extended version
        submitted for publication
*   code maintenance

## LLVM Transformations

*   LLVM IR can be transformed (pre-processed) before verification
*   use static analysis to extend model checker's abilities, improve performance

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

*   a write performed by one thread need not be visible to other threads
    immediately
*   writes can be reordered -- with reads or with reads and writes
*   resulting bugs might be hard to detect by traditional methods

. . .

Solution

*   the program is instrumented to simulate delayed/reordered writes
*   adds more nondeterminism to the program
*   LLVM transformation

# Plans

## Plans -- Overview

\begSaveBox{boxplans}

Long Term

*   improve practical usability of model checking for development of programs
*   explore the use of static analysis for pre-processing of programs for
    DIVINE

Short Term (this year)

*   more robust compilation of programs for DIVINE
*   register allocation for LLVM
*   verification of programs with inputs using SMT (merge SymDIVINE into DIVINE)

\endSaveBox

\usebox\boxplans

## Compilation of Programs for DIVINE

*   currently, DIVINE facilitates a simple wrapper over clang for compilation
*   DIVINE has to provide own implementation of C/C++/`thread`/… libraries
*   system configuration and even system headers can leak into DIVINE
    compilation
*   hard to integrate into nontrivial build processes (makefiles, cmake,…)

. . .

Solution

*   an isolated environment which can access only user-provided sources and
    DIVINE libraries
*   DIVINE compiler which can be used as a drop-in replacement for GCC/clang
*   produce LLVM bitcode for DIVINE alongside native code in a single ELF binary

## Verification of Programs with Inputs

*   programs with inputs cannot be fully verified by DIVINE
*   SymDIVINE can do this for simple programs
    -   a proof-of-concept tool for verification of LLVM programs with inputs

. . .

Solution

*   merge SymDIVINE into DIVINE using an LLVM transformation
*   the program is to be changed so that it manipulates (parts of) data
    symbolically
*   this hybrid program is then executed by DIVINE which uses special algorithm
    to explore state space of such programs

## Plans -- Overview

\usebox\boxplans

\bigskip\bigskip
\hfill Thanks for your attention!

