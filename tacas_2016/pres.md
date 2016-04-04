---
title: "DIVINE: Explicit-State LTL Model Checker"
author:
    - "**Vladimír Štill**"
    - Petr Ročkai
    - Jiří Barnat
authorshort: V. Štill, P. Ročkai, J. Barnat
header-includes:
    - \usepackage{divine}
lang: english
date: 7th April 2016
_aspectratio: 169
...

## Verification of Unit Tests for Real-World C++ Code

*   explicit-state model checking of parallel programs
*   using LLVM as an intermediate language
*   standard C, C++, and `pthread` libraries nearly completely supported
*   exact verification, no bounding, all meaningful interleavings
*   supports all language features (including exceptions)
*   no symbolic data support (yet)

## Verification Properties

*   assertion safety
*   memory safety
*   memory leaks
*   mutex safety
*   uninitialized memory use
*   LTL
*   gives counterexample if property does not hold

## Powerful Reduction Techniques

*   state space size reduction ($\tau+$ reduction)
    *   omission of uninteresting interleavings
*   memory footprint reduction (lossless compression)
*   parallel and distributed search

## DIVINE: Summarization

*   a powerful explicit-state model checker for unit tests of real-world C++ programs
*   specializes on parallel programs
*   verifies large scale of properties
*   got 951 points out of 1240 in concurrency category
*   <https://divine.fi.muni.cz>

. . .

\hfill\includegraphics[width=3em]{divine-logo.png}\
\hfill Thank you!
