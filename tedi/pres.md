---
vim: spell spelllang=en
title: Memory-Model-Aware Analysis of Parallel Programs
subtitle: PhD Thesis Proposal
author:
    - Vladimír Štill
header-includes:
    - \input{defs}

lang: czech, english
date: 16th January 2018
...

## Verification of Parallel Programs I

- design of parallel programs is hard
- easy to make mistakes -- data races, deadlocks

  . . .

- **memory behaviour is very complex**

  - effects of caches, out-of-order and speculative execution

  . . .

```{.cpp}
int x, y = 0;
void thread0() {
    y = 1;
    int a = x;
}

void thread1() {
    x = 1;
    int b = y;
}
```

- is it possible to end with `a == 0 && b == 0`? \pause{} **yes**

##  Verification of Parallel Programs II

- I work on the DIVINE model checker

- we focus on C and C++

- program is translated into LLVM intermediate language

- LLVM is executed by the model checker

- exploration of all possible runs of the program

## Relaxed Memory Example

```{.cpp}
int x, y = 0;
void thread0() {              void thread1() {
    y = 1;                        x = 1;
    int a = x;                    int b = y;
}                                 int c = x;
                              }

```

. . .

\begin{latex}
    \makebox[\textwidth][c]{
    \begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                       , semithick
                       , scale=0.65
                       ]

      \useasboundingbox (-10,1) (7.3,-6);

      \draw [-] (-10,0) rectangle (-7,-6);
      \draw [-] (-10,-1) -- (-7,-1)
                (-10,-2) -- (-7,-2)
                (-10,-3) -- (-7,-3)
                (-10,-4) -- (-7,-4)
                (-10,-5) -- (-7,-5);
      \draw [-] (-9,0) -- (-9,-6);
      \node () [anchor=west] at (-10,0.5) {memory};
      \node () [anchor=west] at (-10,-2.5)  {\texttt{x}};
      \node () [anchor=west] at (-9,-2.5) {\only<-8>{\texttt{0}}\only<9->{\texttt{1}}};

      \node () [anchor=west] at (-10,-3.5)  {\texttt{y}};
      \node () [anchor=west] at (-9,-3.5)  {\texttt{\only<-9>0\only<10->1}};

      \node () [anchor=center] at (-2,-5.5) {store buffer of t. 0};
      \node () [anchor=center] at (4,-5.5) {store buffer of t. 1};

      \draw [-] (-4,-4) rectangle (0,-5);
      \draw [-] (2,-4) rectangle (6,-5);
      \draw [-] (-2,-4) -- (-2,-5);
      \draw [-] (4,-4) -- (4,-5);

      \node<3-9> () [anchor=west] at (-4,-4.5)  {\texttt{y}};
      \node<3-9> () [anchor=west] at (-2,-4.5)  {\texttt{1}};

      \node<5-8> () [anchor=west] at (2,-4.5)  {\texttt{x}};
      \node<5-8> () [anchor=west] at (4,-4.5)  {\texttt{1}};

      \node () [] at (-4, 0.5) {thread 0};
      \draw [->] (-4,0) -- (-4,-2.3);
      \node () [anchor=west, onslide={<3> font=\bf, color=red}] at (-3.5, -0.5) {\texttt{y = 1;}};
      \node () [anchor=west, onslide={<4> font=\bf, color=red}] at (-3.5, -1.5) {\texttt{load x;}};

      \node () [] at (2, 0.5) {thread 1};
      \draw [->] (2,0) -- (2,-3.3);
      \node () [anchor=west, onslide={<5> font=\bf, color=red}] at (2.5, -0.5) {\texttt{x = 1;}};
      \node () [anchor=west, onslide={<6> font=\bf, color=red}] at (2.5, -1.5) {\texttt{load y;}};
      \node () [anchor=west, onslide={<7> font=\bf, color=red}] at (2.5, -2.5) {\texttt{load x;}};

      \draw<3-7> [->, dashed] (0.3,-0.5) to[in=0, out=0] (0,-4.5);
      \draw<4-7> [->, dashed] (-7,-2.5) to[in=0, out=0, out looseness = 3, in looseness=0.5] (-0.7,-1.5);
      \draw<5-7> [->, dashed] (6.3,-0.5) to[in=0, out=0] (6,-4.5);
      \draw<6-7> [->, dashed] (-7,-3.5) to[in=0, out=0, out looseness = 0.2, in looseness = 0.7] (5.3,-1.5);
      \draw<7-7> [->, dashed] (6,-4.5) to[in=0, out=0] (5.3,-2.5);

      \draw<-2> [->] (-4,-0.3) to (-3.4,-0.3);
      \draw<3> [->] (-4,-0.7) to (-3.4,-0.7);
      \draw<4-> [->] (-4,-1.7) to (-3.4,-1.7);

      \draw<-4> [->] (2,-0.3) to (2.6,-0.3);
      \draw<5> [->] (2,-0.7) to (2.6,-0.7);
      \draw<6> [->] (2,-1.7) to (2.6,-1.7);
      \draw<7-> [->] (2,-2.7) to (2.6,-2.7);
  \end{tikzpicture}
  }
