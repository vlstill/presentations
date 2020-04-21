---
vim: spell spelllang=en fo+=t tw=80
title: "Symbolic Liveness Analysis of Real-World Software"
author:
    - Daniel Schemmel
    - Julian Büning
    - Oscar Soria Dustmann
    - Thomas Noll
    - Klaus Wehrle
header-includes:
    - \input{defs}
    - \institute{}
lang: english
date: 7th December 2018
...

## Introduction

*there are some bugs which are not covered by safety analysis*

. . .

- *liveness properties* can describe what must hold for infinite runs
- often described using LTL

. . .

**this paper**

- one generally useful liveness property (*live program*)
- efficient algorithm for checking it
- evaluation on real-world code

## Live Program

a program is *live* iff it always eventually consumes input or terminates
($G(F(\textit{input} \lor \textit{end}))$)

. . .

- add special *end* loop to terminating state
- input and end actions are productive
- find non-productive cycles in the state space

. . .

- state-space must be deadlock-free
- only data/input nondeterminism (no parallel programs)
    - non-productive cycles are deterministic
- based on symbolic execution + cycle detection

## The Algorithm

- program is assumed to work in finite memory \textrightarrow{} violations are
  lassos
- detect repeated states in the execution

. . .

- extend symbolic execution with fingerprinting of states to facilitate state
  comparisons
  - based on strong hashes
  - no direct comparison of states

## State Equality

\bigskip
*claim*: For any lasso in explicit execution there is a lasso in symbolic
execution if the following holds:

- only add branch conditions that are not already implied by the path
  condition

. . .

*I don't think this is correct*

```{.c}
int x = input(), y = input();
while ( true ) {
    x += y;
    x -= y;
}

```

## Fingerprinting

*idea*: don't compare full states, use hashes of states

. . .

- hashes are computed incrementally by xoring hashes of fragments
- a new hash can be computed by:
    1. re-computing the hash of fragment to be modified
    2. xor-ing this hash to the complete hash
    3. modifying the fragment
    4. computing new hash of fragment
    5. xor-ing it to the complete hash

. . .

- furthermore, we need to remember hashes only since the last input

## Evaluation

- on Coreutils, BusyBox, and Toybox
- compared to reachability checking in KLEE

. . .

- reported 5 bugs ($2 \times \text{GNU}$ `tail`, GNU `ptx`, BusyBox `hush`)
