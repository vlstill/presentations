---
vim: spell spelllang=en tw=80 fo+=t
title: "Analysis of Concurrent C++ with DIVINE"
author:
    - Vladimír Štill
header-includes:
    - \input{defs}
lang: english
date: 10th June 2019
...

## Topic Introduction

- analysis of programs written in C and C++
  - assertion violations, memory errors, …

\bigskip\pause

- I focus on parallel programs
  - data races, nondeterministic behaviour, deadlocks, …

\bigskip\pause

- using the DIVINE tool developed in ParaDiSe
  - a "heavy duty" verifier (model checker)


## Relaxed Memory {.fragile}

\begin{center}
  \includegraphics[width=8em]{../icfem_2018/spectre}
\end{center}

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
