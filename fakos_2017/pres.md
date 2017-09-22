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

## DIVINE tým

\begin{tabularx}{\textwidth}{XXXXX}
\includegraphics{divine/jirik} &
\includegraphics{divine/mornfall} &
\includegraphics{divine/vlada} &
\includegraphics{divine/honza_m} &
\includegraphics{divine/heno}

\\

\includegraphics{divine/zuzka} &
\includegraphics{divine/katka} &
\includegraphics{divine/tadeas} &
\includegraphics{divine/honza_h} &
\includegraphics{divine/adam}
\end{tabularx}


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
  \node[state, below = 2em of verifier.south west, minimum width = 8em, onslide = {<5> fill = paradisegreen!80}] (ce) {\only<1-4>{\color{red}}Counterexample};

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
        \item DIVINE je postavený nad LLVM: využívá kompilátor clang a
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
        \item část rozšiřuje funkcionalitu: symbolická verifikace,
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
*   DiVM pracuje s pamětí reprezentovanou jako graf

![\ Paměť jako graf](linklist.pdf)

## Knihovny

*	programy nejsou uzavřené, používají externí hlavičnové soubory, externí
    knihovny

    . . .

*   používat systémové hlavičkové soubory je nebezpečné

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
*   knihovny jsou přeloženy při překladu DIVINE
*   kompilátor je přilinkuje k programu

. . .

*   dále je třeba poskytnout programu plánování vláken, procesů, případně
    simulaci práce se soubory, …

    . . .

*   to vše v DIVINE zajišťuje DiOS (DIVINE Operating System)

## Používání DIVINE: verifikace

*   `divine verify file.cpp [args…]`
*   `divine check file.cpp [args…]`
    *   méně striktní režim (neselhává alokace paměti, …)

. . .

*   ovlivnění kompilace:

    *   `divine verify -std=c++14 file.cpp`
    *   `-std=`, `-l`, přímo, další přepínače přes -C
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
    *   jméno reportu je na konci výpisu z verifikace
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

## Používání DIVINE: další příkazy simulátoru

*   `rewind #stav` -- návrat do předchozího stavu
*   `start` -- skok na začátek `main`
*   `break file:line`
*   `break function`
*   `draw VAR`

*   přidání okna se zdrojovým kódem
    *   `setup --xterm src`
    *   `setup --sticky "source --output-to src --clear-screen" --pygmentize`

\bigskip

**DEMO:**

*   `ssh login@arke`
*   `source /var/obj/divine-fakos`
*   `git clone https://github.com/vlstill/presentations.git`
*   `cd presentations/fakos_2017/demo`

# Instrumentace -- paměťové modely

## Paměťový model

*   při běhu paralelních programů vstupují do hry specifika chování daného
    hardware

    *   přeuspořádávání instrukcí a cache paměti způsobují, že paměťové efekty
        vlákna nemusí být vidět v očekávaném pořadí
    *   Intel `x86` umí „jen“ pozdržovat zápisy, ale zachovává jejich pořadí
    *   ARM umí přeuspořádat téměř libovolné nezávislé a některé závislé operace

*   kompilátor rovněž může přeuspořádávat operace

*   programátor musí správně využívat synchronizaci aby zabránil problémům s
    relaxovanou pamětí

    *   `std::atomic` v C++

## Paměťový model a C++ {.t}

\only<1-2>{\texttt{\textbf{int} x = 0, y = 0;}}
\only<3>{\texttt{\textbf{volatile int} x = 0, y = 0;}}
\only<4>{\texttt{\textbf{std::atomic< int >} x = 0, y = 0;}}

```{.cpp}
void thread1() { x = 1; y = 2; }
void thread2() {
    while ( y == 0 ) { }
    cout << "y = " << y << endl;
    cout << "x = " << x << endl;
}
```

Dosažitelné $x = 0 \land y = 2$? \only<2-3>{\color{red}ano}\only<4->{\color{paradisegreen}ne}


\only<2>{
\begin{itemize}
    \item nedefinované chování -- může dělat cokoli
    \item \texttt{thread2} může přečíst \texttt{y} jen jednou
\end{itemize}
}
\only<3>{
\begin{itemize}
\item opět nedefinované chování
\item na většině kompilátorů zajistí, že se \texttt{y} přečte v každé iteraci cyklu
\item to však na některých platformách nestačí k zajištění korektnosti
(ARM/POWER)
\end{itemize}
}
\only<4>{
\begin{itemize}
\item garance atomického přístupu k \texttt{x}, \texttt{y}
\item \texttt{y} načteno v každé iteraci cyklu
\item procesor nesmí přeuspořádat atomické zápisy
\end{itemize}
}

