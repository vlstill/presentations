---
vim: spell spelllang=cs tw=80 fo+=t
title: "DIVINE: Průběžná zpráva"
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \usetikzlibrary{fit,backgrounds}
    - \usepackage{sansmathfonts}
    - \newcommand{\TODO}[1]{{\color{red}#1}}
    - \setbeamersize{text margin left=1.2em, text margin right=1.2em}
    - \newcommand{\redon}[2]{{\only<#1>{\color{red}}#2}}
    - \usepackage{changepage}
    - \input{../../names.tex}
lang: czech
date: 12\. května 2017
...

## Aktuální stav: implementace

\begin{latex}
\small
\begin{tikzpicture}[ auto, node distance=3cm
                   , semithick
                   , style={ node distance = 2em }
                   , state/.style={ rectangle, draw=black,
                     minimum height=1.7em, minimum width = 17em, inner
                     sep=2pt, text centered, node distance = 2em }
                   , shaded/.style = { fill = black!25!white }
                   ]

    \node[state, shaded] (usrp) {User's program + libraries};
    \node[state, shaded, below = 0em of usrp.south west, anchor = north west]
        (libs) {\redon{2}{C/C++ standard libraries, \texttt{pthreads}}};
    \node[state, shaded, below = 0em of libs.south west, anchor = north west]
        (dios) {\redon{2,4-5,8}{DiOS}};
    \node[state, below = 0em of dios.south west, anchor = north west]
        (divm) {\redon{2,5}{DiVM}};
    \node[state, below = 0em of divm.south west, anchor = north west]
        (alg) {\redon{2,7}{Verification core}};

    \draw[in=0, out=0, >=stealth', looseness=1.7]
        (usrp) edge[->] (dios)
        (libs) edge (dios)
      ;

    \draw[in=180, out=180, >=stealth', looseness=1.7]
        (libs) edge[->] (divm)
        (dios) edge (divm)
      ;
    \node[left = of divm.west] {hypercalls};
    \node[right = of dios.east, align = left] {syscalls};

    \draw[dashed, thin]
        (libs.north west) -- ++(-9em,0)
        (libs.north east) -- ++(7em,0)
      ;
    \node[left = 9em of libs.west, anchor = west] {\redon{2}{DIVINE}};
\end{tikzpicture}
\end{latex}

. . .

*   \redon{2}{DIVINE 4.0 final (9. 1.), aktuální je 4.0.7}

*   \redon{3}{průběžná práce na podpoře symbolické verifikace (\heno)}
*   \redon{4}{modularizace DiOS (\honza)}
*   \redon{5}{záznam a přehrávání systémových volání (\katka)}
*   \redon{6}{testy, benchmarky, evaluace (Vladimír Štill, \mornfall, …)}
*   \redon{7}{monitory, LTL (\ted, \heno)}
*   \redon{8}{práce na procesech (\zuz)}

## Aktuální stav: publikace


*   Petr Ročkai, Vladimír Štill, Ivana Černá, Jiří Barnat:\
    **DiVM: Model Checking with LLVM and Graph Memory**

. . .

*   Vladimír Štill, Petr Ročkai, Jiří Barnat:\
    **Using Off-the-Shelf Exception Support Components in C++ Verification**

. . .

*   **Model Checking of C and C++ with DIVINE 4**

. . .

*   Katarína Kejstová, Petr Ročkai, Jiří Barnat:\
    **From Model Checking to Runtime Verification and Back**

. . .

*   Petr Ročkai, Jiří Barnat: **A Simulator for LLVM Bitcode**

(vše odesláno)

## Aktuální stav: ostatní

**účast na konferenci ETAPS/TACAS**

*   prezentace SymDIVINE na SV-COMP (\honza)
    \bigskip

    . . .

*   novinky v SV-COMP
    *   bez preprocessingu vstupních souborů
    *   nově požadují svědky nekorektnosti ve všech kategoriích
        *   zatím chybí validátory pro concurrency a memory safety
    *   svědci korektnosti (ne všude)
    *   LTL demo kategorie
    *   DIVINE se pravděpodobně zúčastní

. . .

\bigskip

**DIVINE v IA169 System Verification and Assurance**

*   cvičení o DIVINE s domácím úkolem
*   feedback, především k simulátoru, odhalení drobných chyb
    *   připomínky především k (ne)snadnosti debugování paralelních programů

## Syscall Passthrough {.t}

\begin{latex}
\small
\begin{tikzpicture}[ auto, node distance=3cm
                   , semithick
                   , style={ node distance = 2em }
                   , state/.style={ rectangle, draw=black,
                     minimum height=1.7em, minimum width = 18em, inner
                     sep=2pt, text centered, node distance = 2em }
                   , shaded/.style = { fill = black!25!white }
                   ]

    \node[state, shaded] (usrp) {User's program + libraries};
    \node[state, shaded, below = 0em of usrp.south west, anchor = north west]
        (libs) {C/C++ standard libraries, \texttt{pthreads}};
    \node[state, shaded, below = 0em of libs.south west, anchor = north west]
        (dios) {DiOS};
    \node[state, below = 0em of dios.south west, anchor = north west]
        (divm) {DiVM};
    \node[state, below = 0em of divm.south west, anchor = north west]
        (alg) {Verification core};

%    \draw [decorate,decoration={brace,amplitude=1em,mirror,raise=2pt}]
%    (libs.north west) -- (alg.south west) node [black,midway,xshift=-5.5em] {\divine};

    \draw[in=0, out=0, >=stealth', looseness=1.7]
        (usrp) edge[->] (dios)
        (libs) edge (dios)
      ;

    \draw[in=180, out=180, >=stealth', looseness=1.7]
        (libs) edge[->] (divm)
        (dios) edge (divm)
      ;
    \node[left = of divm.west] {hypercalls};
    \node[right = of dios.east, align = left]
    {syscalls${}^*$\\\only<6->{\color{red}replay}};

    \draw[dashed, thin]
        (libs.north west) -- ++(-7em,0)
        (libs.north east) -- ++(7em,0)
      ;
    \node[left = 7em of libs.west, anchor = west] {\divine};

    \node<2->[state, below = 0em of alg.south west, anchor = north west] (slibs)
    {System libc and libc++};
    \node<2->[state, below = 0em of slibs.south west, anchor = north west] (linux)
    {Linux kernel};

    \draw<2->[dashed, thin]
        (alg.south west) -- ++(-7em,0)
        (alg.south east) -- ++(7em,0)
      ;
    \node<2->[left = 7em of slibs.west, anchor = west] {Host system};

    \draw<2->[in=0, out=0, >=stealth', looseness=1.7]
        (divm) edge[->] (linux)
        (alg) edge (linux)
        (slibs) edge (linux)
      ;
    \node<2->[right = of linux.east, align = left] {host syscalls};

    \draw<3->[color=red, line width = 0.3em, dashed]
        (usrp) edge[->, out=0, in=0, looseness=1.7] (dios)
        (libs) edge[out=0, in=0, looseness=1.7] (dios)
        (dios.east) edge[opacity=0.5] (dios.west)
        (dios) edge[->, out=180, in=180, looseness=1.7] (divm)
      ;
    \draw<3-5>[color=red, line width = 0.3em, dashed]
        (divm.west) edge[opacity=0.5] (divm.east)
        (divm) edge[->, out=0, in=0, looseness=1.7] (linux)
      ;
    \draw<6->[color=red, line width = 0.3em, dashed]
        (divm.west) edge[opacity=0.5] (divm.center)
      ;
    \node<3-5>[right = 3em of divm.south east, align = left, color=red] {\bf syscall\\passthrough};

\end{tikzpicture}
\end{latex}

\only<3-5>{\vspace{-1.25em}}

\begin{adjustwidth}{-1em}{0em}
\begin{itemize}
    \item  (*): systémová volání DIVINE + \only<1-2>{simulovaná POSIX systémová
              volání}\only<3->{\only<3-5>{provolávání}\only<6->{přehrávání} POSIX systémových volání}

    \item<3-> \redon{3-5}{passthrough umožňuje volat skutečná systémová volání
    kernelu, na kterém DIVINE běží}
    \only<4-5>{
    \begin{itemize}
        \item v run módu DIVINE (jeden běh)
        \item<5> interakci se systémem lze nahrát
    \end{itemize}
    }
    \only<6->{
    \begin{itemize}
        \item zaznamenanou interakci se systémem lze přehrát
        \item<7-> přehrávání funguje i ve \texttt{verify} a \texttt{sim}
        \item<8-> může verifikovat běhy, které mají stejné (podobné) interakce
    \end{itemize}
    }
\end{itemize}
\end{adjustwidth}

## Plány

*   verifikace se symbolickými daty, abstrakce

*   vylepšování simulátoru, propojení s DiOS

*   rozšiřování DiOS: procesy, konfigurovatelnost pro různé módy verifikace

*   release-ready verze syscall passthrough

*   verifikace LTL vlastností
