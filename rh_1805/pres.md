---
vim: spell spelllang=en tw=80 fo+=t
title: "Memory-Model-Aware Analysis of Parallel Programs"
author:
    - Vladimír Štill
header-includes:
    - \input{defs}
lang: english
date: 10th May 2018
...

## Topic Recapitulation

- analysis of parallel programs running under relaxed memory models
  - effects of cache, out-of-order execution
  * depends on architecture

    . . .

    *   **Intel `x86`: store buffering**
    *   ARM/POWER: almost arbitrary reordering of loads and stores

. . .

- this is significantly harder then analysis of all interleavings of the program

. . .

- we approach this by encoding the memory model into the program

## Current Status: Relaxed Memory

rather efficient implementation of `x86`-TSO

- evaluation on nonsymbolic SV-COMP concurrency benchmarks
(54 in total)
- compared with CBMC and Nidhugg

    \begin{latex}
    \vspace{2ex}
    \setlength\tabcolsep{0.5em} %
    \begin{tabular}{lrrr} \toprule
        \textbf{nosym-tso (54)} & CBMC & DIVINE & Nidhugg \\ \midrule
        finished                & 20   & 23     & 24 \\
        TSO errors              &  3   & 10     &  3 \\
        uniqly solved           &  5   &  6     &  8 \\
        \bottomrule
    \end{tabular}
    \end{latex}

. . .

- future: integrate some ideas from stateless model checking
  - executions graphs
  - more advanced exploration algorithms

## Current Status: SV-COMP 2019

- DIVINE can now once more run (non-preprocessed) SV-COMP benchmarks
- preprocessing -- it is still not clear what is the future
- tested mostly on concurrency
- more results expended after summer


    \begin{latex}
    \vspace{2ex}
    \setlength\tabcolsep{0.5em} %
    \begin{tabular}{lrrr} \toprule
          \textbf{nosym-sc (54)} & CBMC & DIVINE & Nidhugg \\ \midrule
          correct                &   21 &   27   &   26    \\
          wrong                  &    0 &    0   &    1    \\
          uniqly solved          &    4 &    6   &    7    \\
        \bottomrule
    \end{tabular}
    \end{latex}

    - symbolic version not yet tested by me

## What Next?

- publication about `x86`-TSO (in progress)

\bigskip

- SV-COMP 2019

\bigskip

- more efficient relaxed memory model verification
