---
title: LLVM Transformations for Model Checking
subtitle: Master's Thesis
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
lang: czech
date: 15th February 2016
...

## Úvod: DIVINE

Cíl: verifikace paralelních C a C++ programů.

*   snaha přiblížit model checking programátorům
*   co možná nejlepší a nejvěrnější pokrytí programovacích jazyků
*   výpočetně náročné

. . .

*   verifikace pomocí LLVM
*   využití existujících kompilátorů
*   redukční strategie pro zmenšení paměťové a časové náročnosti

\begin{latex}
    \medskip
    \scriptsize
    \makebox[\textwidth][c]{
    \begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                       , semithick
                       , scale=0.7
                       , state/.style={ rectangle, draw=black, very thick,
                         minimum height=2em, minimum width = 4em, inner
                         sep=2pt, text centered, node distance = 2em }
                       ]
      \node[state, minimum width = 5em] (lib) {Knihovny};

      \node[state, below = of lib.south west, anchor = north west] (cpp) {C++};
      \node[state, right = 6em of cpp, rounded corners] (clang) {Clang};
      \node[state, right = 6em of clang] (llvm) {LLVM IR};
      \node[state, right = 6em of llvm, rounded corners] (divine) {\divine};

      \node[state, above = of divine.north east, anchor = south east, minimum width = 15em] (ltl) {ověřovaná vlastnost: safety, LTL};
      \node[state, below = of divine] (valid) {OK};
      \node[state, left = of valid, minimum width = 8em] (ce) {Protipříklad};

      \path (ltl.south) edge[out=270, in=90] (divine.north)
            (cpp) edge (clang)
            (clang) edge (llvm)
            (lib) edge [out=0, in=90, looseness = 1] (clang)
            (llvm) edge (divine)
            (divine.south) edge (valid) edge[out=270, in=90] (ce.north)
            ;
    \end{tikzpicture}
    }
\end{latex}

## Transformace LLVM

*   LLVM -- mezijakzyk používaný při překladu + knihovny pro manipulaci
*   snadná analýza, transformace

. . .

\begin{latex}
    \bigskip
    \makebox[\textwidth][c]{
    \scriptsize
    \begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                       , semithick
                       , state/.style={ rectangle, draw=black, very thick,
                         minimum height=2em, minimum width = 4em, inner
                         sep=2pt, text centered, node distance = 2em }
                       ]
      \node[state, minimum width = 5em] (lib) {Knihovny};

      \node[state, below = of lib.south west, anchor = north west] (cpp) {C++};
      \node[state, right = of cpp, rounded corners] (clang) {Clang};
      \node[state, right = of clang] (llvm) {LLVM IR};
      \node[state, onslide={<-2> color=white}, right = of llvm, rounded corners] (lart) {\lart};
      \node[state, onslide={<-2> color=white}, right = of lart] (llvm2) {LLVM IR};
      \node[state, right = of llvm2, rounded corners] (divine) {\divine};

      \node[state, above = of divine.north east, anchor = south east, minimum width = 15em] (ltl) {ověřovaná vlastnost: safety, LTL};
      \node[state, below = of divine] (valid) {OK};
      \node[state, left = of valid, minimum width = 8em] (ce) {Protipříklad};

      \path (ltl.south) edge[out=270, in=90] (divine.north)
            (cpp) edge (clang)
            (clang) edge (llvm)
            (lib) edge [out=0, in=90, looseness = 1] (clang)
            (divine.south) edge (valid) edge[out=270, in=90] (ce.north)
            ;
      \path<-2> (llvm) edge (divine);
      \path<3-> (llvm) edge (lart)
            (lart) edge (llvm2)
            (llvm2) edge (divine);
    \end{tikzpicture}
    }
\end{latex}

## LART

LART: nástroj pro analýzu a transformaci LLVM

*   bude distribuován spolu s DIVINE
*   využívá C++ API k manilulaci s LLVM IR

. . .

**V rámci této práce rozšířen:**

*   analýzy a podpůrné nástroje
*   podpora pro verifikaci LLVM memory modelu
*   několik optimalizačních technik

# Analýzy a podpůrné nástroje

## Analýzy a podpůrné nástroje

*   analýzy optimalizované na rychlost
*   transformace pro usnadnění interakce s výjimkami
    *   spouštění kódu na konci funkcí
    *   viditelnost výjimek ve funkci
*   převážně implementační detaily

# Relaxované paměťové modely

## Relaxované paměťové modely

# Optimalizace pro verifikaci

# OLD

## Example of Parallel Safe Optimization

**Constant Alloca Elimintation**

*   local variables in LLVM -- results of `alloca` instruction
    *   memory for a variable is allocated (on stack) so that it can be passed by
        referrence
    *   often the variable is neither modified nor accessed through pointer

*   if variable is constant and does not escape the functions it can be
    eliminated
    *   use of any load from `alloca` is replaced by value which was originally
        stored into it

*   compared to LLVM's register promotion this does not add registers

## Future

**Atomic Cycles**

*   if a cycle can be proven to terminate and not perform any visible action it
    can be executed inside atomic section
*   without the atomic section DIVINE emits state after each iteration
*   static or dynamic detection of visibility

*   need to employ termination detection

## Future

**Silent loads/stores**

*   DIVINE's notion of `load`/`store` visibility is based on pointer tracking
*   a value might be reachable by pointers from more then one thread, but might
    be accessed only by one
*   mark `load`/`store` as silent to avoid visibility checking
*   could also improve verification speed
*   requires good pointer analysis

**Slicing**

*   hard to preserve all properties (memory safety,…)
*   maybe as an approximation

## Future

**Symbolic Data**

*   DIVINE and SymDIVINE will be merged
*   symbolic data manipulation compiled in LLVM
*   state comparison using SMT in DIVINE

*   again, we will need good pointer analysis for parallel code


