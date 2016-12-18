---
vim: spell spelllang=cs tw=80 fo+=t
title: DIVINE 4
subtitle: \vspace{0pt}
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \usetikzlibrary{fit,backgrounds}
lang: english
# aspectratio: 169
...

## Co je DIVINE?

*   nástroj na testování a verifikaci programů v C a C++
*   se zaměřením na paralelní programy a detailní detekci problémů s pamětí
*   využívá LLVM IR (jednodušší programovací jazyk používaný při překladu C/C++
    na nativní kód)

## Architektura DIVINE 4

\begin{latex}
    \bigskip
    \makebox[\textwidth][c]{
    \scriptsize
    \begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                       , semithick
                       , state/.style={ rectangle, draw=black, very thick,
                         minimum height=2em, minimum width = 4em, inner
                         sep=2pt, text centered, node distance = 1.8em }
                       ]


      \node[state, rounded corners, minimum width = 5em, minimum width = 12em, align = center] (divm) {
          \begin{minipage}{12em}
              \center \textbf{DiVM}

              \textbullet instruction execution\par
              \textbullet memory management\par
          \end{minipage}
      };
      \node[state, rounded corners, minimum width = 5em, minimum width = 12em, align = center, above = 4em of divm] (dios) {
          \begin{minipage}{12em}
              \center \textbf{DiOS}

              \textbullet threads\par
              \textbullet processes\par
              \textbullet virtual filesystem\par
              \textbullet POSIX support
          \end{minipage}
      };
      \node[state, rounded corners, right = of divm, minimum width = 11em] (alg) {Exploration Algorithm};
      \node[state, draw=none, left = of divm] (phantom) {};

      \path[<->] (alg) edge (divm);

      \node[state, rounded corners, above = of dios, minimum width = 6em] (libs) {
          \begin{minipage}{12em}\center
              \textbf{DIVINE libraries}

              \textbullet pthreads\\
              \textbullet C/C++ standard libraries
          \end{minipage}
      };
      \node[state, rounded corners, above = of libs, minimum width = 13em] (user) {User's program and libraries};
      \node[above = 0.6em of user] (phantomu) {};

      \begin{pgfonlayer}{background}
          \node[state, fit = (libs) (user) (dios) (phantomu), inner sep = 0.8em, thick] (bitcode) {};
      \end{pgfonlayer}

      \node[below right] at (bitcode.north west) {\textbf{Userspace (LLVM IR)}};

      \path[<->] (user) edge (libs)
                 (dios) edge node [right]{syscalls\ \ } (libs)
                 (dios) edge node [left] {VM interface} node [right] {(hypercalls, callbacks)} (divm)
                 (libs.east) edge [->, bend left, out = 90, in = 100, looseness = 1.5]
                      node {hypercalls} (divm.north east)
                 ;

      \begin{pgfonlayer}{background}
          \node[state, inner sep = 0.8em, thick, color = red
               , fit = (divm) (dios) (libs) (alg) (phantom)
               ] (divine) {};
      \end{pgfonlayer}
      \node[below right] at (divine.north west) {\textbf{DIVINE}};
    \end{tikzpicture}
    }
\end{latex}

## DiOS

Hlavní myšlenkou DIVINE 4 je oddělení velké části kódu do DiOS

*   odpovídá operačnímu systému
*   DiVM (interpret) nemusí řešit plánování vláken, jen nedeterministickou volbu
*   DiOS běží uvnitř DIVINE takže se snáze testuje než interpret
*   DiOS lze v případě potřeby vyměnit za jiný systém s jiným plánováním
    (například synchronní paralelismus)
    *   případně vyměnit jeho části (plánovač)
*   snadněji rozšiřitelný než DiVM

## DiOS: plánovač

Plánovač vláken (a procesů) je základní součástí DiOS

*   samotné spouštění uživatelského kódu
*   řeší prokládání vláken
*   a spouštění procesů (`fork`, `exec`, případné sdílení paměti mezi procesy)
*   při přerušení uživatelského kódu DiVM spustí plánovač
*   uživatelský kód s DiOS komunikuje pomocí systémových volání (syscallů)

## DiVM

DiVM (interpret LLVM) poskytuje rozhraní na velmi nízké úrovni (odpovídá
hypervizoru)

funkce (hyperally) pro:

*   správu paměti (alokace, dealokace, změna a zjištění velikosti)
*   nedeterministickou volbu
*   anotaci hran grafu (akceptující hrany, trace)
*   oznamování cyklů a práce s pamětí (bude vysvětleno)
*   ovládání control flow (bude vysvětleno)

dále definuje:

*   layout rámce a volací konvence
*   start programu
*   způsob předávání parametrů z příkazové řádky DIVINE do programu (DiOSu)

## DiVM: oznamování cyklů a práce s pamětí

DIVINE provádí redukci stavového prostoru skrýváním instrukcí, které
nepřistupují k paměti viditelné jinými vlákny

*   je třeba aby DIVINE vědět, kdy dochází k čtení/zápisu z/do paměti
*   může detekovat přímo DiVM -- ale někdy lze staticky poznat, že
    manipulovaná paměť je privátní
*   program musí oznámit DiVM kdy přistupuje k (potenciálně) viditelné paměti
*   oznámení zajistí instrumentace

. . .

obdobně pro cykly v control flow 

*   třeba detekovat kvůli terminaci hledání následníka

. . .

V návaznosti na tato oznámení DIVINE provádí interrupt aktuálního výpočtu a
předání řízení do plánovače.

## DiVM: ovládání control flow

DiOS (případně knihovny) musí být schopny:

*   vytvářet a spravovat zásobníky
*   nastavovat program counter
*   zakazovat interrupt (vytvářet atomické sekce)
*   nastavit, která funkce řeší plánování
*   nastavovat handler chyb
*   nastavovat globální proměnné a konstanty (kvůli procesům)

K tomu DiVM poskytuje sadu registrů a hypercall, který je umí modifikovat a číst.

## Protipříklady

Protipříklad je posloupností fixovaných hodnot nedeterministických voleb

*   k dispozici je interaktivní debugger
    *   umožňuje krokovat
    *   umožňuje inspekci programu v daném bodě výpočtu
    *   podobně jako GDB, ale umí spolehlivě chodit i zpět

