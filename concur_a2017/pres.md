---
vim: spell spelllang=en tw=80 fo+=t
title: "Byte-Precise Verification of Low-Level List Manipulation"
author:
    - Kamil Dudka
    - Petr Peringer
    - Tomáš Vojnar
    - \newline\newline presented by Vladimír Štill
classoption: fleqn
header-includes:
    - \usepackage{divine}
    - \usepackage[mathlf,textlf]{MyriadPro}
    - \usepackage[small]{eulervm}

    - \newcommand{\TODO}[1]{{\color{red}#1}}
    - \newcommand{\redon}[2]{{\only<#1>{\color{red}}#2}}
    - \setbeamersize{text margin left=1.2em, text margin right=1.2em}
    - \usetikzlibrary{arrows.meta}
    - \usepackage{tikz-cd}
    - \usetikzlibrary{arrows}
    - \newcommand{\gc}[4]{\begin{tikzcd}#1 \arrow[shift left=2pt]{r}{#3} \pgfmatrixnextcell{}#2 \arrow[shift left=2pt]{l}{#4}\end{tikzcd}}
    - \newcommand{\gcac}{\gc{A}{C}{\alpha}{\gamma}}
    - \DeclareMathOperator{\lfp}{lfp}
    - \DeclareMathOperator{\ite}{ite}
    - |
        \newcommand\gtikzset[1]{%
        \begingroup%
         \globaldefs=1\relax%
          \pgfqkeys{/tikz}{#1}%
          \endgroup%
          }
    - \newcommand{\wid}{\mathbin{\triangledown}}
    - \newcommand{\nar}{\mathbin{\vartriangle}}
    - \newcommand{\Z}{\mathbb{Z}}
    - \newcommand{\widehatt}[1]{\widehat{\widehat{{#1}}}}
	- \usetikzlibrary{shapes,backgrounds,positioning,calc}
    - \usepackage{mathdots}
    - \institute{}
lang: english
...

## Introduction

*   tool Predator, you might know it from SV-COMP
*   specializes in programs with dynamically allocated data structures
    *   mostly linked list in the version presented here
*   can abstract heap shapes
*   symbolic execution with symbolically represented heap

. . .

*   the paper is quite technical, we will be just after high-level principles

## Symbolic Memory Graphs

*   we will focus on linked lists

\includegraphics[page = 7, clip, trim=8.7cm 21.2cm 4.5cm 5cm, width=\textwidth]{paper.pdf}

. . .

*   can be represented as a *Symbolic Memory Graph*

\includegraphics[page = 7, clip, trim=8.7cm 19.5cm 4.5cm 7cm, width=\textwidth]{paper.pdf}

*   values (marked by $\bullet$)
*   (memory) objects
    *   region (hollow rectange)
    *   doubly-linked list segment (DLS)

## Relations Between Objects and Values I

*   an object can *have a value* ($o \xrightarrow{\mathrm{off},\;\mathrm{type}} v$)

    *   the value is stored at offset $\mathrm{off}$
    *   and has type $\mathrm{type}$

    TODO: obrázek

    . . .

    *   there can be values of multiple types at the same offset (type
        reinterpretation)

    TODO: obrázek

## Relations Between Objects and Values II

*   a value (of type `ptr`) can *point to* an object ($v
    \xrightarrow{\mathrm{off},\;\mathrm{tgt}} o$)

    *   it can point inside $o$ at specific offset $\mathrm{off}$
    *   $\mathrm{tgt}$ is a target specifier
        *   `reg` for a region
        *   `fst` for first node of DLS
        *   `lst` for last node of DLS
        *   `all` for all nodes of DLS (later, with nested objects)


  TODO: obrázek

## Doubly-Linked List Segments

*   abstracts a number of linked list nodes
*   each segment has its length: minimum number of nodes it contains

    . . .

*   abstracted sequences of nodes need to be uninterrupted
    *   no pointers into the middle


## Nested Objects I

*   often each node of linked list points to equally-shaped objects

    \includegraphics[page = 9, clip, trim=11.3cm 7.5cm 4.5cm 17.5cm]{paper.pdf}

    . . .

*   these nodes can be abstracted into DLS with nested elements:

    \begin{tikzpicture}
        \node at (0, 0) {\includegraphics[page = 9, clip, trim=11.3cm 10.5cm
        4.5cm 14.8cm]{paper.pdf}};
        \node[fill=white] at (-0.36, 0.135) {\tiny\textrm{2+}};
    \end{tikzpicture}\vspace*{-1ex}

    *   $a_d$ and $r'$ has nesting *level* higher then $d$ ($\mathit{level}(d) = 0$)
    *   therefore $a_d$ and $r'$ are nested under $d$
    *   each list node corresponding to $d$ has its own copy of $r'$ and $a_d$

## Nesting Objects II

*   if $\mathit{level}(d) = \mathit{level}(r)$ then there is one object $r$
    pointed-to by all the nodes of DLS $d$ (**shared object**):

    \begin{latex}
    \gtikzset{obj/.style={rectangle, draw},
              val/.style={circle, text width = 0em, inner sep = 0.2em, fill=black},
              node distance = 1em}
    \begin{tikzpicture}
        \node[obj] (d) at (0, 0) {2+ DLS ($d$)};
        \node[val, below = of d] (v) {};
        \node[shift={(0.2,0)}] at (v.east) {$v$};
        \node[obj, below = of v] (r) {$r$};

        \draw[->] (d) -- (v);
        \draw[->] (v) -- (r);
    \end{tikzpicture}\qquad$\rightarrow$\qquad
    \begin{tikzpicture}
        \node[obj] (d1) at (0, 0) {$d_1$};
        \node[val, below right = of d1] (v) {};
        \node[obj, above right = of v] (d2) {$d_2$};
        \node[shift={(0.2,0)}] at (v.east) {$v$};
        \node[obj, below = of v] (r) {$r$};

        \draw[<->] (d1) -- (d2);
        \draw[->] (d1) -- (v);
        \draw[->] (d2) -- (v);
        \draw[->] (v) -- (r);
    \end{tikzpicture}
    \end{latex}


*   if $\mathit{level}(d) + 1 = \mathit{level}(r)$ then there is one object $r$
    for each node of DLS $d$ (**nested object**):

    \begin{latex}
    \begin{tikzpicture}
        \node[obj] (d) at (0, 0) {2+ DLS ($d$)};
        \node[val, below = of d] (v) {};
        \node[shift={(0.2,0)}] at (v.east) {$v$};
        \node[obj, below = of v] (r) {$r$};

        \draw[->] (d) -- (v);
        \draw[->] (v) -- (r);
    \end{tikzpicture}\qquad$\rightarrow$\qquad
    \begin{tikzpicture}
        \node[obj] (d1) at (0, 0) {$d_1$};
        \node[obj, right = 2em of d1] (d2) {$d_2$};
        \node[val, below = of d1]  (v1) {};
        \node[val, below = of d2]  (v2) {};
        \node[shift={(0.2,0)}] at (v1.east) {$v_1$};
        \node[shift={(0.2,0)}] at (v2.east) {$v_2$};
        \node[obj, below = of v1] (r1) {$r_1$};
        \node[obj, below = of v2] (r2) {$r_2$};

        \draw[<->] (d1) -- (d2);
        \draw[->] (d1) -- (v1);
        \draw[->] (d2) -- (v2);
        \draw[->] (v1) -- (r1);
        \draw[->] (v2) -- (r2);
    \end{tikzpicture}
    \end{latex}

*   back edges from nested nodes to DLS are marked with the `all` target
    specifier

## Semantics of SMGs

in two steps

1.  mapping from symbolic memory graph to a set of memory graphs
2.  mapping from memory graph to its memory images

. . .

### Memory Graph

*   like SMG, but without DLS and other non-region objects
*   obtained by materialization and removal of DLSs
*   $MG(G)$ … set of all memory graphs corresponding to a symbolic memory graph
    $G$

. . .

### Memory Image

*   gives concrete values to all values and pointers
*   details not needed herea

## Materialization and Removal of DLSs I

from any DLS it is possible to materialize a region before of after it

*   and decrease the number of nodes DLS contains
*   nested objects and values are duplicated
    *   duplicates are connected to the new node
    *   nesting level of duplicates is lowered

\includegraphics[page = 12, clip, trim=10cm 18.5cm 4cm 6.5cm]{paper.pdf}

*   *note*: it is possible to materialize even from 0+ DLS

## Materialization and Removal of DLSs II

*   a 0+ DLS can be removed
    *   letting the pointers bypass it

    \includegraphics[page = 12, clip, trim=10cm 16.17cm 4cm 9.5cm]{paper.pdf}

*   the semantics of SMG is given by all possible materializations and removals
    of DLSs

## Symbolic Program Configuration

*   $\textit{GVar}$ -- finite set of global variables
*   $\textit{SVar}$ -- countable set of stack variables
*   $\textit{SVar} \cap \textit{GVar} = \emptyset$
*   $\textit{Var} = \textit{SVar} \cup \textit{GVar}$

. . .

**Symbolic Program Configuration** (SPC): $C = (G, \upsilon)$, where

*   $G$ is an SMG
*   $\upsilon : \mathit{Var} \rightarrow R$ is a map from variables to
    regions in which their values are stored

## Data Reinterpretation

*   C/C++ allows data to be accessed through pointers of different types
    (casting, unions)

    . . .

*   in SMGs this is represented by multiple overlapping has-value edges

## Read Reinterpretation

*   reading a value of type $t$ at offset $of$ from an
    object $o$ of SMG $G$

    . . .

*   produces a new SMG $G'$ and a value $v$

    . . .

*   ads an edge $o \xrightarrow{of,\;t} v$ into $G'$ such
    that $MI(G) = MI(G')$

    . . .

    *   an unconstrained value if nothing is known about it
    *   or derived from existing values

## Write Reinterpretation

*   writing a value $v$ of type $t$ at offset $off$ of an object $o$ (of SMG $G$)

    . . .

*   the resulting SMG $G'$ must have an edge $e : o \xrightarrow{of,\;t} v$

    . . .

*   and $MI(G) \subseteq MI(G'')$ where $G''$ is obtained from $G'$ by removing
    $e$

    . . .

    *   e.g. obtain $G'$ from $G$ by removing (parts of) values colliding with
        $e$ and adding $e$

## Joins

given SMGs $G_1, G_2$, produce $G$ such that:

*   $MI(G_1) \subseteq MI(G) \supseteq MI(G_2)$
    *   $G$ generalizes both $G_1$ and $G_2$
*   $MI(G_1) = MI(G_2) \implies MI(G_1) = MI(G)$
*   $MI(G_1) \subset MI(G_2) \implies MI(G) = MI(G_2)$
*   $MI(G_1) \supset MI(G_2) \implies MI(G) = MI(G_1)$

. . .

*   implemented by walking the two SMGs from the corresponding starting objects
    and merging matching objects
    *   can merge DLSs with different length (resulting in minimum of the
        lengths)
    *   can introduce has-value edges with unknown values
    *   can join a region with DLS

. . .

*   for joining SMGs from different paths in the control flow
*   for abstraction
*   for equality testing of SMGs

## Abstraction

*   merging uninterrupted sequences of neighbouring objects into DLS
    *   together with possible sub-SMGs rooted at them

    . . .

*   by running join from two objects in the same SMG

    . . .

*   mergeable sequences ranked by length and loss of precision

\includegraphics[page = 18, clip, trim=8.7cm 19cm 4.2cm 6cm, width=\textwidth]{paper.pdf}

## Symbolic Execution with SMGs

*   reading uses read reinterpretation
*   writing uses write reinterpretation

    . . .

*   if DLS is accessed, or its address (except for addresses of first/last node)
    is to be taken, materialization occurs
    *   program statements execute on concrete objects

    . . .

*   DLS becomes 0+, computation is split
    *   either the DLS is removed
    *   or its size is incremented to 1+

    . . .

*   if statements can also split computation

    . . .

*   the basic algorithm only tracks value equality and if values are zeroed
