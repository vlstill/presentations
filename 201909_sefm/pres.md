---
vim: spell spelllang=en tw=80 fo+=t
title: "Local Nontermination Detection\\newline for Parallel C++ Programs"
author:
    - Vladimír Štill
    - Jiří Barnat
header-includes:
    - \input{defs}
lang: english
date: 20th Septempler 2019
...


## Motivation

\center

"Would you trust a program which was verified, but not tested?"

. . .

\vskip4\bigskipamount DEMO

. . .

\vskip4\bigskipamount
… at the very least, we should not blindly trust safety checking

## Safety Checking Parallel Programs

- targeting assertion violations, memory corruption, data races
- primarily caused by thread interleaving
- or by relaxed memory

. . .

- if the program might not terminate…

  - the tool might not terminate
  - or it might report there are no safety violations
    \pause (correctly)

. . .

- not enough for parallel programs

## (Non)Termination Checking

- check that the whole program terminates

. . .

\bigskip\bigskip

- or checks that certain parts of it terminate
  - critical sections
  - waiting for condition variables, threads…
  - user-defined parts

## Local Nontermination Detection for Parallel Programs

- we aim at nontermination caused by unintended parallel interactions
- not at complex control flow & loops
- should be easy to specify
- should not report nontermination spuriously
- should be useful for analysis of services/servers

. . .

\bigskip

- build on explicit-state model checking \textrightarrow{} finite-state
  programs\
  (with possibly infinite behaviour)
- user can specify what to check

\bigskip

```{.cpp}
bool x = true;
while ( true ) { x = !x; }
```

\bigskip
\begin{tikzpicture}[>=latex,>=stealth',auto,node distance=2cm,semithick,initial text=]
  \node[state,initial] (t) {$x$};
  \node[state] (f) [right of = t] {$\lnot x$};

  \path[->, shorten >=1pt]
	(t) edge[bend left] (f)
	(f) edge[bend left] (t)
  ;
\end{tikzpicture}
