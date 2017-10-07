---
vim: spell spelllang=en tw=80 fo+=t
title: "Acceleration of Abstract Interpretation, Widening and Nowrrowing"
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \usepackage{sansmathfonts}
    - \usepackage[mathlf,textlf]{MyriadPro}
    - \usepackage[small]{eulervm}

    - \newcommand{\TODO}[1]{{\color{red}#1}}
    - \newcommand{\redon}[2]{{\only<#1>{\color{red}}#2}}
    - \setbeamersize{text margin left=1.2em, text margin right=1.2em}
    - \usetikzlibrary{arrows.meta}
    - \usepackage{tikz-cd}
    - \usetikzlibrary{arrows}
    - \newcommand{\gs}[4]{\begin{tikzcd}A \arrow[shift left=2pt]{r}{\alpha} \pgfmatrixnextcell B \arrow[shift left=2pt]{l}{\gamma}\end{tikzcd}}
    - \newcommand{\gsac}{\gs{A}{C}{\alpha}{\gamma}}
    - \DeclareMathOperator{\lfp}{lfp}
    - \DeclareMathOperator{\ite}{ite}
    - |
        \usepackage[ backend=biber
                   , style=alphabetic % numeric
                   , sortlocale=en_US
                   , bibencoding=UTF8
                   , maxnames=100
                   ]{biblatex}
        \DeclareSourcemap{
            \maps[datatype=bibtex, overwrite]{
                \map{
                    \step[fieldset=editor, null]
                    \step[fieldset=language, null]
                    \step[fieldset=url, null]
                    \step[fieldset=isbn, null]
                }
            }
        }
        \addbibresource{pres.bib}
    - \newcommand{\wid}{\mathbin{\triangledown}}
    - \newcommand{\nar}{\mathbin{\vartriangle}}
    - \newcommand{\Z}{\mathbb{Z}}
    - \newcommand{\widehatt}[1]{\widehat{\widehat{{#1}}}}
	- \usetikzlibrary{shapes,backgrounds,positioning,calc}
    - \usepackage{mathdots}
lang: english
date: 9th October 2017
...

## Refresh I

### Partially Ordered Set (poset)

*   $(P, \sqsubseteq)$, $P \neq \emptyset$
*   $\sqsubseteq$ is a binary relation which is reflexive, anti-symmetric, and
    transitive

. . .

### Lattice

*   let $(P, \sqsubseteq)$ be a poset
*   if $\inf(x, y)$ and $\sup(x, y)$ exists for all $x, y \in P$, then $(P,
    \sqsubseteq)$ is a *lattice*
*   if $\inf(X)$, $\sup(X)$ exists for all $X \subseteq P$, then $(P,
    \sqsubseteq)$ is a *complete lattice*

## Refresh II

### Galois Connection

*   let $(C, \le)$ and $(A, \sqsubseteq)$ be complete lattices
*   functions $\alpha \colon C \rightarrow A$, $\gamma \colon A \rightarrow C$ such that
    $\forall c \in C, \forall a \in A : \alpha(c) \le a \Leftrightarrow c \le \gamma(a)$

*   also $\gsac$

## Abstracting an Iterative Computation I

*   complete lattice $(C, \le)$
*   suppose we have a monotone function $f \colon C \rightarrow C$
*   we want to calculate (overapproximation of) smallest fixpoint of $f$

    $$\lfp(f) = \inf\{ c \in C \mid c = f(c) \} $$

    $$\lfp(f) \le \gamma(\lfp(f^\#)) = \inf\{ \gamma(a) \mid a \in A, a = f^\#(a) \} $$

    *   where $f^\# \colon A \rightarrow A$ approximates $f$

*   if $A$ is finite or has no infinite strictly ascending chains:

    $$\lfp(f^\#) = (f^\#)^n(\bot_A)$$

    *   for some $n \in \mathbb{N}$, $\bot_A = \inf(A)$
    *   can be calculated iteratively

## Abstracting an Iterative Computation II

important abstract domains have strictly ascending chains:

*   intervals: $[ 0, 0 ] \sqsubseteq [ 0, 1 ] \sqsubseteq [ 0, 2 ] \sqsubseteq
    \cdots$

. . .

but infinite abstract domains are useful

*   we can trade precision for tractability

    . . .

*   accelerate iterative computation

    . . .

*   possibly find a element $a \sqsupseteq \lfp(f^\#)$

## Widening I {.fragile}

*   let $(A, \sqsubseteq)$ be a poset
*   (pair) widening operator: $\wid \colon A \times A \rightarrow A$

    . . .

    *   *covering*: $\forall x, y \in A.\, x \sqsubseteq x \wid y$ and $y
        \sqsubseteq x \wid y$

        . . .

    *   *termination*: for every ascending chain $\{x_i\}_{i \ge 0}$ the ascending
        chain
        $$y_0 = x_0\qquad\qquad y_{i + 1} = y_i \wid x_{i+1}$$
        stabilizes after a finite number of terms

        . . .

    *   *note*: $\wid$ is often not symmetric

\smallskip
\onslide<2->
\center
\begin{tikzpicture}[node distance = 1cm]
	\tikzset{venn/.style={draw,minimum width=1.5cm,fill=#1,opacity=0.4}}

	\node [venn = paradisegreen, ellipse, minimum width=7cm, minimum height=4cm] (wxy) at (0, 0) {};
	\node [venn = green, ellipse, minimum width=4.5cm, minimum height=2.5cm, anchor=center] (sxy) at (0, 0) {};
	\node [venn = red, circle] (x) at (-1.5cm,0) {};
	\node [venn = blue, circle] (y) at (1.5cm, 0) {};

	\node[yshift=-1em] at (wxy.north) {$x \wid y$};
	\node[yshift=-1em] at (sxy.north) {$\sup\{x, y\}$};
	\node at (x) {$x$};
	\node at (y) {$y$};
\end{tikzpicture}

## Widening II

*   iterative calculation of $\widehat{x} \sqsupseteq \lfp(f^\#)$:
    \begin{align*}
        \widehat{x}_0 &= \bot \\
        \widehat{x}_{i + 1} &= \widehat{x}_i \wid f^\#(\widehat{x}_i)
    \end{align*}

*   example of $\wid$ (intervals):
    \begin{align*}
        \bot \wid x &= x \\
        x \wid \bot &= x \\
        [l_0, u_0] \wid [l_1, u_1] &= [\ite(l_1 < l_0, - \infty, l_0), \ite( u_0 < u_1, +\infty, u_0)]
    \end{align*}

    . . .

    *   if the bound is expanding, extrapolate to infinity

## Narrowing I

*   let $(A, \sqsubseteq)$ be a poset
*   (pair) narrowing operator: $\nar \colon A \times A \rightarrow A$

    . . .

    *   *bounding*: $\forall x, y \in A.\, x \sqsubseteq y \implies x
        \sqsubseteq (x \nar y) \sqsubseteq y$

    . . .

    *   *termination*: for every descending chain $\{ x_i \}_{i \ge 0}$
        the chain $$y_0 = x_0\qquad\qquad y_{i+1} = y_i \nar x_{i+1}$$
        stabilizes after a finite number of terms

        . . .

    *   *note*: $\nar$ is often not symmetric

\smallskip
\onslide<2->
\center
\begin{tikzpicture}[node distance = 1cm]
	\tikzset{venn/.style={draw,minimum width=1.5cm,fill=#1,opacity=0.4}}

	\node [venn = red, ellipse, minimum width=7cm, minimum height=4cm] (y) at (0, 0) {};
    \node [venn = green, ellipse, minimum width=4.5cm, minimum height=2.5cm] (nxy) at (0, 0) {};
	\node [venn = blue, ellipse, minimum height=1cm] (x) at (0, 0) {};

	\node[yshift=-1em] at (y.north) {$y$};
	\node[yshift=-1em] at (nxy.north) {$x \nar y$};
	\node at (x) {$x$};
\end{tikzpicture}

## Narrowing II

*   iterative improving of precision of $\widehat{x} \sqsupseteq \lfp(f^\#)$:
    \begin{align*}
        \widehat{\widehat{x}}_0 &= \widehat{x} \\
        \widehat{\widehat{x}}_{i + 1} &= \widehat{\widehat{x}}_i \nar f^\#(\widehat{\widehat{x}}_i)
    \end{align*}

*   obtains $\widehat{\widehat{x}}$ such that: $\lfp(f^\#) \sqsubseteq
    \widehat{\widehat{x}} \sqsubseteq \widehat{x}$

*   example of $\nar$ (intervals):
    \begin{align*}
        \bot \nar x &= \bot \\
        x \nar \bot &= \bot \\
        [l_0, u_0] \nar [l_1, u_1] &= [\ite(l_0 = - \infty, l_1, l_0), \ite( u_0 = +\infty, u_1, u_0)]
    \end{align*}

    . . .

    *   prefer finite bounds

## Widening and Narrowing in a Diagram

\center\vspace*{1.8em}
\begin{tikzpicture}[node distance = 1cm]
	\tikzset{venn/.style={draw,minimum width=1.5cm}}

    \useasboundingbox[minimum height=8cm, minimum width=12cm] (0, 0);
	\node<8-> [venn, onslide = {<8-8> fill=red!70!white}, onslide = {<9> fill=paradisegreen!70!white}, onslide = {<10-> fill=paradisegreen!30!white}, ellipse, minimum height=8cm, minimum width=12cm] (x) at (0, 0) {};
	\node<10-> [venn, onslide = {<10> fill=paradisegreen!70!white, onslide = {<11-> fill=paradisegreen!30!white}}, ellipse, minimum height=7.2cm, minimum width=10.8cm] (nx1) at (0, 0) {};
	\node<11-> [venn, onslide = {<11> fill=paradisegreen!70!white}, ellipse, minimum height=5.4cm, minimum width=8.1cm] (nx) at (0, 0) {};
	\node<4-> [venn, onslide = {<4-4> fill=blue!50!white}, onslide = {<9-> dashed}, ellipse, minimum height=5cm, minimum width=7.5cm] (lfpf) at (0, 0) {};
	\node<7-8> [venn, onslide = {<7-8> fill=red!70!white}, ellipse, minimum height=3.6cm, minimum width=5.4cm] (x2) at (0, 0) {};
	\node<3-8> [venn, onslide = {<3-4> fill=blue!50!white}, onslide = {<5-> dotted}, ellipse, minimum height=3cm, minimum width=4.5cm] (ffb) at (0, 0) {};
	\node<6-8> [venn, onslide = {<6-8> fill=red!70!white}, ellipse, minimum height=2.3cm, minimum width=3.5cm] (x1) at (0, 0) {};
	\node<2-8> [venn, onslide = {<2-4> fill=blue!50!white}, onslide = {<5-> dotted}, ellipse, minimum height=2cm, minimum width=3cm] (fb) at (0, 0) {};
	\node<1-8> [venn, onslide = {<1-4> fill=blue!50!white}, onslide = {<7-> dotted}, onslide = {<5-5> fill=red!70!white}, ellipse, minimum height=1cm] (b) at (0, 0) {};



	\node<-8> at (b) {$\bot\visible<5->{ = \widehat{x}_0}$};
	\node<2-4>[yshift=-0.8em] at (fb.north) {$f^\#(\bot)$};
	\node<3-4>[yshift=-0.8em] at (ffb.north) {$f^\#(f^\#(\bot))$};
	\node<4->[yshift=-0.8em] at (lfpf.north) {$\lfp(f^\#)$};
	\node<4-4>[yshift=-1.7em] at (lfpf.north) {$\vdots$};

    \node<6-8>[yshift=-0.8em] at (x1.north west) {$\widehat{x}_1$};
    \node<7-8>[yshift=-0.8em] at (x2.north west) {$\widehat{x}_2$};
    \node<8->[yshift=-0.8em] at (x.north west) {$\widehat{x}$};
    \node<8-8>[yshift=-1.7em, xshift=1.7em] at (x.north west) {$\ddots$};

    \node<9->[yshift=-0.8em] at (x.north east) {$\widehat{\widehat{x}}_0$};
    \node<10->[yshift=-0.8em] at (nx1.north east) {$\widehat{\widehat{x}}_1$};
    \node<11->[yshift=-0.8em] at (nx.north east) {$\widehat{\widehat{x}}$};
    \node<11->[yshift=0.5em, xshift=1.3em] at (nx.north east) {$\iddots$};
\end{tikzpicture}

## Widening and Narrowing Example

```{.python}
def foo():
    i = 1
    while i <= 100:
        i = i + 1
```

*   possible values of `i` at the beginning of the cycle?

    . . .

*   least fixed point of `f`:
    $$X = f(X) = (\{1\} \cup \{ i + 1 \mid i \in X \}) \cap \{ i \in \Z \mid i \le 100 \}$$

    . . .

*   with widening:
    \begin{align*}
        X &= f^\#(X) = ([1,1] \sqcup (X \oplus [1,1])) \sqcap [-\infty, 100] \\
        \widehat{X}_0 &= \bot \\
        \widehat{X}_1 &= \only<-7>{\only<-3>{\widehat{X}_0}\only<4->{\bot} \wid \only<-6>{(}\only<-5>{(}[1,1] \only<-5>{\sqcup \only<-4>{(\only<-3>{\widehat{X}_0}\only<4->{\bot} \oplus [1,1])}\only<5>{\bot}}\only<-5>{)}\only<-6>{ \sqcap [-\infty, 100]}\only<-6>{)}}\only<8->{[1,1]} \\
      \visible<9->{
        \widehat{X}_2 &= \only<-14>{\only<14>{\color{red}}\only<9>{\widehat{X}_1}\only<10->{[1,1]} \wid \only<-12>{(\only<-11>{([1,1] \sqcup \only<-10>{(\only<9>{\widehat{X}_1}\only<10->{[1,1]} \oplus [1,1])}\only<11->{[2,2]})}\only<12->{[1,2]} \sqcap [-\infty, 100])}\only<13->{[1,2]}}\only<15->{[1,+\infty]} \\
      }
      \visible<16->{
        \widehat{X}_3 &= \widehat{X}_2 = [1, +\infty]
      }
    \end{align*}

## Widening and Narrowing Example

```{.python}
def foo():
    i = 1
    while i <= 100:
        i = i + 1
```

*   improve precision with narrowing:
    \begin{align*}
        \widehatt{X}_0 &= \widehat{X} = [1, +\infty] \\
      \visible<2->{
        \widehatt{X}_1 &= \only<-4>{\only<-2>{\widehatt{X}_0}\only<3->{[1, +\infty]} \nar \only<-3>{(([1,1] \sqcup (\only<-2>{\widehatt{X}_0}\only<3->{[1,+\infty]} \oplus [1,1])) \sqcap [-\infty, 100])}\only<4->{[1,100]}}\only<5->{[1,100]} \\
      }
      \visible<6->{
        \widehatt{X}_2 &= \widehatt{X}_1 = [1,100]
      }
    \end{align*}
    \onslide<7->

    *   it is not always possible to reach the least fixed point by widening + narrowing

## XXX

can problem be always restated with finite abstraction?

. . .

*   no (proved in \cite{Cousot1992})

## Biblography {.allowpagebreaks}

\printbibliography[heading=none]
