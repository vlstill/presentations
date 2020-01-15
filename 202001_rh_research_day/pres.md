---
vim: spell spelllang=en tw=80 fo+=t
title: "Heavy-Duty Program Analysis with DIVINE"
author:
    - Vladimír Štill
header-includes:
    - \input{defs}
lang: english
date: 23rd January 2020
abstract: |
    DIVINE is a tool for analysis of programs written in C and C++ that
    primarily targets hard to find bugs, including concurrency-related bugs,
    assertion violations, and memory errors. It is developed in the Parallel and
    Distributed Systems Laboratory at the Faculty of Informatics, Masaryk
    University. One of the main strengths of DIVINE is that it is precise and
    can discover bugs which are hard or impossible to find by other means.
    Unsurprisingly, this precision comes at some cost in the human and computing
    effort put to the analysis. In this presentation, we outline some of the
    strengths and weaknesses of DIVINE. We describe how we can use DIVINE for
    the analysis of programs which have source code and tests available, and
    what can we do if that is not the case. We also show how DIVINE can be used
    with programs that run in the POSIX environment, i.e., use system calls and
    interact with other programs using filesystem or network. Finally, we
    present how we can apply DIVINE to the discovery of sporadically occurring
    concurrency bugs.
...

## What is DIVINE?

\begin{tikzpicture}[>=latex,>=stealth',auto,node distance=1.5em,semithick,initial
                    text=, ->, shorten >=1pt, initial distance = 1cm]
  \tikzstyle{box}=[rectangle, draw, minimum size=0.75cm, align = center]
  \tikzstyle{group}=[draw, dashed, inner sep = 0.7em, rounded corners=0.4em]
  \draw[use as bounding box, draw=none] (-2.8em, 2.8em) rectangle (27.6em, -11em);

  \node[box] (c) {C source};
  \node[box, right = of c] (cpp) {C++ source};
  \node<2->[box, right = of cpp, text width=10em] (build) {C/C++ pogram with a build recipe (Makefile, CMake, …)};

  \node[box, below = 3em of cpp] (divine) {\textbf{DIVINE}};

  \node<3->[box, below left = 3em and 0em of divine, text width = {width("Error Found")}] (ok) {Correct};
  \node<4->[box, below right = 3em and 0em of divine] (nok) {Error Found};

  \tikzstyle{err}=[box, text width = {width("Nontermination/Deadlock")}]
  \node<5->[err, right = of nok] (assert) {Assertion Violation};
  \node<6->[err, above = 0.2em of assert] (memerr) {Memory Error};
  \node<7->[err, above = 0.2em of memerr] (nonterm) {Nontermination/Deadlock};

  \path
    (c) edge (divine)
    (cpp) edge (divine);
  \path<2-> (build) edge (divine);
  \path<3-> (divine) edge (ok);
  \path<4-> (divine) edge (nok);

  \path<5-> (nok) edge (assert);
  \draw<6-> ($ (nok.east) + (0.75em, 0) $)
      -- ($ (memerr.west) - (0.75em, 0) $)
      -- (memerr.west);
  \draw<7-> ($ (nok.east) + (0.75em, 0) $)
      -- ($ (nonterm.west) - (0.75em, 0) $)
      -- (nonterm.west);

  \node<1>[group, fit = (c) (cpp)] (input) {};
  \node<2->[group, fit = (c) (cpp) (build)] (input) {};

  \node[anchor=west] at (input.east) {Input};
\end{tikzpicture}

\only<1>{DIVINE is a tool for analysis of C and C++ programs.}
\only<2>{It can process single files, or we can use a replacement compiler to compile
larger programs into a form which can be processed by DIVINE.}


## Relaxed Memory {.fragile}


. . .

- caches and inter-core communication in modern CPUs

* out-of-order execution

- has observable effects in multithreaded environments

- can cause nasty and hard to detect bugs

## DIVINE and Relaxed Memory

**Model Checking of C++ Programs under the \xtso Memory Model**

- published and presented at ICFEM 2018

- novel approach to analysis under the memory model used on Intel x86 CPUs

\bigskip\pause

- some performance improvements might be possible in near future with new features of DIVINE

## SV-COMP 2019

- Competition on Software Verification, associated with TACAS

\bigskip\pause

- 5th place out of 12 in Concurrency cathegory (with 0 bad results)

\bigskip

- a lot of unconfirmed but right results

\bigskip\pause

# DEMO

## Nontermination Analysis

- checks that program is not only safe, but also certain parts of it terminate
  - critical sections
  - waiting for condition variables, threads…
  - user-defined parts

\bigskip\pause

**Local Nontermination Detection for Parallel C++ Programs**

- submitted to SEFM 2019 (notification pending)

## Future Work

- more efficient nontermination detection (new algorithm)

\bigskip\pause

- more efficient relaxed memory model support

\bigskip\pause

- at 3.5 years mark… time to start wrapping up
