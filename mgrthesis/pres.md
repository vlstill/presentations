---
title: LLVM Transformations for Model Checking
subtitle: Magisterská práce
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \input{tlua}

lang: czech
date: 15\. února 2016
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

*   LLVM IR: mezijazyk používaný při překladu
*   LLVM: IR + knihovny pro manipulaci s IR, generování assembleru,…
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
    *   $\rightarrow$ zajištění *viditelnosti výjimek* ve funkci

# Relaxované paměťové modely

## Relaxované paměťové modely

zápis do paměti není na moderních CPU okamžitě viditelný ostatním procesorům

*   store buffery, cache, optimalizace cache coherence protokolu
*   okamžitá viditelnost odpovídá *sekvenční konzistenci* (*Sequential Consistency*, SC)
*   reálná CPU se chovají neintuitivně

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
      \node () [anchor=west] at (-10,0.5) {paměť};
      \node () [anchor=west] at (-10,-2.5)  {\texttt{@x}};
      \node () [anchor=west] at (-9,-2.5)  {\texttt{0}};

      \node () [anchor=west] at (-10,-3.5)  {\texttt{@y}};
      \node () [anchor=west] at (-9,-3.5)  {\texttt{0}};

      \node () [anchor=center] at (-2,-5.5) {store buffer vlákna 0};
      \node () [anchor=center] at (4,-5.5) {store buffer vlákna 1};

      \draw [-] (-4,-4) rectangle (0,-5);
      \draw [-] (2,-4) rectangle (6,-5);
      \draw [-] (-2,-4) -- (-2,-5);
      \draw [-] (4,-4) -- (4,-5);

      \node<3-> () [anchor=west] at (-4,-4.5)  {\texttt{@y}};
      \node<3-> () [anchor=west] at (-2,-4.5)  {\texttt{1}};

      \node<5-> () [anchor=west] at (2,-4.5)  {\texttt{@x}};
      \node<5-> () [anchor=west] at (4,-4.5)  {\texttt{1}};

      \node () [] at (-4, 0.5) {vlákno 0};
      \draw [->] (-4,0) -- (-4,-2.3);
      \node () [anchor=west, onslide={<3> font=\bf, color=red}] at (-3.5, -0.5) {\texttt{store @y 1;}};
      \node () [anchor=west, onslide={<4> font=\bf, color=red}] at (-3.5, -1.5) {\texttt{load @x;}};

      \node () [] at (2, 0.5) {vlákno 1};
      \draw [->] (2,0) -- (2,-3.3);
      \node () [anchor=west, onslide={<5> font=\bf, color=red}] at (2.5, -0.5) {\texttt{store @x 1;}};
      \node () [anchor=west, onslide={<6> font=\bf, color=red}] at (2.5, -1.5) {\texttt{load @y;}};
      \node () [anchor=west, onslide={<7> font=\bf, color=red}] at (2.5, -2.5) {\texttt{load @x;}};

      \draw<3-> [->, dashed] (0.3,-0.5) to[in=0, out=0] (0,-4.5);
      \draw<4-> [->, dashed] (-7,-2.5) to[in=0, out=0, out looseness = 3, in looseness=0.5] (-0.7,-1.5);
      \draw<5-> [->, dashed] (6.3,-0.5) to[in=0, out=0] (6,-4.5);
      \draw<6-> [->, dashed] (-7,-3.5) to[in=0, out=0, out looseness = 0.2, in looseness = 0.7] (5.3,-1.5);
      \draw<7-> [->, dashed] (6,-4.5) to[in=0, out=0] (5.3,-2.5);

      \draw<-2> [->] (-4,-0.3) to (-3.4,-0.3);
      \draw<3> [->] (-4,-0.7) to (-3.4,-0.7);
      \draw<4-> [->] (-4,-1.7) to (-3.4,-1.7);

      \draw<-4> [->] (2,-0.3) to (2.6,-0.3);
      \draw<5> [->] (2,-0.7) to (2.6,-0.7);
      \draw<6> [->] (2,-1.7) to (2.6,-1.7);
      \draw<7> [->] (2,-2.7) to (2.6,-2.7);
  \end{tikzpicture}
  }
\end{latex}

## Verifikace paměťových modelů

*   verifikační nástroje často podporují jen sekvenční konzistenci
*   stejně tak DIVINE

. . .

**dva možné přístupy k rozšíření funkcionality**

\begin{latex}
    \begin{minipage}[t]{0.47\textwidth}

    \begin{block}{modifikace interpretru}

    \begin{itemize}
    \tightlist
    \item
      specifické pro DIVINE
    \item
      vyžaduje zásah do složitého interpretru LLVM
    \item
      velmi náročné na ladění při vývoji
    \item
      nevyžaduje nic navíc od uživatele, jen zapnout při verifikaci modelu
    \end{itemize}
    \vfill
    \end{block}

    \end{minipage}\hspace*{0.04\textwidth}\pause\begin{minipage}[t]{0.47\textwidth}

    \begin{block}{\textbf{modifikace vstupního programu}}

    \begin{itemize}
    \tightlist
    \item
      na úrovni C++, nebo \textbf{LLVM}
    \item
      na úrovni LLVM výrazně jednodušší
    \item
      lze modifikovat pro jiné nástroje než DIVINE
    \item
      nevyžaduje modifikaci programu uživatelem, jen zapnout při překladu
    \end{itemize}
    \vfill
    \end{block}
    \end{minipage}
\end{latex}

## LLVM transformace pro paměťové modely