## Jak na paměťový model

*   verifikační nástroje často neberou ohled na paměťový model
*   v DIVINE používáme instrumentaci k zohlednění paměťového modelu

. . .

*   čtení a zápisy nahrazeny za funkce které simulují zpožďování operací
    *   zpožďování zápisů pomocí store bufferů
    *   předbíhání zápisů je komplikovanější

## Zpožďování zápisů

```{.cpp}
int x = 0, y = 0;
```

\begin{minipage}[t]{0.45\textwidth}

```{.cpp}
void thread0() {
    y = 1;
    cout << "x = " << x;
}
```

\end{minipage}\hfill\begin{minipage}[t]{0.45\textwidth}

```{.cpp}
void thread1() {
    x = 1;
    cout << "y = " << y;
}
```

\end{minipage}

\bigskip\pause

\begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                   , semithick
                   , scale=0.65
                   ]
  \draw [-] (-10,0) -- (-6,0) -- (-6,-2) -- (-10,-2) -- (-10,0);
  \draw [-] (-10,-1) -- (-6,-1);
  \draw [-] (-8,0) -- (-8,-2);
  \node () [anchor=west] at (-10,0.5) {main memory};
  \node () [anchor=west] at (-10,-0.5)  {\texttt{0x04}};
  \node () [anchor=west] at (-8,-0.5)  {\texttt{0x08}};
  \node () [anchor=west] at (-10,-1.5)  {\texttt{x = 0}};
  \node () [anchor=west] at (-8,-1.5)  {\texttt{y = 0}};

  \node () [anchor=west] at (-10,-3.5) {store buffer for thread 0};
  \node () [anchor=west] at (0,-3.5) {store buffer for thread 1};

  \draw [-] (-10,-4) -- (-4, -4) -- (-4,-5) -- (-10,-5) -- (-10,-4);
  \draw [-] (0,-4) -- (6, -4) -- (6,-5) -- (0,-5) -- (0,-4);
  \draw [-] (-8,-4) -- (-8,-5);
  \draw [-] (-6,-4) -- (-6,-5);
  \draw [-] (2,-4) -- (2,-5);
  \draw [-] (4,-4) -- (4,-5);

  \node<3-> () [anchor=west] at (-10,-4.5)  {\texttt{0x08}};
  \node<3-> () [anchor=west] at (-8,-4.5)  {\texttt{1}};
  \node<3-> () [anchor=west] at (-6,-4.5)  {\texttt{32}};

  \node<5-> () [anchor=west] at (0,-4.5)  {\texttt{0x04}};
  \node<5-> () [anchor=west] at (2,-4.5)  {\texttt{1}};
  \node<5-> () [anchor=west] at (4,-4.5)  {\texttt{32}};

  \node () [] at (-4, 0.5) {thread 0};
  \draw [->] (-4,0) -- (-4,-2);
  \node () [anchor=west, onslide={<3> font=\bf, color=red}] at (-3.5, -0.5) {\texttt{store y 1;}};
  \node () [anchor=west, onslide={<4> font=\bf, color=red}] at (-3.5, -1.5) {\texttt{load x;}};

  \node () [] at (2, 0.5) {thread 1};
  \draw [->] (2,0) -- (2,-2);
  \node () [anchor=west, onslide={<5> font=\bf, color=red}] at (2.5, -0.5) {\texttt{store x 1;}};
  \node () [anchor=west, onslide={<6> font=\bf, color=red}] at (2.5, -1.5) {\texttt{load y;}};

  \draw<3-> [->, dashed] (-0.5,-0.5) to[in=0, out=0] (-4,-4.5);
  \draw<4-> [->, dashed] (-9,-2) to[in=0, out=-90, out looseness=0.7] (-1.3,-1.5);
  \draw<5-> [->, dashed] (5.5,-0.5) to[in=0, out=0] (6,-4.5);
  \draw<6-> [->, dashed] (-7,-2) to[in=0, out=-90, out looseness=0.5] (4.7,-1.5);

\end{tikzpicture}
