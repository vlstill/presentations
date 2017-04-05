---
vim: spell spelllang=en tw=80 fo+=t
title: Complexity of Fixed-Size Bit-Vector Logics
author:
    - Gergely Kovásznai
    - Andreas Fröhlich
    - Armin Biere
    - \
    - presented by Vladimír Štill
header-includes:
    - \newcommand{\redon}[2]{{\only<#1>{\color{red}}#2}}
    - \usepackage{divine}
    - \usetikzlibrary{fit,backgrounds}
    - \usepackage{eulervm}
    - \newcommand{\NP}{\ensuremath{\mathrm{NP}}}
    - \newcommand{\PSPACE}{\ensuremath{\mathrm{PSPACE}}}
    - \newcommand{\NEXPTIME}{\ensuremath{\mathrm{NEXPTIME}}}
    - \newcommand{\SAT}{\ensuremath{\mathsf{SAT}}}
    - \newcommand{\QBF}{\ensuremath{\mathsf{QBF}}}
    - \newcommand{\DQBF}{\ensuremath{\mathsf{DQBF}}}
    - \newcommand{\bv}[2]{\ensuremath{#1^{[#2]}}}
    - \newcommand{\bvnot}{{\sim}}
    - \newcommand{\bvand}{\mathbin{\&}}
    - \newcommand{\bvor}{\mathbin{|}}
    - \institute{}
lang: english
date: 7th April 2017
...

## Complexity Refresh

\begin{block}{\SAT{}}

satisfiability of unqualified boolean formula

$$(x \lor y \lor ¬z) \land (¬y \lor ¬x)$$

\begin{itemize}
    \item \NP-complete
\end{itemize}
\end{block}

. . .

### \QBF

is given quantified formula true?

$$\forall a \exists y \forall z.\ (x \lor y \lor ¬z) \land (¬y \lor ¬x)$$

*   every existential variable can depend on value of any universal variables
    before it in the prefix
*   \PSPACE-complete

## Complexity: DQBF

**Dependency Quantified Boolean Formulas** (\DQBF)

*   every existentially quantified variable specifies which universally
    quantified variables it can depend on

$$\forall x_1, x_2, x_3\,\exists y_1 (x_1, x_3), y_2(x_2, x_1).\ \Phi$$

*   more general then \QBF
*   satisfiability is \NEXPTIME-complete

## Refresh: Theory of Bitvectors

first order logic with bit-vectors -- numbers of fixed bit width and operations
on them

*   variables and constants have to have specified bit-width ($x^{[32]}$)
*   arithmetic operations (modular): $+$, $\times$, $\div$, $\bmod$
*   bitwise operations: $\&$, $|$, $\oplus$ (XOR), $\sim$ (complement)
*   shifts: $\gg_s$ (arithmetic shift right), $\gg_u$ (logical shift right),
    $\ll$
*   comparison and equality: $=$, $\neq$, $\le_u$ (unsigned), $\le_s$
    (signed), …
*   \textsc{extract}, $\circ$ (concat)

. . .

*   unlike other definitions, the definition in the paper does not distinguish
    booloans and bit-vectors of size 1

    . . .

    $$ \forall \bv{x}{32} \exists \bv{y}{32}.\ \bv{x}{32} + \bv{y}{32} = \bv{0}{32}
    \bvand \bv{x}{32} \neq \bvnot\bv{y}{32}$$

    *   the matrix should result in bit-vector of size 1

## Bitvector Formula Encoding Size

We will consider bit-vector formulas in *flat form*

*   with no nested relational operators

. . .

*   bit-vector constants are encoded in binary -- length is logarithmic to the
    value
*   two possibilities of encoding of scalars (bit-widths, arguments of \textsc{extract},
    …)
    *   binary -- more natural
    *   unary -- larger formulas, but easier complexity analysis

## Bitvector Formula Encoding Size

let $\Phi = Q_0\bv{x_0}{n_0}\,Q_1\bv{x_1}{n_1}…Q_k\bv{x_k}{n_k}.\ \bv{t}{1}$ be
quantified bit-vector formula,

*   $|\Phi| = \left|\bv{x_0}{n_0}\right| + \dots + \left|\bv{x_k}{n_k}\right| + \left|\bv{t}{1}\right|$

\includegraphics[page = 14, clip, trim=1.5cm 11.5cm 1.5cm 5.42cm, width=\textwidth]{paper.pdf}

## Complexity Overview

...

# Logics with Unary Encoding

## Refresh: Bit-Blasting

*   transformation from bit-vector formula to boolean formula
*   each bit-vector variable of length $n$ is replaced with $n$ boolean
    variables
*   operations are implemented similarly to hardware circuits (i.e. adder)

## Quantifier-free Bit-Vector Logic, Unary Encoding

QF\_BV1 is \NP-complete

*   is in \NP{}
    *   by bit-blasting: since bit-widths are encoded in unary, bit blasting
        is polynomial

. . .

*   is \NP-hard
    *   by reduction from \SAT{}
    *   every \SAT{} formula can be transformed to QF\_BV1 formula by replacing
        every boolean variable with variable of bit-width 1

## Ackermann Reduction

a way to remove uninterpreted functions from a formula

*   given term with uninterpreted functions

$$ F(F(x)) = 0 $$

1.  number the occurrences of uninterpreted function

    $$ \underbrace{F(\ \overbrace{F(x)}^{f_1}\ )}_{f_2} = 0 $$

2.  replace each function with a fresh variable

    $$ f_2 = 0 $$

3.  add functional consistency constraints for every pair of instances of the
    same function

    $$ ((x = f_1) \implies (f_2 = f_1)) \land (f_2 = 0) $$

## Ackermann Reduction Example {.t}

\begin{latex}

\only<1>{
\[ x_1 \neq x_2 \lor F(x_1) = F(x_2) \lor F(x_1) \neq F(x_3) \]
}
\only<2->{
\[ x_1 \neq x_2 \lor \underbrace{F(x_1)}_{f_1} = \underbrace{F(x_2)}_{f_2} \lor \underbrace{F(x_1)}_{f_3} \neq \underbrace{F(x_3)}_{f_3} \]
}

\pause \pause \center transformed to

\only<3>{
\[ x_1 \neq x_2 \lor f_1 = f_2 \lor f_1 \neq f_3 \]
}

\only<4>{

\[  \left( \begin{matrix}
        (x_1 = x_2 \implies f_1 = f_2) \land {} \\
        (x_1 = x3 \implies f_1 = f_3) \land {} \\
        (x_2 = x_3 \implies f_2 = f_3)
    \end{matrix} \right) \land
    (x_1 \neq x_2 \lor f_1 = f_2 \lor f_1 \neq f_3)
\]
}

\end{latex}

## Adding Uninterpreted Functions

QF\_UFBV1 is \NP-complete

*   is in \NP{}
    *   use Ackermann reduction
    *   quadratic growth

. . .

*   is \NP-hard: $\text{QF\_BV1} \subset \text{QF\_UFBV1}$

## Adding Quantifiers

BV1 is \PSPACE-complete

*   it is in \PSPACE -- by bit-blasting
*   it is \PSPACE-hard -- \QBF{} is special case of BV1

## Adding Quantifiers and Uninterpreted Functions

*   TODO

# Logics with Binary Encoding

## Quantifier-free Bit-Vector Logic, Binary Encoding

QF\_BV2 is \NEXPTIME-complete

*   it is in \NEXPTIME{}
    *   bit-blasting can cause exponential growth of the formula
    *   bit-blasted formula is solvable in \NP

. . .

*   we want to show that it is \NEXPTIME-hard

    *   by reduction of \DQBF{} to QF\_BV2 (\DQBF{} is \NEXPTIME-complete)

## Reducing \DQBF{} to QF\_BV2

\global\def\rbase{
    we have formula of form
    \[ \forall u_1, u_2, \ldots, u_n\,\exists e_1(u_{1,1},
    \ldots u_{1,i_1}), \ldots, e_m(u_{m,1}, \ldots u_{m,i_m}).\ \Phi \]}
\rbase{}

. . .

*   we want to check that for all possible assignments to universally-quantified
    variables, we can choose existentially-quantified variables such that $\Phi$
    holds, and existentially-quantified variables can depend only on specified
    universally-quantified variables

    . . .

    *   that is, value of an existentially-quantified variable has to remain
        same as long as only variables it does not depend on changed

    . . .

*   this check should be encoded as quantifier-free formula of bit-vector logic
    which is satisfiable if the original formula holds

## Reducing \DQBF{} to QF\_BV2

\rbase{}

*   idea: encode in the bit-vector formula all possible assignments of the
    universally-quantified variables

. . .

*   and allow only such choices of existentially-quantified variables which
    satisfy given constraints

. . .

*   and check that all the cases are satisfiable

## Reducing \DQBF{} to QF\_BV2 {.t}

$$ \Phi = \forall x, y\,\exists z(y).\ (x \lor y \lor z) \land (y \lor \lnot z)
$$

\begin{latex}

\only<2-4>{
what if we only wanted to check satisfiability of Matrix? (Using QF\_BV2
solver)

\[ (\bv{X}{1} \bvor \bv{Y}{1} \bvor \bv{Z}{1}) \bvand (\bv{Y}{1} \bvor
\bvnot\bv{Z}{1})
\]
}

\only<3-4>{
How can we check all possible assignments of X, Y, Z at once ($n$
universally-quantified variables)?
}

\only<4>{
\begin{itemize}
    \item there are $2^n$ assignments
    \item represent each universally-quantified variable with bitvector of
    length $2^n$
    \item let first bit of all variables represent first possible assignment,
    second bit represent second assignment…
\end{itemize}
}

\only<5->{
represent each universally-quantified variable with bitvector of length $2^n$
(where $n$ is the number of universally-quantified variables)

\[ (\bv{X}{4} \bvor \bv{Y}{4} \bvor \bv{Z}{4}) \bvand (\bv{Y}{4} \bvor
\bvnot\bv{Z}{4}) = \bvnot\bv{0}{4}
\]

\only<6->{

}
}


\end{latex}

## x
