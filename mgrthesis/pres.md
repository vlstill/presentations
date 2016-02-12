---
title: LLVM Transformations for Model Checking
subtitle: Master's Thesis
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \input{tlua}

lang: czech
date: 15th February 2016
aspectratio: 169
...

## Úvod: DIVINE

Cíl: verifikace paralelních C a C++ programů.

*   snaha přiblížit model checking programátorům
*   co možná nejširší a nejvěrnější podpora programovacích jazyků

. . .

*   výpočetně náročné
*   verifikace pomocí LLVM, využití existujících kompilátorů
*   redukční strategie pro zmenšení paměťové a časové náročnosti

\begin{latex}
    \medskip
    \footnotesize
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
      \node[state, below = of divine] (valid) {\color{paradisegreen!70!black}OK};
      \node[state, left = of valid, minimum width = 8em] (ce) {\color{red!70!black}Protipříklad};

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

*   LLVM -- mezijazyk používaný při překladu + knihovny pro jeho manipulaci
*   snadná analýza, transformace

. . .

\begin{latex}
    \bigskip
    \makebox[\textwidth][c]{
    \footnotesize
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
      \node[state, below = of divine] (valid) {\color{paradisegreen!70!black}OK};
      \node[state, left = of valid, minimum width = 8em] (ce) {\color{red!70!black}Protipříklad};

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
*   využívá C++ API k manipulaci s LLVM IR

. . .

**V rámci této práce rozšířen:**

*   analýzy a podpůrné nástroje
*   **podpora pro verifikaci LLVM memory modelu**
*   několik optimalizačních technik

# Analýzy a podpůrné nástroje

## Analýzy a podpůrné nástroje

*   analýzy optimalizované na rychlost

*   transformace pro usnadnění interakce s výjimkami
    *   spouštění kódu na konci funkcí
    *   potřebné pro mnohé další transformace
    *   výjimka může způsobit ukončení funkce v místě volání jiné funkce
    *   $\rightarrow$ zajištění *vyditelnosti výjimek* ve funkci

# Relaxované paměťové modely

## Relaxované paměťové modely

zápis do paměti není na moderních CPU okamžitě viditelný ostatním procesorům

*   store buffery, cache, optimalizace cache coherence protokolu
*   okamžitá viditelnost odpovídá *sekvenční konzistenci* (*Sequential Consistency*, SC)
*   reálná CPU se chovají neintuitivně

\begin{latex}
\makebox[\textwidth][c]{
    \textit{[obrázek]}
}
\end{latex}

## Verifikace paměťových modelů

*   běžně verifikační nástroje podporují jen sekvenční konzistenci
*   stejně tak DIVINE

. . .

**Dva možné přístupy k rozšíření funkcionality**

\begSplit

modifikace interpretru

*   specifické pro DIVINE
*   vyžaduje zásah do složitého interpretru LLVM
*   velmi náročné na ladění při vývoji
*   nevyžaduje nic navíc od uživatele, jen zapnout při verifikaci modelu

\Split[\pause]

**modifikace vstupního programu**

*   na úrovni C++, nebo **LLVM**
*   na úrovni LLVM výrazně jednodušší
*   lze modifikovat pro jiné nástroje než DIVINE
*   nevyžaduje modifikaci programu uživatelem, jen zapnout při překladu modelu

\endSplit

## LLVM transformace pro paměťové modely

*   přidává podporu paměťového modelu LLVM
*   téměř úplné pokrytí paměťového modelu C++11
*   paměťový model obsahuje atomické instrukce (compare-and-swap, read-modify-write)
*   simulace pomocí store bufferů rozšířených o informace o instrukci

. . .

*   transformace je parametrizovatelná
    *   jaké garance paměťový model dává (reálné paměťové modely dávají větší garance než LLVM)
    *   jak velký má být store buffer

## Princip transformace

*   každý zápis, čtení, atomická instrukce a paměťová bariéra transformovány
    *   zápis proběhne do store bufferu
    *   čtení kontroluje jestli je v lokálním store bufferu nová hodnota
*   přidáno vlákno, které vylévá store buffer

. . .

*   atomické instrukce se simulují pomocí synchronizace + atomické masky
*   ostatní nahrazeny funkcemi (implementované v C++)

. . .

*   nutno zabránit opožděnému zápisu do uvolněné paměti
    *   vylití store bufferu po uvolnění paměti/skončení funkce
    *   dané lokace odstraněny ze store bufferu
    *   vyžaduje viditelné výjimky

## Optimalizace velikosti stavového prostoru

Transformace zhoršuje stavovou explozi $\rightarrow$ několik optimalizací

*   využívá toho, že DIVINE dokáže poznat, jestli ukazatel ukazuje do thread-local paměti
*   ne vždy nutné použít store buffer
    *   čtení z thread-local paměti
    *   zápis do paměti, která je prokazatelné thread-private po celou dobu
        běhu programu (staticky určeno)

## Vyhodnocení: total store order (velikost stavového prostoru)

\begin{tabularx}{\textwidth}{|l|C|CCC|CCC|} \hline
Jméno  & SC & \multicolumn{3}{c|}{TSO} & \multicolumn{3}{c|}{TSO: nárůst} \\
  & - & 1 & 2 & 3 & 1 & 2 & 3 \\ \hline
\texttt{simple}   &    \si{127} & \si{3445}\dg & \si{5973}\dg & \si{15663}\dg & \speedup{3445}{127} & \speedup{5973}{127} & \speedup{15663}{127} \\
\texttt{peterson} &    \si{703} & \si{21837}\dg & \si{53443}\dg & \si{55665}\dg & \speedup{21837}{703} & \speedup{53443}{703} & \speedup{55665}{703} \\
\texttt{fifo}     &    \si{791} & \si{14892} & \si{35865} & \si{48787} & \speedup{14892}{791} & \speedup{35865}{791} & \speedup{48787}{791} \\
\texttt{fifo-at}  &    \si{717} & \si{39539} & \si{166621} & \si{497229} & \speedup{39539}{717} & \speedup{166621}{717} & \speedup{497229}{717} \\
\texttt{fifo-bug} &\si{1611}\dg & \si{11291}\dg & \si{44192}\dg & \si{68655}\dg & \speedup{11291}{1611} & \speedup{44192}{1611} & \speedup{68655}{1611} \\
\texttt{hs-2-1-0} & \si{890973} & \si{250390514} & -- & -- & \speedup{250390514}{890973} & -- & -- \\ \hline % 251\,M & & \\ \hline
\end{tabularx}

## Vyhodnocení: LLVM memory model (velikost stavového prostoru)

\begin{tabularx}{\textwidth}{|l|C|CCC|CCC|} \hline
Jméno  & SC & \multicolumn{3}{c|}{LLVM} & \multicolumn{3}{c|}{LLVM: nárůst} \\
  & - & 1 & 2 & 3 & 1 & 2 & 3 \\ \hline
\texttt{simple}   &    \si{127} & \si{3522}\dg & \si{8072}\dg & \si{23609}\dg & \speedup{3522}{127} & \speedup{8072}{127} & \speedup{23609}{127} \\
\texttt{peterson} &    \si{703} & \si{21981}\dg & \si{56336}\dg & \si{69787}\dg & \speedup{21981}{703} & \speedup{56336}{703} & \speedup{69787}{703} \\
\texttt{fifo}     &    \si{791} & \si{18297} & \si{15648}\dg & \si{22985}\dg & \speedup{18297}{791} & \speedup{15648}{791} & \speedup{22985}{791} \\
\texttt{fifo-at}  &    \si{717} & \si{53498} & \si{255636} & \si{1067735} & \speedup{53498}{717} & \speedup{255636}{717} & \speedup{1067735}{717} \\
\texttt{fifo-bug} &\si{1611}\dg & \si{12131}\dg & \si{14142}\dg & \si{21098}\dg & \speedup{12131}{1611} & \speedup{14142}{1611} & \speedup{21098}{1611} \\
\texttt{hs-2-1-0} & \si{890973} & \si{251249798} & -- & -- & \speedup{251249798}{890973} & -- & -- \\ \hline
\end{tabularx}

## Vyhodnocení: další experimenty (po odevzdání)

\begin{tabularx}{\textwidth}{|>{\tt\,}l<{\,}|C|CCCC|CC|} \hline
    \multicolumn{1}{|c|}{\sf Jméno} & SC & \multicolumn{4}{c|}{TSO} &
    \multicolumn{2}{c|}{\llvm} \\
    &  & 1 & 2 & 4 & 8 & 2 & 4 \\ \hline

    hs-2c & 2.08\,k & 60.3\,k & 76.2\,k & 102\,k & 134\,k & 9.39\,M & --\\
    hs-2c+resize & 26.7\,k & 901\,k & 1.15\,M & 1.56\,M & 1.96\,M & 110\,M & --\\
    hs-2d & 2.35\,k & 70.1\,k & 88.2\,k & 118\,k & 156\,k & 10.5\,M & --\\
    hs-2d+2c & 7.86\,k & 351\,k & 469\,k & 663\,k & 948\,k & 69.5\,M & --\\
    fifo & 10\,k & 204\,k & 326\,k & 480\,k & 593\,k & 491\,k\dg &
    538\,k\dg\\
    fifo-at & 9.33\,k & 216\,k & 472\,k & 1.26\,M & 1.65\,M & 10.1\,M & 240\,M\\
    blockring & 2.37\,k & 20.1\,k & 27.6\,k & 36.3\,k & 40\,k &
    10.4\,k\dg & 11\,k\dg\\
    blocklink & 1.37\,k\dg & 5.38\,k\dg & 4.87\,k\dg
    & 3.45\,k\dg & 3.49\,k\dg & 4.48\,k\dg &
    10.8\,k\dg\\
    dynarray & 95\,k & 2.07\,M & 2.7\,M & 3.08\,M & 3.13\,M & 28.9\,M & --\\
    vector & 162\,k & 8.15\,M & 8.95\,M & 10.3\,M & 10.6\,M & 333\,M & --\\
    \hline
\end{tabularx}

# Optimalizace pro verifikaci

## Optimalizace pro verifikaci

*   využití optimalizací pro zmenšení stavového prostoru, nebo paměťových nároků
*   nesmí změnit platnost verifikovatelných vlastností
*   nesmí zvětšit stavový prostor

. . .

*   $\rightarrow$ generické optimalizace kompilátoru nelze použít
*   navrženo několik optimalizací

. . .

*   vesměs mají vliv na paměťovou náročnost verifikace

## Optimalizace: vyhodnocení

## Závěr

Shrnutí

*   demonstrována použitelnost LLVM transformací pro model checking v DIVINE
*   podpora pro paměťové modely pomocí LLVM transforamce
*   optimalizace zachovávající verifikované vlastnosti

. . .

Plány do budoucna

*   další snahy o redukci stavového prostoru při použití memory modelu
*   vyžití dalších analýz

. . .

\bigskip
\hfill{}Děkuji za pozornost

