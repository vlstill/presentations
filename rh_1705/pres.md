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
    - \input{../names.tex}
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
        (divm) {\redon{2}{DiVM}};
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
*   \redon{5}{provolávání a záznam systémových volání v \texttt{divine run}, replay (\katka)}
*   \redon{6}{testy, benchmarky (Vladimír Štill, \mornfall, …)}
*   \redon{7}{monitory, LTL (\ted, \heno)}
*   \redon{8}{počátek práce na procesech (\zuz)}

## Aktuální stav: publikace


*   DiVM: Model Checking with LLVM and Graph Memory 
    *   představení konceptu rozdělení verifikačního nástroje na virtuální stroj
        \+ operační systém, grafová paměť
    *   Petr Ročkai, Vladimír Štill, Ivana Černá, Jiří Barnat

. . .

*   Using Off-the-Shelf Exception Support Components in C++ Verification
    *   Vladimír Štill, Petr Ročkai, Jiří Barnat

. . .

*   Model Checking of C and C++ with DIVINE 4 
    *   tool paper, celkový přehled DIVINE 4
    *   primárně Vladimír Štill a Petr Ročkai

. . .

*   From Model Checking to Runtime Verification and Back
    *   provolávání systémových volání do hostitelského operačního systému
    *   Katarína Kejstová, Petr Ročkai, Jiří Barnat

(vše odesláno)

## Aktuální stav: ostatní

**návštěva konference ETAPS/TACAS**

*   prezentace SymDIVINE na SV-COMP (\honza)
    *   starší nástroj na verifikaci se symbolickými daty

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
    \node[right = of dios.east, align = left] {syscalls${}^*$};

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
        (divm.west) edge[opacity=0.5] (divm.east)
        (divm) edge[->, out=0, in=0, looseness=1.7] (linux)
      ;
    \node<3->[right = 3em of divm.south east, align = left, color=red] {\bf syscall\\passthrough};

\end{tikzpicture}
\end{latex}

\only<3->{\vspace{-1.25em}}

\begin{itemize}
    \item  (*): systémová volání DIVINE + \only<1-2>{simulovaná POSIX systémová
              volání}\only<3->{passthrough POSIX systémových volání}
    \only<3->{
        \item {\color{red} díky passthrough lze volat skutečná systémová volání
        kernelu na kterém DIVINE běží}
        \item tato systémová volání lze zaznamenávat
        \item záznam lze později použít ve verify, sim, …
    }
\end{itemize}

## Syscall Passthrough

*   DIVINE může být spuštěn ve třech základních módech
    *   `verify` -- prověřuje všechny možné běhy systému (všechny možnosti
        nedeterministických voleb, plánování vláken)
    *   `run` -- spouští jeden (náhodný) běh
    *   `sim` -- interaktivně prochází stavový prostor podle instrukcí
        uživatele nebo podle požadovaných voleb z verify

. . .

*   `verify` mód nemůže interagovat se systémem
    *   systémová volání jsou simulovaná
*   `run` mód může nyní buď simulovat systémová volání, nebo skutečně
    komunikovat s hostitelským systémem (passthrough)
    *   při komunikaci se systémem může zaznamenávat systémová volání

    . . .

*   zaznamenaná systémová volání lze „přehrát“ ve `verify`
    *   a umožnit další běhy, které interagují se systémem stejně
    *   v plánu: mezi systémovými voláními je částečné uspořádání, některá lze
        prohazovat

## Plány


