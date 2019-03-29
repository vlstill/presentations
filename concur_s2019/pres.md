---
vim: spell spelllang=en fo+=t tw=999
title: "Dynamic Reductions for Model Checking Concurrent Software"
author:
    - Henning Günther
    - Alfons Laarman
    - Ana Sokolova
    - \mbox{Georg Weissenbacher}
header-includes:
    - \input{defs}
    - \institute{}
lang: english
date: 29th March 2019
...

## Introduction

- reductions of state-space in context of Symbolic Model Checking (BMC/IC3)

    . . .

\includegraphics[width=\textwidth, page=7, clip, trim={1.8cm 17.4cm 1.8cm 1.8cm}]{paper.pdf}

- **Lipton reduction** (left) -- hides intermediate (local) states
- **partial order reduction** (right) -- selects only some representative paths

    . . .

- often reduction uses static global information about the program
  - hard to calculate for real-world cases

## Problems of Global Analysis

```{.c .numberLines}
void T1()        void T2()
    *p++;            b = 2;
    *q = 2;          c = 3;
                     *q = 1;

```

Pointer analysis results:
$$ b \in \mathrm{pointsTo}(p) \land c \notin \mathrm{pointsTo}(p) $$
$$ {} \land \mathrm{pointsTo}(p) \cap \mathrm{pointsTo}(q) = \emptyset $$

. . .

`*p++`{.c} and `q = 2`{.c} can both interfere with `T2`, but `*p++`{.c} interferes only with the `b = 2`{.c} statement

. . .

```{.c}
void T1_instr()
    if ( PC[T2] >= 3 )
        atomic { *p++; *q = 2; }
    else
        *p++;
        *q = 2;
```

## Motivation

\includegraphics[width=0.48\textwidth, page=3, clip, trim={7.4cm 3.4cm 2.1cm 16.5cm}]{paper.pdf}
\hfill
\includegraphics[width=0.48\textwidth, page=4, clip, trim={2cm 3.2cm 7.9cm 15.2cm}]{paper.pdf}

- data written only once

- pointers for which it suffices to check aliasing once

## Problem: Conditions Breaks Commutativity of Actions

```{.c .numberLines}
void T1()        void T2()
    *p++;            b = 2;
    *q = 2;          c = 3;
                     *q = 1;

```

suppose
$$ \{b, c\} \cap \Big{(}\mathrm{pointsTo}(p) \cup \mathrm{pointsTo}(q)\Big{)} = \emptyset $$
$$ {} \land \mathrm{pointsTo}(p) \cap \mathrm{pointsTo}(q) = \emptyset $$

- then `*p++`{.c} can be swapped with `b = 2` in the trace (it *commutes*)
- similarly for other actions

    . . .

- but `if ( PC[T2] >= 3 )`{.c} does not commute with `b = 2`

    . . .

- **solution**: bisimilarity of states in the branches

## Comutativity

Let $\aarr$, $\barr$ be two binary relations (actions). $\stackrel{\alpha}{\rightarrow};\stackrel{\beta}{\rightarrow}$ their sequential composition.

* $\aarr \mathbin{\bowtie} \barr \define \aarr; \barr = \barr; \aarr$ (both commute)

  . . .

* $\aarr \mathbin{\stackrel{\rightarrow}{\bowtie}} \barr \define \aarr; \barr \subseteq \barr; \aarr$ ($\aarr$ right commutes with $\barr$)

    * can be moved right without loss of behaviour

    . . .

* $\aarr \mathbin{\stackrel{\leftarrow}{\bowtie}} \barr \define \aarr; \barr \supseteq \barr; \aarr$ ($\aarr$ left commutes with $\barr$)

## Lipton Reduction

an action $\aarr_i$ of thread $i$ is called:

* *right-mover* if it commutes to the right with all action of other threads
  (~ resource acquisition)
* *left-mover* if it commutes to the left with all action of other threads
  (~ resource release)
* *both-mover*, *none-mover*

. . .

\bigskip
Assume we want to verify safety condition $G \varphi$

. . .

**transaction**: a sequence of actions ${\stackrel{\alpha_1}{\rightarrow}_i};{\stackrel{\alpha_2}{\rightarrow}_i};\cdots;{\stackrel{\alpha_n}{\rightarrow}_i}$ such that for some $1 \le k < n$:

#.  all actions before $\stackrel{\alpha_k}{\rightarrow}_i$ are right-movers
#.  all actions after $\stackrel{\alpha_k}{\rightarrow}_i$ are left-movers
#.  actions after $\stackrel{\alpha_1}{\rightarrow}_i$ do not block
#.  $\varphi$ is not disabled by
    ${\stackrel{\alpha_1}{\rightarrow}_i};\cdots;{\stackrel{\alpha_{k - 1}}{\rightarrow}_i}$
    nor enable by
    ${\stackrel{\alpha_{k + 1}}{\rightarrow}_i};\cdots;{\stackrel{\alpha_n}{\rightarrow}_i}$

    . . .

… can be executed atomically

## Dynamic Movers

A state predicate (subset of states) $c_\alpha$ is a *dynamic both-moving condition* for a CFG edge $\alpha$ of thread $i$ if for all threads $j \neq i$:

- all $\alpha$ transitions starting in $c_\alpha$ commute with all transtions of other threads starting in $c_\alpha$

- neither $\alpha$ transitions nor transitions of any thread $j$ that start in $c_\alpha$ lead out of $c_\alpha$

. . .

**Useful conditions**

- reachability: e.g. non-reachability of pointer modification (in CFG) from the current location

    . . .

- pointer dereference: if pointers  are not modified in the future, their dereference commutes provided they point to different locations

    . . .

- monotonic atomic: atomic compare and swap where: i) the location is never modified by other action and ii) expected value is never equal to the written one

## Instrumentation and Reduction Idea

**Instrumentation**

*   break each action of thread $i$ into *pre*, *post* and *external* parts

*   use *pre* parts as long as the dynamic mover conditions hold, then switch to *post* parts

*   in *post* the mover conditions have to hold, otherwise switch to the *external* part before running the action

*   other threads can only run if $i$ is in *external* part

. . .

**Reduction**

- keep visible only *external* states and states needed to avoid cycles (based on control flow)

    . . .

- preserves safety properties

## Evaluation I

\includegraphics[width=\textwidth, page=16, clip, trim={1cm 3.8cm 1cm 1.9cm}]{paper.pdf}

## Evaluation II (SV-COMP)

\includegraphics[width=\textwidth, page=17, clip, trim={1cm 3.8cm 1cm 1.9cm}]{paper.pdf}
