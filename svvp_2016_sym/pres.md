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
    - \usetikzlibrary{shapes, arrows, shadows, positioning, calc, fit, backgrounds, decorations.pathmorphing}
lang: czech
date: 27\. dubna 2017
aspectratio: 169
...

## Cíle projektu

*   nástroj DIVINE pro ověřování vlastností vstupně-výstupně uzavřených programů
*   nástroj SymDIVINE -- prototypový nástroj pro ověřování vlastností
    vstupně-výstupně otevřených programů

. . .

*   cíle projektu jsou
    1.  navržení techniky pro efektivnější využívání SMT solveru v\ dotazech
        v\ nástroji SymDIVINE za pomoci cache dotazů
    2.  integrace DIVINE a SymDIVINE

## Cíl 1: Cache dotazů pro SymDIVINE

*   SymDIVINE generuje symbolické stavy programu
    * kvantifikované dotazy na SMT solver pro rovnost stavů
    * nekvantifikované dotazy na SMT solver pro prázdnost stavů
*   inkrementální tvorba stavů $\rightarrow$ podobné dotazy
*   naivnímu využití cache dotazů brání kvantifikátory

## Cíl 1: Cache dotazů pro SymDIVINE

*   technika rozdělení stavů na datově nezávislé komponenty

\begin{figure}[!ht]
\begin{center}
\resizebox{0.7\textwidth}{!}{
    \begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=1.5cm
                       , semithick
                       , scale=0.7
                       , font=\sffamily
                       , stateprog/.style={ rectangle, draw=black, very thick,
                         minimum height=2em, minimum width = 10em, inner
                         sep=6pt, text centered, node distance = 2em, align = left,  rounded corners }
                       ]

        \node[stateprog, label=Původní reprezentace] (p1)
            {Program counter: x \\
             $a < 42~\wedge $ \\
             $a > 0~\wedge $ \\
             $b = a + 4~\wedge $ \\
             $c > 42$
             };

        \node[text centered, align = left, above right = -2.1em and 6em of p1] (pc)
            {Program counter: x};
        \node[stateprog, below = 0.5em of pc] (p2)
            {$a < 42~\wedge $ \\
             $a > 0~\wedge $ \\
             $b = a + 4$};
        \node[stateprog, below = 0.5em of p2] (p3)
            {$c > 42$};

        \node[stateprog, fit = (pc) (p2) (p3), label=Nová reprezentace] {};
    \end{tikzpicture}
    }
\end{center}
\end{figure}

*   reprezentace pomocí datově nezávislých komponent
*   porovnání po komponentách
    *   umožňuje cachovat dotazy na nezměněné komponenty
    *   komponenty je třeba spojovat podle struktury obou stavů

## Cíl 1: Cache dotazů pro SymDIVINE

\center
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

\bigskip

*   prezentace na MEMICS
*   prezentace v rámci soutěže SV-COMP
*   publikace v přípravě

## Cíl 2: Integrace DIVINE a SymDIVINE

*   SymDIVINE přináší podporu pro vstupně-výstupně otevřené programy v C/C++
    *   jedná se ale o prototypový nástroj sloužící primárně k demonstraci
        použitelnosti techniky Control-Explicit Data-Symbolic model checkingu
    *   z dlouhodobého hlediska je nepraktické udržovat DIVINE i SymDIVINE

. . .

*   potřeba integrovat SymDIVINE do DIVINE bez zásadního zkomplikování jádra
    DIVINE

## Cíl 2: Integrace DIVINE a SymDIVINE

*   myšlenka: zakódování manipulací se symbolickými daty do vstupního programu +
    algoritmus schopný procházet symbolický stavový prostor

\includegraphics[width=\textwidth]{lart}

## Cíl 2: Integrace DIVINE a SymDIVINE

*   symbolická data = vstupy
    *   lze reprezentovat pomocí formulí v bitvektorové logice
    *   v SymDIVINE reprezentaci vytváří model checker
    *   DIVINE instrumentuje program tak, aby formuli vytvářel sám


. . .

*   do DIVINE přidány:
    *   podpora pro označování části paměti za symbolickou
        *   symbolická paměť porovnávána pomocí SMT solveru
        *   využívá nového způsobu reprezentace haldy v DIVINE 4
    *   interní formát reprezentace formulí
    *   základní verze instrumentace/symbolizace programu
    *   podpora pro volání SMT solveru
