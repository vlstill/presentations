---
vim: spell spelllang=en fo+=t tw=80
title: "On Solving Quantified Bit-Vectors using Invertibility Conditions"
author:
    - Aina\ Niemetz
    - Mathias\ Preiner
    - Andrew\ Reynolds
    - Clark\ Barrett
    - Cesare\ Tinelli
header-includes:
    - \input{defs}
    - \institute{}
lang: english
date: 11th May 2018
...

## Introduction

- solving quantified bitvector formulas
- using counterexample guided quantifier instantiation
    - essentially refining possible values of existential variables
    - the technique aims to generalize counterexamples in the refinement loop

## Inverting (In)equalities

- SMT literals: $s \bowtie t$ for some term $s$ and $t$ and ${\bowtie} \in \{
  \approx, \not\approx, <, >, … \}$
- if they contain only one occurrence of variable $x$ they might be solvable for
  $x$:
  - $x + a \approx b \rightarrow x = b - a$

. . .

- more complex solutions can be described using the *Hilbert choice exression*
  - $\varepsilon x.\ \varphi(x)$ is an expression which has a value of a witness of
    truth of $\varphi(x)$ if such a witness exists (and arbitrary value
    otherwise)

    . . .

  - $\varepsilon x.\ x + 3 = 5$ has value 2

      . . .

  - $\varepsilon x.\ x > 5$ can have arbitrary value larger then 5

      . . .

  - $\varepsilon x.\ x > 6 \land x < 3$ can have arbitrary value

## Invertibility Conditions

- most SMT equations are only conditionally invertible
    - $x << s \approx t$ is invertible iff \pause $t$ ends with at least $s$
      zeroes \pause ($(t >> s) << s \approx t$) \pause
      - solution: $x = t >> s$ \pause
    - $\mathit{concat}(x, s) \approx t$ is invertible if the last $|s|$ bits of
      $t$ are equal to\ $s$

      . . .

- if the invertibility condition holds, Hilbert choice expression is a witness
  of truth

  . . .

- the paper introduces invertibility conditions for all SMT-LIB bitvector operations
  - either manually constructed
  - or created using syntax-guided synthesis
  - and validated by SMT solvers for bitwidths up to 65

## Counterexample-Guided Quantifier Instantiation

- suppose the formula is of form $\exists \mathbf{y} \forall \mathbf{x}.\
  \psi(\mathbf{y}, \mathbf{x})$

- $\Gamma := \emptyset$
- Repeat:
    1.  if $\Gamma$ is unsatisfiable, return *unsat*
    2.  otherwise, if $\Gamma' = \Gamma \cup \{ \lnot \psi(\mathbf{y},
        \mathbf{x}) \}$ is unsatisfiable, return *sat*
    3.  otherwise,
        - let $\mathcal{I}$ be a model of $\Gamma'$ and $\mathbf{t} =
          \mathcal{S}(\mathbf{x}, \mathcal{I}, \Gamma')$
        - $\Gamma := \Gamma \cup \{ \psi(\mathbf{y}, \mathbf{t}) \}$

\bigskip

- intuitively, $\Gamma$ is a set of constrains on $\mathbf{y}$

- $\mathcal{S}$ returns a vector of terms which can be substituted to $\mathbf{x}$
    - these terms do not contain free $\mathbf{x}$
    - should somehow abstract the model $\mathcal{I}$

      . . .

    1.  collects all literals from $\Gamma'$
    #.  projects them into simplified conditions
    #.  linearizes them
    #.  selects and solves one

## 