*   přidává podporu LLVM paměťového modelu do programu
*   téměř úplné pokrytí paměťového modelu C++11
*   paměťový model definuje atomické instrukce (compare-and-swap, read-modify-write)
*   simulace pomocí store bufferů rozšířených o informace o instrukci

. . .

*   transformace je parametrizovatelná
    *   jaké garance paměťový model dává (reálné paměťové modely dávají často větší garance než LLVM)
    *   jak velký má být store buffer

## Princip transformace

*   každý zápis, čtení, atomická instrukce a paměťová bariéra transformovány
    *   zápis proběhne do store bufferu
    *   čtení kontroluje, jestli je v lokálním store bufferu nová hodnota
*   přidáno vlákno, které vylévá store buffer

. . .

*   atomické instrukce se simulují pomocí synchronizace + atomické masky
*   ostatní nahrazeny funkcemi (implementované v C++)

. . .

*   nutno zabránit opožděnému zápisu po uvolnění paměti
    *   vylitím store bufferu po uvolnění paměti/skončení funkce
    *   dané lokace odstraněny ze store bufferu
    *   vyžaduje *viditelné* výjimky

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

\renewcommand{\thefootnote}{\dag}
\footnotetext{DIVINE nalezl v modelu chybu}

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

\renewcommand{\thefootnote}{\dag}
\footnotetext{DIVINE nalezl v modelu chybu}

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

\renewcommand{\thefootnote}{\dag}
\footnotetext{DIVINE nalezl v modelu chybu}

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

\begin{tabularx}{\textwidth}{|>{\tt\,}l<{\,}|CCC|CCC|} \hline
    \multicolumn{1}{|c|}{\sf Jméno} & \multicolumn{3}{c|}{Čas} & \multicolumn{3}{c|}{Paměť} \\
    & původní & opt. & zrychl. & původní & opt. & úspora \\ \hline
    fifo & \si{791} & \si{791} & \speedup{791}{791} & \dmem{388724} & \dmem{338960} & \speedup{388724}{338960} \\
    fifo-tso-3 & \si{48787} & \si{42098} & \speedup{48787}{42098} & \dmem{703484} & \dmem{522304} & \speedup{703484}{522304} \\
    elevator2 & \si{17720078} & \si{11482370} & \speedup{17720078}{11482370} & \dmem{1296532} & \dmem{1328844} & \speedup{1296532}{1328844} \\
    hs-2-1-0 & \si{890973} & \si{874778} & \speedup{890973}{874778} & \dmem{659780} & \dmem{345060} & \speedup{659780}{345060} \\
    hs-2-1-0-tso-1 & \si{250390514} & \si{184001777} & \speedup{250390514}{184001777} & \dmem{32274396} & \dmem{19012388} & \speedup{32274396}{19012388} \\
    hs-2-2-2 & \si{2328550} & \si{2292383} & \speedup{2328550}{2292383} & \dmem{1266776} & \dmem{873252} & \speedup{1266776}{873252} \\ \hline
\end{tabularx}

opt. = odstranění konstantních lokálních proměnných + detekce konstantních globálních proměnných

## Závěr

Shrnutí

*   demonstrována použitelnost LLVM transformací pro model checking v DIVINE
*   podpora pro paměťové modely pomocí LLVM transformace
*   optimalizace zachovávající verifikované vlastnosti

. . .

Plány do budoucna

*   další snahy o redukci stavového prostoru při použití memory modelu
*   využití dalších analýz k redukci stavového prostoru

. . .

\bigskip
\hfill{}Děkuji za pozornost


# Dotazy

## Dotaz vedoucího

Bylo by možné zvýšit míru redukce stavového prostoru dosahovaného
transformacemi, za předpokladu, že se verifikace programu zaměří na
jeden vybraný konkrétní problém, řekněme třeba detekci deadlocku?

*   ano, některé metody, například slicing, mohou fungovat lépe při omezené
    množině verifikovaných vlastností
*   pro detekci deadlocku je však třeba zachovat implementaci synchronizace a
    kódu, který k ní vede

## Dotazy oponenta

V obrázku 4.5 je napsáno, že řádek `foo( ptr );` je maskován, pokud byla
maskována funkce volající `doSomething`. Z textu jsem však nabyl dojmu, že tento
řádek není maskován za žádných okolností. Jak to tedy je?

```{.cpp .numberLines}
void doSomething( int *ptr, int val ) {
    divine::InterruptMask mask;
    *ptr += val;
    // release the mask only if 'mask' object owns it:
    mask.release();
    // masked only if caller of doSomething was masked:
    foo( ptr );
}
```

\begin{latex}
\only<1>{Pokud není \texttt{doSomething} voláno pod maskou, pak objekt \texttt{mask} na řádku 2 vlastní masku a ta je na řádku 5 uvolněna.}
\only<2>{Pokud je \texttt{doSomething} voláno pod maskou, pak \texttt{mask} objekt nemá na tuto masku žádný vliv, a tedy řádek 7 je pod (vnější) maskou.}
\end{latex}

## Dotazy oponenta

Proč se liší první číselné sloupce v tabulkách 5.5 a 5.6?

*   v tabulce 5.5 představuje první sloupec neoptimalizovanou transformaci přidání memory modelů, avšak $\tau+$ redukce je v nové variantě
*   v tabulce 5.6 představuje první sloupec starou verzi $\tau+$ redukce, avšak transformace je optimalizovaná
*   poslední sloupec v obou tabulkách obsahuje výsledky se stejnými optimalizacemi
