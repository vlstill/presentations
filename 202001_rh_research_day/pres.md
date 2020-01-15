---
vim: spell spelllang=en tw=80 fo+=t
title: "Heavy-Duty Program Analysis with DIVINE"
author:
    - Vladimír Štill
header-includes:
    - \input{defs}
lang: english
date: 23rd January 2020
abstract: |
    DIVINE is a tool for analysis of programs written in C and C++ that
    primarily targets hard to find bugs, including concurrency-related bugs,
    assertion violations, and memory errors. It is developed in the Parallel and
    Distributed Systems Laboratory at the Faculty of Informatics, Masaryk
    University. One of the main strengths of DIVINE is that it is precise and
    can discover bugs which are hard or impossible to find by other means.
    Unsurprisingly, this precision comes at some cost in the human and computing
    effort put to the analysis. In this presentation, we outline some of the
    strengths and weaknesses of DIVINE. We describe how we can use DIVINE for
    the analysis of programs which have source code and tests available, and
    what can we do if that is not the case. We also show how DIVINE can be used
    with programs that run in the POSIX environment, i.e., use system calls and
    interact with other programs using filesystem or network. Finally, we
    present how we can apply DIVINE to the discovery of sporadically occurring
    concurrency bugs.
...

## What is DIVINE? {.t}

\begin{tikzpicture}[>=latex,>=stealth',auto,node distance=1.5em,semithick,initial
                    text=, ->, shorten >=1pt, initial distance = 1cm]
  \tikzstyle{box}=[rectangle, draw, minimum size=0.75cm, align = center]
  \tikzstyle{group}=[draw, dashed, inner sep = 0.7em, rounded corners=0.4em]
  \draw[use as bounding box, draw=none] (-2.8em, 2.8em) rectangle (27.6em, -11em);

  \node[box] (c) {C source};
  \node[box, right = of c] (cpp) {C++ source};
  \node<2->[box, right = of cpp, text width=10em] (build) {C/C++ pogram with a build recipe (Makefile, CMake, …)};

  \node[box, below = 3em of cpp] (divine) {\textbf{DIVINE}};

  \node<3->[box, below left = 2em and 0em of divine, text width = {width("Error Found")}] (ok) {Correct};
  \node<3->[box, below right = 2em and 0em of divine] (nok) {Error Found};

  \tikzstyle{err}=[box, text width = {width("Nontermination/Deadlocks")}]
  \node<4->[err, right = of nok] (assert) {Assertion Violation};
  \node<5->[err, above = 0.2em of assert] (memerr) {Memory Error};
  \node<6->[err, above = 0.2em of memerr] (nonterm) {Nontermination/Deadlocks};

  \path
    (c) edge (divine)
    (cpp) edge (divine);
  \path<2-> (build) edge (divine);
  \path<3-> (divine) edge (ok);
  \path<3-> (divine) edge (nok);

  \path<4-> (nok) edge (assert);
  \draw<5-> ($ (nok.east) + (0.75em, 0) $)
      -- ($ (memerr.west) - (0.75em, 0) $)
      -- (memerr.west);
  \draw<6-> ($ (nok.east) + (0.75em, 0) $)
      -- ($ (nonterm.west) - (0.75em, 0) $)
      -- (nonterm.west);

  \node<1>[group, fit = (c) (cpp)] (input) {};
  \node<2->[group, fit = (c) (cpp) (build)] (input) {};

  \node[anchor=west] at (input.east) {Input};
\end{tikzpicture}

\newcommand{\INT}{\textcolor{red}{int}\xspace}
\newcommand{\VOID}{\textcolor{red}{void}\xspace}
\newcommand{\WHILE}{\textcolor{red}{while}\xspace}
\newcommand{\IF}{\textcolor{red}{if}\xspace}
\newcommand{\ELSE}{\textcolor{red}{else}\xspace}

\only<1>{DIVINE is a tool for analysis of C and C++ programs.}
\only<2>{It can process single files, or we can use a replacement compiler to compile
larger programs into a form which can be processed by DIVINE.

\medskip
\texttt{\$ make CC=divcc}}
\only<3>{Given enough resources, DIVINE will produce result. If an error is
found, DIVINE will produce a report.}
\only<4>{\tt
\INT foo( \INT x ) \{ assert( x > 0 ); /* ... */ \} \\

\INT main() \{ \\
\ \ \ \ \INT x = input(); \\
\ \ \ \ \IF ( x >= 0 ) \{ foo( x ); \} \ELSE \{ puts("Error"); \} \\
\}
}
\only<5>{\tt
\INT main() \{ \\
\ \ \ \ \INT x = input(); \\
\ \ \ \ \INT array[100]; \\
\ \ \ \ array[x] = 42; \\
\}
}
\only<6>{\tt
\VOID thread1() \{ \\
\ \ \ \ \WHILE ( x != 0 ) \{ /* wait */ \} \\
\ \ \ \ /* ... */ \\
\}

\VOID thread2() \{ x = 42; \}
}


