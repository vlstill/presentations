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
    - \newcommand{\NEXP}{\ensuremath{\mathrm{NEXP}}}
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

satisfiability of quantifier-free boolean formula

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

    *   matrix of a formula is a term
    *   it should result in bit-vector of size 1

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


\begin{tabularx}{\textwidth}{l|CCCC|}
           & \multicolumn{4}{c|}{quantifiers} \\
           & \multicolumn{2}{c}{no} & \multicolumn{2}{c|}{yes} \\ \hline
           & \multicolumn{2}{c}{uninterpreted funs} & \multicolumn{2}{c|}{uninterpreted funs} \\
  encoding & no  & yes     & no & yes \\ \hline
  unary    & QF\_BV1 \NP & QF\_UFBV1 \NP     & BV1 \PSPACE & UFBV1 \NEXP \\ \hline
  binary   & QF\_BV2 \NEXP & QF\_UFBV2 \NEXP & BV2 $\mathrm{AEXP(poly)}$ & UFBV2 2-\NEXP \\
\end{tabularx}

# Logics with Unary Encoding

## Refresh: Bit-Blasting

*   transformation from bit-vector formula to boolean formula
*   each bit-vector variable of length $n$ is replaced with $n$ boolean
    variables
*   operations are implemented similarly to hardware circuits (i.e. adder)

## Quantifier-free Bit-Vector Logic, Unary Encoding

QF\_BV1 is \NP-complete

. . .

*   is in \NP{}
    *   by bit-blasting: since bit-widths are encoded in unary, bit-blasting
        is polynomial

. . .

*   is \NP-hard
    *   by reduction from \SAT{}
    *   every \SAT{} formula can be transformed to QF\_BV1 formula by replacing
        every boolean variable with variable of bit-width 1

## Refresh: Uninterpreted functions

*   functions for which we know only their arity, type of inputs and type or
    result

. . .

$$ \left(x > 0 \lor f\left(x\right) > 0\right) \land x = 0 \land f\left(0\right)
= 0 $$

. . .

*   but we still know they are functions

## Ackermann Reduction

a way to remove uninterpreted functions from a (quantifier-free) formula

*   given term with uninterpreted functions
    $$ F(F(x)) = 0 $$

. . .

1.  number the occurrences of uninterpreted functions
    $$ \underbrace{F(\ \overbrace{F(x)}^{f_1}\ )}_{f_2} = 0 $$

    . . .

2.  replace each function with a fresh variable
    $$ f_2 = 0 $$

    . . .

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

. . .

*   is in \NP{}
    *   use Ackermann reduction
    *   quadratic growth

. . .

*   is \NP-hard: $\text{QF\_BV1} \subset \text{QF\_UFBV1}$

## Adding Quantifiers

BV1 is \PSPACE-complete

. . .

*   it is in \PSPACE{} -- by bit-blasting

. . .

*   it is \PSPACE-hard -- \QBF{} is special case of BV1

## Adding Quantifiers and Uninterpreted Functions

UFBV1 is \NEXPTIME-complete

*   can be shown reducible to and from Effectively Propositional Logic (EPR),
    proof not in the paper

# Logics with Binary Encoding

## Quantifier-free Bit-Vector Logic, Binary Encoding

QF\_BV2 is \NEXPTIME-complete

. . .

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

*   we want to check that
    *   for all possible assignments to universally-quantified variables,
    *   we can choose existentially-quantified variables
    *   such that $\Phi$ holds,
    *   and existentially-quantified variables can depend only on specified
        universally-quantified variables

    . . .

    *   that is, value of an existentially-quantified variable has to remain
        same as long as only variables it does not depend on changed

    . . .

*   this check should be encoded as quantifier-free formula of bit-vector logic
    (of polynomial size) which is satisfiable iff the original formula holds

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

$$ \Phi = \forall x, y, z\,\exists w(y).\ (x \lor y \lor w) \land (y \lor \lnot w)
$$

\begin{latex}

