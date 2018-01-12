---
vim: spell spelllang=en
title: Memory-Model-Aware Analysis of Parallel Programs
subtitle: PhD Thesis Proposal
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \usepackage{csquotes}

lang: english, czech
date: 16\. ledna 2018
...

## Verification of Parallel Programs I

- design of parallel programs is hard
- easy to make a mistake -- data races, deadlocks

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
                                  int c = x;
}                             }

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

- memory significantly slower that the processor cores
- processor has caches to speed up execution

  . . .

- optimizations of cache coherency protocols\
  $\rightarrow$ observable effects

  . . .

- reordering of instructions might be also observable

  . . .

- overall behaviour described by **(relaxed) memory model**

## Memory-Model-Aware Analysis

- encode the memory model into the program
- verify it using verifier without memory model support
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

## Implementation of Relaxed Memory Model

**program transformation**

- relatively straight-forward
- possibility of improvement with static analysis
- memory model independent

. . .

**memory operations**

- memory model dependent
- rather complex
- impacts efficiency a lot
  - amount of nondeterminism
  - extra memory consumption
  - speed of operations
- most of my work aims here
- I will primarily use bounded ordering of instructions

## Aims of the Work

- implement LLVM-based program transformation for relaxed memory models
  - done and quite stable, unlikely to need significant changes

. . .

- provide efficient support of memory models
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

## Publications

\framesubtitle{Techniques for Memory-Efficient Model Checking of C and C++ Code}

## Weak Memory Models as LLVM-to-LLVM Transformation

## Using Off-the-Shelf Exception Support Components in C++ Verification

## Model Checking of C and C++ with DIVINE 4

# Reader's Questions

## Otázky: Tomáš Vojnar I

\begin{quote}
V práci je několikrát uvedeno, že testování není pro paralelní programy
užitečné. V souvislosti s tím bych se rád zeptal, kam přesně klade doktorand
hranici pojmu testování a zda za neužitečné považuje i přístupy, jako je
systematické testování či dynamická analýza s využitím algoritmů jako např.
FastTrack
\end{quote}

- primárně mi jde o klasické postupy testování jako jednotkové testování nebo
  stress testing, případně s použítím nástrojů jako thread sanitizer nebo
  helgrind
- FastTrack umí detekovat data race, ale nevidím jak by bylo možné jen použít
  pro hledání chyb v lock-free programech v C/C++: pokud jsou všechny sdílené
  proměnné označeny jako atomické, pak podle C/C++ v programu nejsou data races,
  ale mohou se v něm snadno projevit chyby plynoucí z relaxované paměti

## Otázky: Tomáš Vojnar II

\begin{quote}Dále bych se doktoranda rád zeptal, jak chápe pojmy \uv{soundness} a
\uv{completeness} v případě analyzátorů programů? Považuje za \uv{sound}
přístup, který nemusí odhalit v programu chyby?
\end{quote}

- *soundness*: analyzátor může neskončit nebo odpovědět NEVÍM, pokud však
  odpoví ANO/NE odpověď musí být správná
  - tj. bounded model checker, který při detekci překročení omezení odpoví
    NEVÍM může být \uv{sound}, pokud opoví ANO tak ne
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
Jaké jsou praktické omezení akutálního nástroje z pohledu velikosti
a komplexnosti analyzovaného kódu? Na jaké databázi zdrojových kódů je
funknčnost nástroje testována?
\end{quote}

- u pralelních programů záleží především na množství vláken a míře jejich
  komunikace
  - lock-free je typicky náročnější než program korektně pracující se zámky
  - jednodušší lock-free datové struktury na 2-4 vláknech
  - složitější na 2 vláknech
- testy: cca 700 testů na veškerou funkcionalitu DIVINE
  - vlastní testy, testy C, C++ knihovny

## Otázky: Petr Švenda II

\begin{quote}
Jaké je strovnání vyvíjeného nástroje DIVINE (a speciálně jeho část
řešná studentem v rámci předložených tezí) s dalšími výzkumnými i komerčně
dostupnými nástroji?
\end{quote}

- co se týče výkonu porovnání zatím není

## Otázky: Petr Švenda III

\begin{quote}Dále bych požádal o bližší vysvětlení pozice nástroje DIVINE z
pohledu formální verifikace vs. hledání chyb. Pro jaké typy kontrolovaných
programů lze očekávat realizovatelnost kompletní formální verifikace a ve
kterých oblastech bude hlavním přínosem hledání chyb?
\end{quote}

- pro relaxované paměťové modely: verifikace modulo limit na množství
  přeuspořádaných operací
  - aktuálně není možné poznat, že došlo k překročení limitu
  - šlo by detekovat a případně postupně zvyšovat limit
  - pro libovolný limit lze zkonstruovat program pro který je nedostatečný
  - v hardware je přeuspořádání vždy omezené

- jinak pro uzavřené programy (bez vstupů) závislé jen na plánování verifikace
  - modulo chyby v generování spustitelného kódu
  - případně v překladači/knihovně při použití jiného překladače pro generování
    spustitelného kódu

- na verifikaci otevřených programů se pracuje

## Otázky: Petr Švenda IV

\begin{quote}
Je možné vyvíjený nástroj rozšířit o automatické vkládání synchronizačních
bariér tak, aby se nástrojem detekovaný problé ihned odstranil?
\end{quote}

- v principu by to mělo být možné, ale přijde mi smysluplnější soustředit se na
  efektivitu hledání chyb
