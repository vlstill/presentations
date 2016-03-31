---
title: "DIVINE: Explicit-State LTL Model Checker"
author:
    - "**Vladimír Štill**"
    - Petr Ročkai
    - Jiří Barnat
authorshort: Vladimír Štill et al.
header-includes:
    - \usepackage{divine}
lang: english
date: 7th April 2016
_aspectratio: 169
...

## DIVINE: an Overview

*   verification of real-world parallel C++ code

\iffalse
\fi

\bigskip

*   safety and LTL properties

\bigskip

*   powerfull reduction techniques

## Verification of Real-World Parallel C++ Code

*   explicit-state model checking
*   using LLVM as an itermediate language
*   standard C, C++, and `pthread` libraries nearly completely supported
*   exact verification, no bounding, all meaningful interleavings
*   support all language features (including exceptions)
*   no symbolic data support (yet)

## Verification Properties

*   assertion safety
*   memory safety
*   memory leaks
*   mutex safety
*   uninitialized memory use
*   LTL
*   gives counterexample if property does not hold

## Powerfull Reduction Techniques

*   state space size reduction ($\tau+$ reduction)
    *   ommision of uninteresting interleavings
*   memory footprint reduction (lossless compression)
*   parallel and distributed search

## DIVINE: Summarization

*   a powerfull explicit-state model checker for real-world C++
*   C++ library support and full C++ language features support
*   specializes on parallel programs
*   verifies large scale of properties
*   <https://divine.fi.muni.cz>

. . .

\hfill\includegraphics[width=3em]{divine-logo.png}\
\hfill Thank you!
