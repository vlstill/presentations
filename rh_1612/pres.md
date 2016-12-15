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

# Shrnutí prvního roku PhD

## Únor až duben

*   nástup na PhD, dokončování publikace o podpoře relaxovaných paměťových
    modelů pro DIVINE 3
    *   nakonec nepublikováno z důvodu nalezení chyby v implementaci, odloženo
*   prezentace DIVINE 3 na SV-COMP/TACAS 2016 v Eindhovenu
*   práce na kompilátoru pro DIVINE
    *   integrace clangu
    *   robustnější prostředí pro kompilaci
    *   nezávislost na systémovém kompilátoru a jeho nastavení
    *   nezávislost na systémových hlavičkových souborech

## Květen až červenec

*   v květnu proběhl první DIVINE sprint, vznikla první interně použitelná verze
    DIVINE 4
*   dokončování kompilace
*   instrumentace LLVM pro DIVINE 4
*   metadata pro potřeby programu/DiOS-u (DIVINE Operating System -- poskytuje
    podporu pro vlákna, části POSIX verifikovanému programu)
*   podpora C/C++ knihoven pro DIVINE 4

## Srpen až říjen

*   implementace výjimek a `setjmp`/`longjmp` pro DIVINE 4
    *   robustnější podpora výjimek založená na vlastním unwinderu
    *   funguje bez modifikací C++ knihovny
*   statické redukce stavového prostoru na základně detekce lokálních proměnných
*   podíl na přípravě článku o architektuře DiVM
    *   Ročkai, Mrázek, Štill, Barnat: *A Virtual Machine with Graph-Organised
        Memory for Model Checking*, odesláno na TACAS

## Listopad a prosinec

*   refaktoring a optimalizace podpory vláken (`pthreads`)
    *   optimalizace Thread Local Storage
    *   robustnější kontrola chyb
    *   využití symetrie vláken k redukci stavového prostoru
*   testy, větší množství oprav
    *   testy knihovny `bricks`, kterou DIVINE využívá částečně verifikovány
        DIVINE
    *   díky tomu identifikace a částečné doplnění chybějící funkcionality
*   příprava publikace: Štill, Ročkai, Mrázek, Barnat: *Verifying
    Exception-Enabled C++ Using Unmodified Standard Library*, odesláno na NASA
    Formal Methods

# Další novinky v DIVINE 4

## Vylepšení jádra DIVINE (DiVM)

*   vylepšené redukce stavového prostoru
    *   sledování, které objekty jsou sdílené mezi vlákny
    *   zápis no soukromých objektů nemusí vyvolat přerušení
    *   podobná redukce ale menší režie než DIVINE 3
*   lepší reprezentace haldy
    *   úspora paměti
    *   rychlejší srovnávání stavů

## Vylepšení interaktivního debuggeru

*   (dostupný jako `divine sim`)
*   lepší podpora pro debugovací informace
*   mnoho opravených chyb
*   breakpointy podle jména souboru a čísla řádku
*   automatické testy

## Vylepšení DiOS

*   DiOS -- jednoduchý operační systém uvnitř DIVINE, který poskytuje části
    POSIX rozhraní verifikovanému programu
*   předávání argumentů programu (argumenty funkce `main`, `environ`,…)
*   konfigurace chování chybových stavů, simulace chyb
    *   lze deaktivovat některé kontroly
    *   nastavitelné chování `malloc`, …
*   přepracovaný systém systémových volání

## Podpora pro virtuální souborový v DIVINE 4

*   DIVINE 4 nyní podporuje základní funkce pro práci se souborovým systémem
*   lze zachytit část souborového systému, vstup
*   výstup (`printf`/`write`…) zaznamenáván, součástí případného protipříkladu

## Symbolizace programu

*   \TODO{TODO}

## Aktuální stav DIVINE 4

*   první beta verze 1. 12.
*   druhá 14. 12.
*   RC do Vánoc
*   finální verze pravděpodobně v lednu
    *   v plánu dát k dispozici virtuální stoj s predinstalovaným DIVINE
*   podali jsme návrh na prezentaci a workshop na DevConf, obojí ale zamítnuto

*   rozšíření dokumentace na webu
*   7 vývojářů

# Pohled do roku 2017

## Výhled na rok 2017

*   větší podpora POSIX systémových volání
    *   pass-through mód pro run -- volání systémových volání Linuxu z `divine
        run`
*   podpora procesů, meziprocesové komunikace
*   podpora verifikace se symbolickým/abstrahovaným vstupem
*   další vylepšování redukce a komprese stavového prostoru
