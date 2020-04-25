---
vim: spell spelllang=en tw=80 fo+=t
title: "Local Nontermination Detection\\newline for Parallel C++ Programs"
author:
    - \textbf{Vladimír Štill}
    - Jiří Barnat
header-includes:
    - \input{defs}
lang: english
date: 6th December 2019
...


## Motivation

\center

"Would you trust a program which was verified, but not tested?"

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

- we aim at nontermination caused by unintended parallel interactions \pause
- not at complex control flow & loops \pause
- should be easy to specify
- should not report nontermination spuriously
- should be useful for analysis of services/servers

. . .

\bigskip

- builds on explicit-state model checking \textrightarrow{} finite-state
  programs\
  (with possibly infinite behaviour)
- user can specify what to check

\bigskip

```{.cpp}
bool x = true;
while (true) { x = !x; }
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
atomic< bool > spin_lock = false;
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
    while (true) {
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

- does not happen in reality\pause

- for realistic schedulers an infinite run does not imply nontermination

## What is Nontermination?

**Nontermination**

- a program does not terminate if it can reach a point from which it cannot
  reach its end

\pause\bigskip

**Resource Section**

- a block of code with an identifier
- delimited in the source code

\pause\bigskip

**Local Nontermination**

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

## Going **Local**: Active Resource Section Instances

\begin{center}
\begin{tikzpicture}[node distance=0.4cm,>=stealth',bend angle=45,auto, baseline=(i.base)]

    \useasboundingbox (-6,0.5) rectangle (5,-7.3);
    \tikzstyle{state}=[circle, thick, draw, minimum size=5mm]
    \tikzstyle{endcheck}=[circle, thick, draw, cross out, minimum size=3mm]
    \tikzstyle{acc}=[line width=2pt]

    \node [state, initial, initial text=, initial above] (i) {};
    \node [state, below left = 0.7cm and 3cm of i] (cs1) {}
        edge [pre] node {\texttt{lock(m1)}} (i);

    \node<3-> [state, below right = 0.7cm and 2cm of i] (cs1a) {}
        edge [pre] node[above right] {\texttt{lock(m1)}} (i);

    \node<3-> [state, below = of cs1a] (cs1aw) {}
        edge [pre, acc] node[right] {\texttt{do\_work\_1}} (cs1a);
    \node<3-> [state, below = 0.6cm of cs1aw] (cs1acs2) {}
        edge [pre, acc] node[right] {\texttt{lock(m2)}} (cs1aw);
    \node<3-> [state, below = of cs1acs2] (cs1acs2w) {}
        edge [pre, acc] node[right, name=cs1acs2wl] {\texttt{do\_work\_2}} (cs1acs2);
    \node<3-> [state, below = 0.6cm of cs1acs2w] (cs1acs2u) {}
        edge [pre, acc] node[right, name=cs1aul] {\texttt{unlock(m2)}} (cs1acs2w);
    \node<3-> [endcheck, below = 0.6cm of cs1acs2u] (cs1au) {}
        edge [pre] node[right] {\texttt{unlock(m1)}} (cs1acs2u);

    \node [state, below = of cs1] (cs1w) {}
        edge [pre] node[right, name=cs1wl] {\texttt{do\_work\_1}} (cs1);

    \node [state, below left = 0.5cm and 1cm of cs1w] (cs1cs2) {}
        edge [pre] node[above left, name=cs1cs2l] {\texttt{lock(m2)}} (cs1w);
    \node [state, below = of cs1cs2] (cs1cs2w) {}
        edge [pre] node[right, name=cs1cs2wl] {\texttt{do\_work\_2}} (cs1cs2);
    \node [state, below = 0.6cm of cs1cs2w] (cs1cs2u) {}
        edge [pre] node[right] {\texttt{unlock(m2)}} (cs1cs2w);
    \node [state, below = 0.6cm of cs1cs2u] (cs1u) {}
        edge [pre] node[right] {\texttt{unlock(m1)}} (cs1cs2u);
    \node [state, below = of cs1u] (end) {}
        edge [pre] node[right] {\texttt{end}} (cs1u);

    \node<3-> [state, below right = 0.5cm and 1cm of cs1w] (cs1cs2a) {}
        edge [pre] node[above right] {\texttt{lock(m2)}} (cs1w);
    \node<3-> [state, below = of cs1cs2a] (cs1cs2aw) {}
        edge [pre, acc] node[right, name=cs1cs2awl] {\texttt{do\_work\_2}} (cs1cs2a);
    \node<3-> [endcheck, below = 0.6cm of cs1cs2aw] (cs1cs2au) {}
        edge [pre] node[right, name=cs1cs2aul] {\texttt{unlock(m2)}} (cs1cs2aw);


  \begin{pgfonlayer}{background}
    \tikzstyle{dashed}=[dash pattern=on 5pt off 5pt]
    \node<3->[draw, fit=(cs1a) (cs1acs2u) (cs1aul), inner sep=0.1cm, fill = yellow!70, xshift=-0.1cm, line width = 2pt] (cs1ains) {};
    \node<3->[below left = 1mm of cs1ains.north east] {ARSI};
    \node<3->[draw, dashed, fit=(cs1cs2a) (cs1cs2aw) (cs1cs2awl), inner sep=0.1cm, fill = yellow!70, line width = 2pt] (cs1cs2ains) {};
    \node<3->[below left = 1mm of cs1cs2ains.north east] {ARSI};

    \node<2>[draw, dashed, fit=(cs1cs2) (cs1cs2w) (cs1cs2wl), inner sep=0.1cm] (cs1cs2ins) {};
    \node<2>[draw, fit=(cs1) (cs1wl) (cs1cs2u) (cs1cs2l) (cs1wl), inner ysep=0.1cm, inner xsep=0cm] (cs1ins) {};

    \node<3->[draw, fit=(cs1) (cs1cs2l) (cs1cs2u) (cs1cs2aul), inner ysep=0.1cm, inner xsep=0cm] (cs1ins) {};
    \node<3->[draw, dashed, fit=(cs1cs2) (cs1cs2w) (cs1cs2wl), inner sep=0.1cm] (cs1cs2ins) {};
    \node<3->[draw, dashed, fit=(cs1acs2) (cs1acs2w) (cs1acs2wl), inner sep=0.1cm] (cs1acs2ins) {};
  \end{pgfonlayer}
\end{tikzpicture}
\end{center}

## Detecting **Local** Nontermination

- a resource section does not terminate if the program can reach a point in the
  section from which it cannot reach the corresponding resource section end

    . . .

- mark edges in ARSIs as **accepting**
- detect **fully accepting** terminal strongly connected components (**FATSCC**)

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

  \node[state] (c4a) [right of = c2a] {};
  \node[state] (c4b) [right of = c4a] {};
  \path
    (c2a) edge (c4a)
    (c4a) edge[bend left] (c4b)
    (c4b) edge[bend left] (c4a)
  ;
  \node[scc, fit=(c4a) (c4b)] (c4) {};
  \node[anchor=south] at (c4.south) {nontriv. terminal SCC};

  \node[state] (c5a) [below of = c4a] {};
  \node[state] (c5b) [right of = c5a] {};
  \path
    (c2a) edge[bend right] (c5a)
    (c5a) edge[bend left, line width=3pt, draw=blue] (c5b)
    (c5b) edge[bend left, line width=3pt, draw=blue] (c5a)
  ;
  \node[scc, fit=(c5a) (c5b), line width=3pt, draw=blue] (c5) {};
  \node[anchor=south] at (c5.south) {FATSCC};
\end{tikzpicture}

## Detection Algorithm

- modified Tarjan's algorithm for SCC decomposition: $\mathcal{O}(|G|)$

- global nontermination has no overhead

- for local nontermination the graph can get bigger

. . .

\begin{center}
\begin{tikzpicture}
\begin{loglogaxis}[xlabel={safety [s]}, ylabel={local nonterm. [s]},
                   width=0.53\textwidth, height=0.53\textwidth,
                   xmin = 0.8, ymin=0.8, xmax=14400, ymax=14400,
                   title = Wall Time (in seconds)]
\addplot [ scatter, only marks,mark size=2pt ]
         table [x=safety, y=local, meta=sl] {wall.dat};

\addplot [ black, sharp plot, thin, update limits=false ] coordinates { (0.1, 0.1) (10e6, 10e6) };
\addplot [ black, sharp plot, thin, update limits=false, dashed ] coordinates { (0.1, 1) (10e6, 100e6) };
\addplot [ black, sharp plot, thin, update limits=false, dotted ] coordinates { (0.1, 10) (10e6, 1000e6) };
\addplot [ black, sharp plot, thin, update limits=false, dashed ] coordinates { (1, 0.1) (100e6, 10e6) };
\addplot [ black, sharp plot, thin, update limits=false, dotted ] coordinates { (10, 0.1) (1000e6, 10e6) };
\end{loglogaxis}
\end{tikzpicture}
\end{center}

## Resource Sections & Summary & Limitations

**Source of resource sections**

- either built-in (mutexes, condition variables, thread joining, …)
- or user-provided (in source code; block of code, function end, …)

. . .

\bigskip

**Summary**

- allows detecting bugs not captured by safety (or LTL/CTL\*) analysis

- built on explicit-state model checking \textrightarrow{} finite state space
  required

- works also on programs which do not terminate

\bigskip
. . .

**Limitations**

- works only with deterministic threads
- does not work with relaxed memory

---

Published on SEFM 2019\ \cite{SB2019}.

## Extending to Nondeterminism

```{.cpp}
atomic< bool > flag = false;
void t1() {
    while ( true ) {
        x = read_input();
        if ( x < 0 )
            flag = true;
    }
}
void t2() {
    while ( !flag ) { } /* wait */
}
```

Does *wait* end? \pause depends on input

. . .

- for user, network or similar inputs we cannot expect that any outcome will
eventually happen

. . .

- want to report problems like this

## Extending to Nondeterminism

\begin{tikzpicture}[>=latex,>=stealth',auto,node distance=2cm,semithick,initial
                    text=, ->, shorten >=1pt, initial distance = 1cm]
  \tikzstyle{state}=[circle, draw, minimum size=0.75cm]
  \tikzstyle{scc}=[draw, dashed, inner sep = 1.3em, rounded corners=0.4em]
  \node[state,initial] (c1a) {};
  \node[state] (c1b) [right of = c1a] {};
  \node[state] (c1c) [below of = c1a] {};

  \path
	(c1a) edge[bend left] (c1b)
	(c1b) edge[bend left, draw = red, line width = 3pt] node{$input = \alpha$} (c1c)
    (c1c) edge[bend left] (c1a)
    (c1b) edge[bend left] (c1a)
  ;

  \node[scc, fit = (c1a) (c1b) (c1c)] (c1) {};

  \node[state] (c2a) [right = 3cm of c1b] {};
  \path (c1b) edge[draw = red, dashed, line width = 3pt] node{$input < \alpha$} (c2a);
  \node[scc, fit=(c2a)] (c2) {};

  \node (c2E) [right of = c2a] {};
  \path (c2a) edge node{\ \ end} (c2E) ;

  \node[state] (c3a) [below of =c2a] {};
  \path (c1b) edge[bend right, draw = red, dashed, line width = 3pt] node[above] {\ \ \ \ \ \ $input > \alpha$} (c3a);
  \node[scc, fit=(c3a)] (c3) {};

  \node (c3E) [right of = c3a] {};
  \path (c3a) edge node{\ \ end} (c3E) ;
\end{tikzpicture}

- suppose the choice of red edges depends on user input

- need to disregard outgoing edges bound to the same choice if there is at least one staying in the SCC

## Relaxed Memory

- up to this point verification was precise

. . .

\bigskip
with x86-TSO:

- the x86-TSO memory model is an *over-approximation* of behaviour of the real processor
  - it can delay write arbitrarily

  . . .

- the implementation is *under-approximation* of x86-TSO
  - as the buffer is bounded
  - we do not know if it is over-approximation or under-approximation of the real hardware

. . .

- over-approximation while searching for an end of a resource section is a problem

## Relaxed Memory

\begin{tikzpicture}[>=latex,>=stealth',auto,node distance=2cm,semithick,initial
                    text=, ->, shorten >=1pt, initial distance = 1cm]
  \tikzstyle{state}=[circle, draw, minimum size=0.75cm]
  \tikzstyle{scc}=[draw, dashed, inner sep = 1.3em, rounded corners=0.4em]
  \node[state,initial] (c1a) {};
  \node[state] (c1b) [right of = c1a] {};
  \node[state] (c1c) [below of = c1a] {};

  \path
	(c1a) edge[bend left] (c1b)
	(c1b) edge[bend left, draw = red, line width = 3pt] node[yshift=-0.5em, xshift=-0.8em] {$\max\{|\textit{buf}_i|\} = 2$} (c1c)
    (c1c) edge[bend left] (c1a)
    (c1b) edge[bend left] (c1a)
  ;

  \node[scc, fit = (c1a) (c1b) (c1c)] (c1) {};

  \node[state] (c2a) [right = 5cm of c1b] {};
  \path (c1b) edge[draw = red, dashed, line width = 3pt] node{$\max\{|\textit{buf}_i|\} = 16$} (c2a);
  \node[scc, fit=(c2a)] (c2) {};

  \node (c2E) [right of = c2a] {};
  \path (c2a) edge node{\ \ end} (c2E) ;

  \node[state] (c3a) [below of =c2a] {};
  \path (c1b) edge[bend right, draw = red, dashed, line width = 3pt] node[above, yshift=0.4em] {\ \ \ \ \ \ $\max\{|\textit{buf}_i|\} = 4$} (c3a);
  \node[scc, fit=(c3a)] (c3) {};

  \node (c3E) [right of = c3a] {};
  \path (c3a) edge node{\ \ end} (c3E) ;
\end{tikzpicture}

- maybe report sections as conditionally nonterminating if more relaxed behaviour is required to leave them then stay

. . .

- possibly report only those that can be reached with less relaxed behaviour then is required to reach the point of no return

. . .

- problem: buffer size is not a good measure of degree of relaxation

## Biblography {.allowpagebreaks}

\printbibliography[heading=none]
