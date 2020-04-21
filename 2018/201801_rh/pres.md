---
vim: spell spelllang=en tw=80 fo+=t
title: "Memory-Model-Aware Analysis of Parallel Programs"
author:
    - Vladimír Štill
header-includes:
    - \input{defs}
lang: english
date: 24th January 2018
...

## Topic Recapitulation

- analysis of parallel programs running under relaxed memory models
  - effects of cache, out-of-order execution
  * depends on architecture

    . . .

    *   Intel `x86`: store buffering
    *   ARM/POWER: almost arbitrary reordering of loads and stores

. . .

- this is significantly harder then analysis of all interleavings of the program

. . .

- we approach this by encoding the memory model into the program

## Current Status

- I have passed the state examination and thesis proposal defence on the 16th January
  - a lot of time consumed by the preparation


. . .

- work on efficiency of memory model simulation
  - decreasing nondeterminism
  - speed
  - splitting `x86`-TSO as a separate special case to increase its efficiency

. . .

- additional topic: SV-COMP 2019
  - we will try to participate
  - I will be preparing DIVINE for this

## Plans for 2018

- efficiency of analysis of programs running under relaxed memory models
  - main focus on `x86`-TSO
  - lock-free data structures (C/C++ atomics)

. . .

- benchmarks for relaxed memory models
  - SV-COMP demo category?

. . .

- publication about memory-model-aware analysis

. . .

- comparison with other tools

. . .

- participation in SV-COMP 2019
  - submission in fall 2018
