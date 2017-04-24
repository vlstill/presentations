---
vim: spell spelllang=cs tw=80 fo+=t
title: Rozšíření nástroje DIVINE pro verifikaci vstupně-výstupně otevřených programů
author:
    - Henrich Lauko
    - Jan Mrázek
    - Vladimír Štill
    - \
    - "garant: Jiří Barnat"
header-includes:
    - \usepackage{divine}
    - \usepackage[small]{eulervm}
    - \newcommand{\TODO}[1]{{\color{red}#1}}
    - \usepackage{pbox}
    - \usepackage{booktabs}
    - \usepackage{siunitx}
    - \usepackage{tabularx}
lang: czech
date: 27\. dubna 2017
...

## Cíle projektu

*   nástroj DIVINE pro ověřování vlastností vstupně-výstupně uzavřených programů
*   nástroj SymDIVINE -- prototypový nástroj pro ověřování vlastností
    vstupně-výstupně otevřených programů

. . .

*   cíle projektu jsou
    1.  navržení techniky pro efektivnější využívání SMT solveru v dotazech v
        nástroji SymDIVINE za pomoci cache dotazů
    2.  integrace DIVINE a SymDIVINE

## Cíl 1: Cache dotazů pro SymDIVINE

*   SymDIVINE posílá kvantifikované dotazy na SMT solver pro porovnávání
    symbolických stavů programu
*   inkrementální tvorba stavů $\rightarrow$ podobné dotazy
*   naivnímu využití cache dotazů brání kvantifikátory

. . .

*   vyvinuta technika rozdělení stavů na datově nezávislé komponenty
*   následně stačí porovnávat po komponentách
    *   umožňuje cachovat dotazy na nezměněné komponenty
    *   komponenty může být potřeba spojovat podle struktury obou stavů
*   optimalizace nepřináší overhead

. . .

*   prezentace na MEMICS, článek v přípravě

## Cíl 1: Cache dotazů pro SymDIVINE

\begin{tabular}{lp{0.3cm}rrp{0.3cm}rrp{0.3cm}rrp{0.3cm}rr}
    \toprule
        &&
    \multicolumn{2}{c}{\pbox{20cm}{\relax\ifvmode\centering\fi Bez cache}} &&
    \multicolumn{2}{c}{\pbox{20cm}{\relax\ifvmode\centering\fi S cache}} \\
    \addlinespace
    \cmidrule{3-4}
    \cmidrule{6-7}
    Kategorie && Čas[s] & Vyřešeno && Čas[s] & Vyřešeno \\
    \midrule
    Concurrency && 1828 & 40 && \textbf{1506} & \textbf{42} \\
    DeviceDrivers && 12156 & 241 &&  \textbf{763} & \textbf{298} \\
    ECA && \textbf{20794} & \textbf{230} && 21606 & 211 \\
    ProductLines && 19571 & 276 && \textbf{11995} & \textbf{293} \\
    Sequentialized && 3710 & 44 && \textbf{1735} & \textbf{47} \\ \midrule
    \textbf{Celkem} && 58061 & 831 && \textbf{37607} & \textbf{891} \\
    \bottomrule
\end{tabular}


## Cíl 2: Integrace DIVINE a SymDIVINE

*   SymDIVINE přináší podporu pro vstupně-výstupně otevřené programy v C/C++
    *   jedná se ale o prototypový nástroj sloužící primárně k demonstraci
        použitelnosti techniky Control-Explicit Data-Symbolic model checkingu
    *   z dlouhodobého hlediska je nepraktické udržovat DIVINE i SymDIVINE

. . .

*   potřeba integrovat SymDIVINE do DIVINE bez zásadního zkomplikování jádra
    DIVINE

. . .

*   myšlenka: zakódování manipulací se symbolickými daty do vstupního programu +
    algoritmus schopný procházet symbolický stavový prostor

## Cíl 2: Integrace DIVINE a SymDIVINE

*   symbolická data = vstupy
    *   lze reprezentovat pomocí formulí v bitvektorové logice
    *   v SymDIVINE reprezentaci vytváří model checker
    *   DIVINE instrumentuje program tak, aby formuli vytvářel sám

. . .

*   využití SMT solveru
    *   zjištění neprázdnosti stavu
    *   porovnávání symbolických částí stavů

. . .

*   do DIVINE přidány:
    *   interní formát reprezentace formulí
    *   základní verze instrumentace/symbolizace programu
    *   podpora pro označování části paměti za symbolickou
        *   symbolická paměť porovnávána pomocí SMT solveru
    *   podpora pro volání SMT solveru