\end{latex}

## Why Relaxed Memory?

- memory significantly slower than processor cores
- processor has caches to speed up execution

  . . .

- optimizations of cache coherency protocols\
  $\rightarrow$ observable effects

  . . .

- reordering of instructions might be also observable

  . . .

- overall behaviour described by **(relaxed) memory model**

## Memory-Model-Aware Analysis -- My Approach I

- encode the memory model into the program
- verify it using a verifier without memory model support
  - e.g. DIVINE, a lot of other verifiers
  - program transformation instead of modification of the verifier

. . .

- can be more robust
- same program transformation for multiple verifiers

. . .

```{.cpp}
x = 1;
int a = y;
```

$\rightsquigarrow$

```{.cpp}
_store( &x, 1 );
int a = _load( &y );
```

- `_load`, `_store` simulate the memory model

## Memory-Model-Aware Analysis -- My Approach II

**program transformation**

- relatively straightforward
- possibility of improvement with static analysis
- memory model independent

. . .

**memory operations**

- memory model dependent
- rather complex
- impact efficiency a lot $\rightarrow$ the main aim of my work
  - efficient data structures (time & memory)
  - amount of nondeterminism
- I will primarily use bounded reordering of instructions

## Aims of the Work

- implement an LLVM-based program transformation for relaxed memory models
  - done and quite stable, unlikely to need significant changes

. . .

- provide efficient support for memory models
  - for `x86-TSO` and Non-Speculative Writes
  - evaluate using DIVINE, other LLVM-based parallel verifiers

. . .

- explore heuristically-directed exploration to increase efficiency of analysis
  - tighter integration with the analysis tool

. . .

- analysis of very weak memory models
  - e.g. ARM and POWER which can feature write reordering

. . .

- techniques for unbounded analysis

## Publications I

\framesubtitle{Techniques for Memory-Efficient Model Checking of C and C++ Code}

\fcite{RSB15TC}

- tree-based compression scheme, custom allocation scheme
- I have made part of the implementation (compression), full evaluation, and part of the text
- I have presented this at SEFM 2015 (rank B)

## Publications II

\framesubtitle{Weak Memory Models as LLVM-to-LLVM Transformation}

\fcite{SRB15weakmem}

- analysis of programs under the TSO memory model using LLVM transformation
- I am the main author of this paper
- I have also presented this at MEMICS 2015

## Publications III

\framesubtitle{Using Off-the-Shelf Exception Support Components in C++ Verification}

\fcite{SRB2017except}

- verification of C++ code with exceptions in DIVINE 4
- with maximal reuse of existing libraries
- I am the main author of this paper
- I have presented this at QRS 2017 (rank B)
- awarded best paper award

## Publications IV

\framesubtitle{Model Checking of C and C++ with DIVINE 4}

\fcite{DIVINEToolPaper2017}

- tool paper describing architecture of DIVINE 4 and new features of this version
- I have written most of the text for this paper
- ATVA 2017 (rank A)

# Reader's Questions

## Otázky: Tomáš Vojnar I

\newcounter{finalframe}
\setcounter{finalframe}{\value{framenumber}}

\begin{quote}
V práci je několikrát uvedeno, že testování není pro paralelní programy
užitečné. V souvislosti s tím bych se rád zeptal, kam přesně klade doktorand
hranici pojmu testování a zda za neužitečné považuje i přístupy, jako je
systematické testování či dynamická analýza s využitím algoritmů jako např.
FastTrack
\end{quote}

- primárně mi jde o klasické postupy testování jako jednotkové testování nebo
  stress testing, případně s použitím nástrojů jako thread sanitizer nebo
  helgrind
- FastTrack umí detekovat data race, ale nevidím, jak by bylo možné jej adaptovat
  pro hledání chyb v lock-free programech v C/C++: pokud jsou všechny sdílené
  proměnné označeny jako atomické, pak podle C/C++ v programu nejsou data races,
  ale mohou se v něm snadno projevit chyby plynoucí z relaxované paměti

