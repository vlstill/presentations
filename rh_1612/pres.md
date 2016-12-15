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
    - \newcommand{\TODO}[1]{{\color{red}#1}}
lang: czech
date: 16th December 2016
...

## Stav DIVINE 4

*   první beta verze 1. 12.
*   druhá 14. 12.
*   třetí beta a RC do Vánoc
*   release pravděpodobně v lednu
    *   v plánu dát k dispozici virtuální stoj s predinstalovaným DIVINE
*   mnoho novinek
*   7 vývojářů

## Vylepšení knihovny `pthreads`

*   DIVINE potřebuje vlastní implementaci `pthreads`
*   původní implementace z roku 2013, postupně upravována
*   zásadnější změny související s DIVINE 4
    *   robustnější kontrola chyb (re-inicializace, mutex zamčený mrtvým
        vláknem,…)
    *   efektivnější Thread-Local Storage
    *   nová implementace umožňuje využít symetrie vláken k redukci stavového
        prostoru
    *   lepší pokrytí testy

## Vylepšená podpora výjimek, `setjmp`/`longjmp`

*   podpora C++ výjimek pro DIVINE 4 přepracována
    *   implementována podmnožina `libunwind` používaná pro výjimky
    *   kód v C++ knihovně pro podporu výjimek běží v DIVINE 4 nezměněný
*   přidána podpora pro `setjmp`/`longjmp`
*   článek: Štill, Ročkai, Mrázek, Barnat: *Verifying Exception-Enabled C++ Using
    Unmodified Standard Library*, odesláno na NASA Formal Methods

## Vylepšení jádra DIVINE (DiVM)

*   vylepšené redukce stavového prostoru
    *   sledování, které objekty jsou viditelné ostatními vlákny
    *   zápis no neviditelných objektů nemusí vyvolat interrupt
    *   rychlejší sledování viditelnosti objektu než v DIVINE 3
*   \TODO{objID TODO}
*   článek: Ročkai, Mrázek, Štill, Barnat: *A Virtual Machine with
    Graph-Organised Memory for Model Checking*, odesláno na TACAS

## Vylepšení simulátoru protipříkladů

*   `divine sim`
*   lepší podpora pro debugovací informace
*   mnoho oprav chyb
*   \TODO{TODO}

## Vylepšení DiOS

*   DiOS -- jednoduchý operační systém uvnitř DIVINE, který poskytuje části
    POSIX rozhraní verifikovanému programu
*   předávání argumentů programu (argumenty funkce `main`, `environ`,…)
*   konfigurace chování chybových stavů, simulace chyb
    *   lze deaktivovat některé kontroly
    *   nastavitelné chování `malloc`, …
*   přepracovaný systém systémových volání
    *   příprava pro podporu většího množství POSIX systémových volání
    *   a možnost provolávání se z DIVINE do systému ve kterém DIVINE běží

## Podpora pro virtuální souborový v DIVINE 4

*   DIVINE 4 nyní podporuje základní funkce pro práci se souborovým systémem
*   lze zachytit část souborového systému, vstup
*   výstup (`printf`/`write`…) zaznamenáván, součástí případného protipříkladu
*   součástí DiOS, využívá nového rozhraní pro systémová volání

## Symbolizace programu

*   \TODO{TODO}

## Testy

-   mnoho nových vlastních testů
-   část testů na knihovnu `bricks` (součást DIVINE) nyní verifikovány DIVINE
    *   z 22 testů DIVINE zvládá 16
    *   4 testy využívají procesy, případně některé nepodporované
        syscally (`mmap`, část FS)
    *   2 vyžadují sledování inicializace paměti po bitech
-   Posix Test Suite testy na knihovnu `pthreads`
    *   156 testů, 78 aktivních
*   $\rightarrow$ identifikace chybějící funkcionality
    *   některé pokročilejší funkcionality `pthreads`
    *   semafory
    *   `sys/sched.h`, `sys/time.h`
    *   význačná vlastnost: \uv{nenastane chyba a program skončí} (neboli
        nenastane livelock) -- v PTS simulované pomocí `alarm`

## Různé

*   opravy fungování některých přepínačů integrovaného kompilátoru (`-x`,
    některé případy `-I`)
*   rozšíření dokumentace na webu
*   návrh na prezentaci a workshop na DevConf
    *   obojí zamítnuto

# Demo (bricks-test)

## Shrnutí prvního roku PhD {.allowframebreaks}

*   únor
    *   nástup na PhD, příprava publikace o verifikaci s relaxovanými paměťovými
        modely (v návaznosti na diplomovou práci)
        *   na základně review později odhalena chyba, prozatím odloženo
*   březen -- duben
    *   práce na kompilaci (integrace clangu do DIVINE)
    *   prezentace DIVINE 3 na SV-COMP/TACAS v Eindhovenu
*   květen -- srpen
    *   *květen: první DIVINE sprint, první použitelný prototyp DIVINE 4*
    *   instrumentace programu pro verifikaci, přidávání metadat
    *   úpravy knihoven pro DIVINE 4
    *   statické redukce stavového prostoru
    *   prototyp spustitelných protipříkladů
    *   automatické buildy DIVINE

*   září -- prosinec
    *   podpora výjimek
    *   rozsáhlejší změny v `pthreads`
    *   prezentace posteru o verifikaci programů se vstupy na MEMICS
    *   příprava publikací

průběžně různé opravy, testy, konzultace s mladšími studenty…

## Výhled na rok 2017

*   větší podpora POSIX systémových volání
    *   pass-through mód pro run -- volání systémových volání Linuxu z `divine
        run`
*   podpora procesů, meziprocesové komunikace
*   podpora verifikace se symbolickým/abstrahovaným vstupem
*   další vylepšování redukce a komprese stavového prostoru
