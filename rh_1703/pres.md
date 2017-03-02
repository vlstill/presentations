---
vim: spell spelllang=cs tw=80 fo+=t
title: DIVINE 4
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \usetikzlibrary{fit,backgrounds}
    - \usepackage{sansmathfonts}
    - \usepackage[small]{eulervm}
    - \newcommand{\TODO}[1]{{\color{red}#1}}
lang: czech
date: 3\. Března 2017
...

## Shrnutí posledního čtvrt roku

*   DIVINE 4 final (9. 1.) \pause
*   2\. DIVINE sprint -- 9. 1. -- 15. 1.
    *   prototyp DIVINE s podporou symbolické verifikace
    *   přidávání benchmarků a testů, související úpravy
        *   doplněny některé chybějící části knihovny (libm, …)
        *   C++ benchmarky s podporou Google Test, CATCH
    *   prototyp provolávání systémových volání v `divine run`
    \pause
*   upraven bitcode linker
*   testy pro PDClib (implementace standardní C knihovny)
    *   nalezené chyby: přístup k neinicializované paměti, out-of-bound access,
        chyby v printf
    \pause
*   VM obrazy DIVINE ke stažení \pause
*   vzniká sada benchmarků a benchmarkovací nástroj, pravidelnější analýza
    výkonu DIVINE
    *   testy standardní knihovny C, libc++, DIVINE, programovací úlohy, …
    \pause
*   optimalizace implementace výjimek, optimalizace výkonu haldy

## Aktuální plány

*   blízké publikace: výjimky, architektura DIVINE 4
    *   další benchmarky, testy
    *   evaluace
    \pause
*   run-mód DIVINE
    *   provolávání syscallů do systému
    *   přesná kontrola paměti (lepší než `valgrind`), složitější vlastnosti než
        sanitizery
    *   aktuálně základ implementace, třeba rozšířit
    \pause
*   další publikace: halda, simulátor, symbolická verifikace
    *   třeba rozšíření implementace → očekávané zlepšení výkonu i
        použitelnosti DIVINE
    \pause
*   `divine-cc`/`divine-ld` pro kompilaci větších projektů

