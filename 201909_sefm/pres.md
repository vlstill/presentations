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

## What is Nontermination?

```{.cpp}
mutex mtx;
void w() { mutex.lock(); x++; mutex.unlock(); }
int main() { thread t0(w), t1(w); t0.join(); t1.join(); }
```

\bigskip Does this program terminate? \pause … yes

## What is Nontermination?

```{.cpp}
atomic< bool > spin_lock;
void w() {
    while (spin_lock.exchange(true)) { /* wait */ }
    x++;
    spin_lock = false;
}
int main() { thread t0(w), t1(w); t0.join(); t1.join(); }
```

\bigskip Does this program terminate? \pause … yes

\pause\bigskip

But there is an *infinite run*:

\newcommand{\ta}[1]{\text{\textcolor{blue}{\texttt{[t0: #1]}}}}
\newcommand{\tb}[1]{\text{\textcolor{red}{\texttt{[t1: #1]}}}}

$\ta{spin\_lock.exchange(true) \textrightarrow{} false}$\
$\tb{spin\_lock.exchange(true) \textrightarrow{} true}^\omega$ (repeats infinitely)

\pause\medskip{} **but only because `t0` is not allowed to run**

## What is Nontermination?

```{.cpp}
void w() {
    while ( true ) {
        while (spin_lock.exchange(true)) { /* wait */ }
        x++;
        spin_lock = false;
    }
}
```

Does every *wait* end? \pause yes\pause ?\pause

\bigskip
$\ta{spin\_lock.exchange(true) \textrightarrow{} false}$\
$\big(\tb{spin\_lock.exchange(true) \textrightarrow{} true}$\
$\phantom{\big(}\ta{x++}$\
$\phantom{\big(}\ta{spin\_lock = false}$\
$\phantom{\big(}\ta{spin\_lock.exchange(true) \textrightarrow{} false}\big)^\omega$\

\medskip
**both threads can run**

## What is Nontermination?

$\ta{spin\_lock.exchange(true) \textrightarrow{} false}$\
$\big(\tb{spin\_lock.exchange(true) \textrightarrow{} true}$\
$\phantom{\big(}\ta{x++}$\
$\phantom{\big(}\ta{spin\_lock = false}$\
$\phantom{\big(}\ta{spin\_lock.exchange(true) \textrightarrow{} false}\big)^\omega$\
\medskip

- this run requires a scheduler which allows `t1` to run only if `t0` is in
  the critical section \pause

- never happens in reality\pause

- for realistic schedulers infinite run does not imply nontermination

## What is Nontermination?

**Nontermation**

- a program does not terminate if it can reach a point from which it cannot
  reach its end

\pause\bigskip

**Resource Section**

- a block of code with an identifier
- delimited in the source code

\pause\bigskip

**Local Nontermation**

- a resource section does not terminate if the program can reach a point *in the
  resource section* from which it cannot reach *the corresponding resource
  section end*

## Detecting Nontermination

- a program does not terminate if it can reach a point from which it cannot
  reach its end

  . . .

- detect nontrivial terminal strongly connected components

\bigskip
\begin{tikzpicture}[>=latex,>=stealth',auto,node distance=2cm,semithick,initial
                    text=, ->, shorten >=1pt, initial distance = 1cm]
  \tikzstyle{state}=[circle, draw, minimum size=0.75cm]
  \tikzstyle{scc}=[draw, dashed, inner sep = 1.3em, rounded corners=0.4em]
  \node[state,initial] (c1a) {};
  \node[state] (c1b) [right of = c1a] {};
  \node[state] (c1c) [below of = c1a] {};

  \path
	(c1a) edge[bend left] (c1b)
	(c1b) edge[bend left] (c1c)
    (c1c) edge[bend left] (c1a)
    (c1b) edge[bend left] (c1a)
  ;

  \node[scc, fit = (c1a) (c1b) (c1c)] (c1) {};

  \node[state] (c2a) [right of = c1b] {};
  \path (c1b) edge (c2a);
  \node[scc, fit=(c2a)] (c2) {};

  \node[state] (c3a) [below of = c2a] {$\bot$};
  \path (c2a) edge (c3a);
  \node[scc, fit=(c3a)] (c3) {};

  \node[state] (c4a) [right of = c2a] {};
  \node[state] (c4b) [right of = c4a] {};
  \path
    (c2a) edge (c4a)
    (c4a) edge[bend left] (c4b)
    (c4b) edge[bend left] (c4a)
  ;
  \node[scc, fit=(c4a) (c4b), line width=3pt] (c4) {};
  \node[anchor=south] at (c4.south) {nontriv. terminal SCC};
\end{tikzpicture}
