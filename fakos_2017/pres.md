---
vim: spell spelllang=cs tw=80 fo+=t
title: "DIVINE: analýza programů v C++"
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \newcommand{\TODO}[1]{{\color{red}#1}}
    - \newcommand{\redon}[2]{{\only<#1>{\color{red}}#2}}
    - \setbeamersize{text margin left=1.2em, text margin right=1.2em}
lang: czech
date: 16\. září 2017
...

## DIVINE: analýza programů v C++

DIVINE je nástroj na analýzu programů v C a C++

*   historicky býval zaměřený převážně na verifikace paralelních/distribuovaných
    systémů

*   nyní především důraz na podporu C a C++ a na hledání těžko-nalezitelných
    problémů

. . .

*   memory safety, assertion safety, problémy související s paralelismem
    (deadlocky, data race, memory modely)

*   simulace chyb

. . .

*   rozsáhlá jazyková podpora, podpora části POSIX


## DIVINE: analýza programů v C++ {.t}

\begin{latex}
\medskip
\makebox[\textwidth][c]{
\footnotesize
\begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                   , semithick
                   , style={ node distance = 2em }
                   , state/.style={ rectangle, draw=black, very thick,
                     minimum height=1.7em, minimum width = 4.4em, inner
                     sep=2pt, text centered, node distance = 2em }
                   ]
  \node[state, minimum width = 6em] (code) {C++ code};
  \node[state, minimum width = 10.4em, right = 13.6em of code] (prop) {property and options};

  \node[state, below = 3em of code, rounded corners, onslide = {<2> fill = paradisegreen!80}] (clang) {compiler};
  \node[state, below = 1.5em of clang.south west, anchor = north west] (runtime) {runtime};
  \node[state, right = of clang] (llvm) {\llvm IR};
  \node[state, right = of llvm, rounded corners, minimum width = 8em, onslide = {<3> fill = paradisegreen!80}] (lart) {instrumentation};
  \node[state, right = of lart] (illvm) {\divm IR};
  \node[state, below = 1.5em of illvm.south west, anchor = north west, rounded corners, minimum width = 8em, onslide = {<4> fill = paradisegreen!80}] (verifier) {verification core};
  \node[above = 0.5em of lart] (pverify) {};

  \node[state, below = 2em of verifier.south east] (valid) {\color{green!40!black}Valid};
  \node[state, below = 2em of verifier.south west, minimum width = 8em, onslide = {<5> fill = paradisegreen!80}] (ce) {\color{red}Counterexample};

  \begin{pgfonlayer}{background}
      \node[state, fit = (pverify) (clang) (runtime) (llvm) (lart) (illvm) (verifier),
            inner sep = 0.8em, thick, rounded corners, dashed] (verify) {};
  \end{pgfonlayer}

  \node[below = 0.2em] at (verify.north) {DIVINE};

  \path (prop.348) edge[|*] (verifier.north)
        (prop.192) edge[|*] (lart.north)
        (code) edge (clang)
        (runtime) edge (clang)
        (clang) edge (llvm)
        (llvm) edge (lart)
        (lart) edge (illvm)
        (illvm) edge[|*] (verifier.north)
        (verifier) edge (valid) edge (ce)
        ;
\end{tikzpicture}
}

\bigskip
\only<1>{
    \begin{itemize}
        \item DIVINE je postavený nad LLVM: využívá kompilátoru clang a
        analyzuje LLVM bitkód
    \end{itemize}
}
\only<2>{
    \begin{itemize}
        \item kompilátor je clang integrovaný jako knihovna do DIVINE
        \item překládá nejen uživatelův program ale i knihovny, které program
        může volat
    \end{itemize}
}
\only<3>{
    \begin{itemize}
        \item část instrumentace je nutná pro chod DIVINE: sledování přístupu k
        paměti, detekce cyklů v control flow.
        \item část zajišťuje rozšiřuje funkcionalitu: symbolická verifikace,
              memory modely
    \end{itemize}
}
\only<4>{
    \begin{itemize}
        \item interpret LLVM s grafovou pamětí, který zajišťuje samotné
        spouštění programu
        \item algoritmus pro prohledávání stavového prostoru
    \end{itemize}
}
\only<5>{
    \begin{itemize}
        \item DIVINE dále obsahuje simulátor/debugger pro procházení běhů
        programu
    \end{itemize}
}
\end{latex}

## Jádro DIVINE: požadavky

*   DIVINE používá vlastní interpret LLVM
    *   DIVINE Virtual Machine = DiVM

. . .

*   interpret musí být velmi striktní
    *   kontrola přístupu k paměti, …
*   také musí být pokud možno rychlý
*   musí umožnit uložit snapshot stavu programu
*   musí umožnit spustit vše potřebné pro běh programu

## Jádro DIVINE: DiVM

*   LLVM IR je nejprve přeložen do interní reprezentace: DiVM IR
*   přidává k LLVM IR: alokace paměti, nízkoúrovňová manipulace zásobníku,
    hlášení chyb, …
*   DiVM pracuje nad pamětí reprezentovanou jako graf

![\ Paměť jako graf](linklist.pdf)

## Knihovny

*	programy nejsou uzavřené, používají externí hlavičnové soubory, externí
    knihovny

    . . .

*   používat systémové hlavičkové soubory je nebezpečné:

    . . .

    ```{.bash}
    $ ./symbiotic mem_heap_bug01.c
    cc: /usr/include/stdlib.h:33:10: fatal error:
        'stddef.h' file not found
    cc: 1 warning and 1 error generated.
    ```

    . . .

*   proto si s sebou DIVINE nese knihovny
    *   standardní knihovny C a C++
    *   POSIX threads

## Knihovny, kompilace, DiOS

*   DIVINE překládá C/C++ soubory pomocí integrovaného kompilátoru
    *   clang jako knihovna
*   knihovny pro použití s DIVINE jsou přeloženy při překladu DIVINE
*   kompilátor je přilinkuje k programu

. . .

*   dále je třeba poskytnout programu plánování vláken, procesů, případně
    simulaci práce se soubory

    . . .

*   to vše v DIVINE zajišťuje DiOS (DIVINE Operating System)

## Používání DIVINE: verifikace

*   `divine verify file.cpp [args…]`
*   `divine check file.cpp [args…]`
    *   méně striktní režim (neselhává alokace paměti, …)

. . .

*   ovlivnění kompilace:

    *   `divine verify -std=c++14 -Idir file.cpp`
    *   `-std=`, `-I`, `-l`, přímo, další přepínače přes -C
    *   `divine verify -C,-O3 file.cpp`

. . .

*   oddělená kompilace a verifikace

    *   `divine cc -almost-any-clang-flags file1.cpp file2.cpp`
    *   `divine verify file1.bc`

## Používání DIVINE: simulátor

*   `divine sim file.cpp`

    *   pro krokování programu od začátku

*   `divine sim --load-report file.report.????`

    *   načtení reportu z `verify`/`check`
    *   automaticky skočí před chybu
    *   na chybu se obvykle lze dostat přes příkaz `stepa`

## Používání DIVINE: základní příkazy simulátoru

*   `up`/`down` -- skákání po zásobníku
*   `show VAR` -- výpis hodnoty
    *   `show .localvar`
    *   `show $globals.globalvar`
*   `backtrace`
    *   `backtrace $state` pro všechna vlákna
*   `step [--over] [--out]` -- krokování po „příkazu“
*   `stepi [--over] [--out]` -- krokování po instrukci
*   `stepa [--over] [--out]` -- krokování po atomickém kroku
*   `thread pid:tid` -- výběr vlákna
*   `rewind #stav` -- návrat do předchozího stavu
*   `start` -- skok na začátek `main`
*   `break file:line`
*   `break function`
*   `draw VAR`


## Instrumentace -- memory modely

TODO

## Instrumentace -- abstraktní a symbolická verifikace

TODO