\only<2-4>{
what if we only wanted to check satisfiability of Matrix? (Using QF\_BV2
solver)

\[ (\bv{X}{1} \bvor \bv{Y}{1} \bvor \bv{W}{1}) \bvand (\bv{Y}{1} \bvor
\bvnot\bv{W}{1})
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

\only<5-6>{\noindent\smallskip
\[ (\bv{X}{8} \bvor \bv{Y}{8} \bvor \bv{Z}{8}) \bvand (\bv{Y}{8} \bvor
\bvnot\bv{Z}{8}) = \bvnot\bv{0}{8}
\]
}

\only<7-8>{\noindent\smallskip
\[ (\bv{01010101}{8} \bvor \bv{00110011}{8} \bvor \bv{W}{8}) \bvand (\bv{00110011}{8} \bvor
\bvnot\bv{W}{8}) = \bvnot\bv{0}{8} \]
}

\only<9-10>{\noindent\smallskip
\[ (\bv{X}{8} \bvor \bv{Y}{8} \bvor \bv{W}{8}) \bvand (\bv{Y}{8} \bvor
\bvnot\bv{W}{8}) = \bvnot\bv{0}{8} \]
{\[ \only<9>{\color{white}}{} \bvand (\bv{X}{8} \ll 1 = \bvnot\bv{X}{8}) \bvand (\bv{Y}{8} \ll 2 =
\bvnot\bv{Y}{8}) \bvand (\bv{Z}{8} \ll 4 = \bv{Z}{8})
\]}
}

\only<6-8>{
replace universally-quantified variables ($U_0, \ldots, U_{n-1}$) with constants of form
\begin{itemize}
    \item \bv{01010101\dots{}0101}{2^n} for $U_0$
    \item \bv{00110011\dots{}0011}{2^n} for $U_1$
    \item $\dots$
    \item \bv{00\dots{}0011\dots{}11}{2^n} for $U_{n-1}$
\end{itemize}

\only<8>{\par not really, these constants are exponential to the size of the
original formula!}
}

\only<9->{\vspace{-\bigskipamount}
constrain the universally-quantified variables ($u_0, \ldots, u_{n-1}$) so they can
only have the value of the aforementioned constants

\begin{itemize}
    \item $\bv{u_0}{2^n} \ll 2^0 = \bvnot\bv{u_1}{2^n}$ (01010101\dots{}0101)
    \item $\bv{u_1}{2^n} \ll 2^1 = \bvnot\bv{u_2}{2^n}$ (00110011\dots{}0011)
    \item $\dots$
    \item $\bv{u_i}{2^n} \ll 2^i = \bvnot\bv{u_i}{2^n}$
\end{itemize}
}
}

\end{latex}

## Reducing \DQBF{} to QF\_BV2 {.t}

\begin{latex}
\[ \Phi = \forall x, y, z\,\exists w(y).\ (x \lor y \lor w) \land (y \lor \lnot w) \]
\begin{center}→\end{center}
\[ (\bv{X}{8} \bvor \bv{Y}{8} \bvor \bv{W}{8}) \bvand (\bv{Y}{8} \bvor
\bvnot\bv{W}{8}) = \bvnot\bv{0}{8} \]
\[ {} \bvand (\bv{X}{8} \ll 1 = \bvnot\bv{X}{8}) \bvand (\bv{Y}{8} \ll 2 =
\bvnot\bv{Y}{8}) \bvand (\bv{Z}{8} \ll 4 = \bv{Z}{8})
\]

\only<2-3>{
\begin{itemize}\item still, the existentially-quantified variables can depend on any
universally-quantified variables\end{itemize}
}

\only<3->{
\begin{itemize}
    \item we have to ensure the value of existentially-quantified variable does
    not change unless any universally-quantified variable it depends on changed

    \only<4>{
    \begin{itemize}
        \item consider existentially-qualified variable $e$,
        universally-qualified variable $u$, and two universal assignments
        $\alpha$, $\beta$, such that $\alpha$ differs from $\beta$ only in value
        of $u$
        \item if $e$ does not depend on $u$, then value of $e$ for both
        $\alpha$ and $\beta$ has to be the same
    \end{itemize}
    }


    \only<5-8>{
    \medskip
    \begin{minipage}[t]{0.3\textwidth}
    \begin{tabular}{l|l}
        X & \texttt{01010101} \\
        Y & \texttt{00110011} \\
        Z & \texttt{00001111} \\
        W & \texttt{abcdefgh}
    \end{tabular}
    \end{minipage}
    \only<6->{
    \begin{minipage}{0.55\textwidth}
        if $W$ is independent of \only<6>{$X$}\only<7>{$Y$}\only<8>{$Z$} we have
        to ensure that
        \only<6>{$(a = b) \land (c = d) \land (e = f) \land (g = h)$}
        \only<7>{$(a = c) \land (b = d) \land (d = f) \land (f = h)$}
        \only<8>{$(a = e) \land (b = f) \land (c = g) \land (d = h)$}
    \end{minipage}
    }
    }

    \only<9->{
    \begin{itemize} \pause\pause\pause \pause\pause\pause \pause\pause
        \item consider $E_i$ which does not depend on $U_m$ (which correspond to
        assignments of $e_i$ and $u_m$ respectively) \pause
        \item zero-out assignments of $e_i$ in which $u_m$ is \emph{false}: $E_i
        \bvand \bvnot U_m$
        \item $E_i \bvand \bvnot U_m = (E_i \ll 2^m) \bvand \bvnot U_m$
    \end{itemize}
    }
\end{itemize}
}

\end{latex}

## Reducing \DQBF{} to QF\_BV2 {.t}

\begin{latex}
\[ \Phi = \forall x, y, z\,\exists w(y).\ (x \lor y \lor w) \land (y \lor \lnot w) \]
\begin{center}→\end{center}
\[ (\bv{X}{8} \bvor \bv{Y}{8} \bvor \bv{W}{8}) \bvand (\bv{Y}{8} \bvor
\bvnot\bv{W}{8}) = \bvnot\bv{0}{8} \]
\[ {} \bvand (\bv{X}{8} \ll 1 = \bvnot\bv{X}{8}) \bvand (\bv{Y}{8} \ll 2 =
\bvnot\bv{Y}{8}) \bvand (\bv{Z}{8} \ll 4 = \bv{Z}{8})
\]
\[{} \bvand \left(\bv{W}{8} \bvand \bvnot \bv{X}{8} = \left(\bv{W}{8} \ll \bv{1}{8}\right) \bvand \bvnot \bv{X}{8}\right)
\]
\[{} \bvand \left(\bv{W}{8} \bvand \bvnot \bv{Z}{8} = \left(\bv{W}{8} \ll \bv{4}{8}\right) \bvand \bvnot \bv{Z}{8}\right)
\]
\end{latex}

*   add the constraints of form $E_i \bvand \bvnot U_m = (E_i \ll 2^m) \bvand
    \bvnot U_m$ for every $U_m$ on which $E_i$ does not depend

## Quantified Bit-Vector Logic with Binary Encoding

UFBV2 is 2-\NEXPTIME-complete

. . .

*   in 2-\NEXPTIME{}
    *   re-encoding to unary
    *   UFBV1 is \NEXPTIME-complete

. . .

*   it is \NEXPTIME-hard
    *   proof by reduction from *square domino tiling problem*

## Square Domino Tiling Problem

\begin{block}{Domino System $D$}
\[ D = \langle T, H, V, n \rangle \]

\begin{itemize}
    \item $T$ -- finite set of \emph{tile types}, $T = [0, k-1]$
    \item $H, V \subseteq T \times T$ -- horizontal and vertical matchning
    conditions
    \item $n \ge 1$ -- size parameter encoded in \emph{unary} format
\end{itemize}
\end{block}

. . .

\bigskip

### $f(n)$-square tiling problem for $D = \langle T, H, V, n \rangle$

problem of finding a mapping $\lambda : [0, f(n)-1] \times [0, f(n)-1]
\rightarrow T$ such that

*   first row starts with the start tile (tile 0)
*   last row ends with the terminal tile (tile $k - 1$)
*   all horizontal and vertical matching conditions are met
*   *this problem is $\mathsf{NTIME(f(n))}$-complete*

## Reducing $2^{(2^n)}$ Square Tiling to UFBV2

given $D = \langle T = [0, k-1], H, V, n \rangle$ encode

*   tiles with bit-vectors of bit-width $\lceil \log_2 k \rceil = w$
*   the tiling with uninterpreted function $\lambda$ which assigns to a pair of
    indices the tile which resides on this index:
    $\bv{\lambda}{w}\left(\bv{i}{2^n}, \bv{j}{2^n}\right)$

## Reducing $2^{(2^n)}$ Square Tiling to UFBV2 {.t}

given $D = \langle T = [0, k-1], H, V, n \rangle$

\begin{latex}
\begin{eqnarray*}
    \forall \bv{i}{2^n}\,\forall \bv{j}{2^n}.\ \lambda(0, 0) = 0 \land
    \lambda\left(2^{(2^n)} - 1, 2^{(2^n)} - 1\right) = k - 1 \\
    {} \land \left( j \neq 2^{(2^n)} - 1 \implies (\lambda(i, j), \lambda(i, j + 1)) \in H \right) \\
    {} \land \left( i \neq 2^{(2^n)} - 1 \implies (\lambda(i, j), \lambda(i + 1, j)) \in V \right) \\
\end{eqnarray*}
\end{latex}

## Reducing $2^{(2^n)}$ Square Tiling to UFBV2 {.t}

given $D = \langle T = [0, k-1], H, V, n \rangle$

\begin{latex}
\begin{eqnarray*}
    \forall \bv{i}{2^n}\,\forall \bv{j}{2^n}.\ \lambda(0, 0) = 0 \land
    \lambda\left(2^{(2^n)} - 1, 2^{(2^n)} - 1\right) = k - 1 \\
    {} \land \left( j \neq 2^{(2^n)} - 1 \implies \bigvee_{(t_1,t_2) \in  H} (\lambda(i, j) = t_1 \land \lambda(i, j + 1) = t_2) \right) \\
    {} \land \left( i \neq 2^{(2^n)} - 1 \implies \bigvee_{(t1, t_2) \in V} (\lambda(i, j) = t_1 \land \lambda(i + 1, j) = t_2) \right) \\
\end{eqnarray*}
\end{latex}

. . .

*   replace $2^{(2^n)} - 1$ with $\bvnot\bv{0}{2^n}$ to ensure
    polynomial size of the formula