## Otázky: Tomáš Vojnar II

\begin{quote}Dále bych se doktoranda rád zeptal, jak chápe pojmy \uv{soundness} a
\uv{completeness} v případě analyzátorů programů? Považuje za \uv{sound}
přístup, který nemusí odhalit v programu chyby?
\end{quote}

- *soundness*: analyzátor může neskončit nebo odpovědět NEVÍM, pokud však
  odpoví ANO/NE, odpověď musí být správná
  - např. bounded model checker, který při detekci překročení omezení odpoví
    NEVÍM může být \uv{sound}, pokud opoví ANO tak není \uv{sound}
- *completeness*: analyzátor musí skončit pro programy, které mají konečný stavový
  prostor pod modelem sekvenční konzistence a když skončí, tak dát výsledek
  ANO/NE

## Otázky: Tomáš Vojnar III

\begin{quote}
Konečně bych rád přesnější objasnění, proč je zapotřebí zvláštní
podpora paměťových operací v modelu NSW (viz cíle v sekci 4.1.2) a není ji možno
pokrýt přístupy definovanými v sekci 4.1.1.
\end{quote}

- část 4.1.1 se zabývá primárně transformací/instrumentací LLVM, ale neřeší
  konkrétní implementace paměťového modelu
  - tato část musí být dostatečně generická pro zapojení různých paměťových
    modelů
- v části 4.1.2 jde primárně o efektivní implementaci konkrétního paměťového
  modelu nad základy vybudovanými v 4.1.1

## Otázky: Petr Švenda I

\begin{quote}
Jaká jsou praktická omezení akutálního nástroje z pohledu velikosti
a komplexnosti analyzovaného kódu? Na jaké databázi zdrojových kódů je
funknčnost nástroje testována?
\end{quote}

- u paralelních programů záleží především na množství vláken a míře jejich
  komunikace
  - lock-free je typicky náročnější než program korektně pracující se zámky
  - jednodušší lock-free datové struktury na 2-4 vláknech
  - složitější na 2 vláknech
- testy: cca 700 testů na veškerou funkcionalitu DIVINE
  - vlastní testy, testy C, C++ knihovny

## Otázky: Petr Švenda II

\begin{quote}
Jaké je strovnání vyvíjeného nástroje DIVINE (a speciálně jeho části
řešené studentem v rámci předložených tezí) s dalšími výzkumnými i komerčně
dostupnými nástroji?
\end{quote}

- co se týče výkonu, porovnání v oblasti relaxované paměti zatím není
- komerční nástroje často založeny na statické analýze
  - riziko false alarms -- v DIVINE by být neměly
- výzkumné nástroje
  - DIVINE má typicky lepší podporu jazyka (především u C++)
  - typicky jiný princip (bounded MC, stateless MC, symbolická exekuce)
    $\rightarrow$ jiné kompromisy

## Otázky: Petr Švenda III

\begin{quote}
Dále bych požádal o bližší vysvětlení pozice nástroje DIVINE z
pohledu formální verifikace vs. hledání chyb. Pro jaké typy kontrolovaných
programů lze očekávat realizovatelnost kompletní formální verifikace a ve
kterých oblastech bude hlavním přínosem hledání chyb?
\end{quote}

- snaha o verifikaci, jsou tu ale jistá omezení
  - **relaxovaná paměť**: limit na množství přeuspořádaných instrukcí
  - otevřené programy (verifikace pro všechny vstupy): nyní jen omezená podpora
  - vždy: riziko chyb v překladu LLVM na binární kód, v DIVINE
    - také v překladači/knihovně při překladu výsledné binárky jiným překladačem
- omezením je velikost analyzovaného programu
  - pro velké programy plánovaný bugfinding mód v DIVINE

## Otázky: Petr Švenda IV

\begin{quote}
Je možné vyvíjený nástroj rozšířit o automatické vkládání synchronizačních
bariér tak, aby se nástrojem detekovaný problém ihned odstranil?
\end{quote}

- není vždy zřejmé, co má být správné chování, či jestli je chyba způsobená
  relaxovaným paměťovým modelem
- v principu by to mělo být možné detekovat \uv{relaxované chování}, ale přijde
  mi smysluplnější soustředit se spíše na efektivitu hledání chyb

\setcounter{framenumber}{\value{finalframe}}
\addtocounter{framenumber}{-2}
