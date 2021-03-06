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
    - \setbeamersize{text margin left=1.2em, text margin right=1.2em}
    - \input{../../names.tex}
lang: czech
date: 3\. Března 2017
...

## Shrnutí posledního čtvrt roku I

*   DIVINE 4.0 final (9. 1.) \pause
*   2\. DIVINE sprint -- 9. 1. -- 15. 1.
    *   prototyp DIVINE s podporou symbolické verifikace
    *   přidávání benchmarků a testů, související úpravy
        *   doplněny některé chybějící části knihovny (libm, `regex.h`, …)
        *   C++ benchmarky s podporou Google Test, CATCH
    *   prototyp provolávání systémových volání v `divine run`
    *   monitory

## Shrnutí posledního čtvrt roku II

*   upraven bitcode linker
*   testy pro PDClib (implementace standardní C knihovny)
    *   nalezené chyby: přístup k neinicializované paměti, přístup mimo meze
        paměti, chyby v printf
    \pause
*   VM obrazy DIVINE ke stažení \pause
*   vzniká sada benchmarků a benchmarkovací nástroj, pravidelnější analýza
    výkonu DIVINE
    *   testy standardní knihovny C, libc++, DIVINE, programovací úlohy, …
    \pause
*   optimalizace implementace výjimek, optimalizace výkonu haldy \pause
*   nezávislý článek \uv{Bounded model checking of C++ programs based on the
    Qt cross-platform framework} (Software testing Verification & Reliability)
    *   porovnání DIVINE s ESBMC, LLBMC -- DIVINE 3 měl nejméně false positives,
        srovnatelné pokrytí testů, ale pomalejší (ale započítán i čas kompilace
        knihoven)
*   nový člen týmu: \zuz

## Aktuální plány

\vspace*{-\medskipamount}

**moje:**

*   blízké publikace: výjimky, architektura DIVINE 4
    *   další benchmarky, testy
    *   evaluace
    \pause
*   `divine-cc`/`divine-ld` pro kompilaci větších projektů \pause
*   možnosti redukce stavového prostoru \pause

\hrule\medskip

**celkové:**

*   run-mód DIVINE
    *   provolávání syscallů do systému
    *   přesná kontrola paměti (lepší než `valgrind`), složitější vlastnosti než
        sanitizery
    *   aktuálně základ implementace, nutno rozšířit
    \pause
*   další publikace: halda, simulátor, symbolická verifikace
    *   třeba rozšíření implementace → očekávané zlepšení výkonu i
        použitelnosti DIVINE
    \pause
*   podpora procesů \pause
*   nový člen týmu: \hh

