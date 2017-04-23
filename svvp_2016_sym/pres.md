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
lang: czech
date: 27\. Dubna 2017
...

## Cíle projektu

*   nástroj DIVINE pro ověřování vlastností vstupně-výstupně uzavřených programů
*   nástroj SymDIVINE -- prototypový nástroj pro ověřování vlastností
    vstupně-výstupně otevřených programů
    *   za využití SMT solveru

. . .

*   cíle projektu jsou
    1.  navržení techniky pro efektivnější využívání SMT solveru v dotazech v
        nástroji SymDIVINE za pomoci cache dotazů
    2.  integrace DIVINE a SymDIVINE

## Cíl 1: Cache dotazů pro SymDIVINE

*   SymDIVINE posílá kvantifikované dotazy pro porovnávání stavů SMT solveru
*   následující stavy jsou často podobné
*   naivnímu využití cache dotazů brání kvantifikátory

. . .

*   vyvinuta technika rozdělení stavů na datově závislé komponenty
*   následně stačí porovnávat po komponentách
    *   umožňuje cachovat dotazy na nezměněné komponenty
    *   komponenty může být potřeba spojovat podle struktury obou stavů

. . .

*   prezentace na MEMICS, článek v přípravě
*   TODO: graf?

## Cíl 2: Integrace DIVINE a SymDIVINE

*   SymDIVINE 
