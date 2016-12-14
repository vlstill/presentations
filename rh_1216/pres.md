---
vim: spell spelllang=cs tw=80 fo+=t
title: DIVINE
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \usetikzlibrary{fit,backgrounds}
    - \usepackage{sansmathfonts}
    - \usepackage[small]{eulervm}
lang: czech
date: 16th December 2016
...

## Stav DIVINE 4

*   první beta verze 1. 12.
*   druhá 14. 12.
*   třetí beta a RC do Vánoc
*   release pravděpodobně v lednu
*   mnoho novinek

## Novinky v DIVINE (oproti Q3)

*   refaktoring knihovny `pthreads`
*   vylepšené redukce stavového prostoru
*   nová implementace výjimek
*   přizpůsobení virtuálního filesystému pro běh v DIVINE 4
*   nové testy

## Testy

-   mnoho nových vlastních testů
-   část testů na knihovnu `bricks` (součást DIVINE) nyní verifikovány DIVINE
    *   z 22 testů DIVINE zvládá 16
    *   4 testy využívají procesy, případně některé nepodporované
        syscally (`mmap`, část FS)
    *   2 vyžadují sledování inicializace po bitech
-   Posix Test Suite testy na knihovnu `pthreads`
    *   156 testů, ? aktivních

