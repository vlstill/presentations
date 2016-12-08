---
vim: spell spelllang=cs tw=80 fo+=t
title: Solving QBF with Counterexample Guided Refinement
author:
    - Mikoláš Janota
    - William Klieber
    - Joao Marques-Silva
    - Edmund Clarke
    - \
    - presented by Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \usetikzlibrary{fit,backgrounds}
    - \usepackage{sansmathfonts}
    - \usepackage[small]{eulervm}
    - \institute{}
lang: czech
date: 8th December 2016
...

## Připomenutí

*   kvantifikované booleovské formule (QBF)
    $$ \exists x (x \land \forall y \exists z((y \lor z) \land (\lnot y \lor \lnot z))) $$

*   v prenexové normální formě
    $$ \exists x \forall y \exists z (x \land (y \lor z) \land (\lnot y \lor
    \lnot z ))$$

    *   není-li uvedeno jinak, uvažujeme QBF v prenexové formě bez volných
        proměnných

*   dosazení: $\varphi[\tau]$ ($\tau$ je množina nekonfliktních literálů)
    $$ (\forall x\, \exists y.\; x \lor y)[\set{\lnot x}] \rightsquigarrow \exists
    y.\; y $$

## Bloky kvantifikátorů

*   namísto
    $$ \forall x_1 … \forall x_n\,\exists x_1 … \exists x_m.\ \varphi $$
    budeme psát
    $$ \forall X\,\exists Y.\ \varphi $$
    kde $X = \set{ x_1, …, x_n }$ a $Y = \set{ y_1, …, y_m }$

\bigskip

*   maximální bloky: nelze do nich přidat další proměnné

*   alternace:
    $$QX\,\overline{Q}Y\,QZ.\ \varphi$$
    *   jestliže $Q = \forall$ pak $\overline{Q} = \exists$
    *   jestliže $Q = \exists$ pak $\overline{Q} = \forall$

## Výherní tah

Uvažme (uzavřenou) QBF ve tvaru
$$\varphi = QX.\;\psi$$
($\psi$ může obsahovat kvantifikátory)
a přiřazení $\tau$ proměnným z $X$

pak $\tau$ je **výherní tah** pro $\varphi$, jestliže

*   $Q = \exists$ a $\varphi[\tau] = \top$
*   $Q = \forall$ a $\varphi[\tau] = \bot$

. . .

**Značení**: $\mathcal{M}(\varphi)$ je množina všech výherních tahů pro $\varphi$

. . .

### Pozorování

Nechť $\varphi$ je QBF (ne nutně uzavřená)

*   uzavřená QBF $\exists X.\; \varphi$ je pravdivá, jestliže **existuje** výherní
    tah pro $\exists X.\; \varphi$
*   uzavřená QBF $\forall X.\; \varphi$ je pravdivá, jestliže **neexistuje** výherní
    tah pro $\forall X.\; \varphi$

## Expanze kvantifikátorů

*   označme $\mathcal{B}^Y$ množinu všech přiřazení proměnným v množině $Y$

Nechť $\varphi$ je QBF, pak platí:

$$ \mathcal{M}(\forall X\,\exists Y.\;\varphi) = \mathcal{M}(\forall X.
\bigvee_{\mu \in \mathcal{B}^Y} \varphi[\mu]) $$

$$ \mathcal{M}(\exists X\,\forall Y.\;\varphi) = \mathcal{M}(\exists X.
\bigwedge_{\mu \in \mathcal{B}^Y} \varphi[\mu]) $$

*   expanze je drahá (exponenciální nárůst formule)
*   myšlenka: omezení a následný refinement (zpřesňování) expanze

## $\omega$-abstrakce

nechť $\omega \subseteq \mathcal{B}^Y$, pak

*   $\omega$-abstrakce uzavřené QBF $\forall X\,\exists Y.\;\varphi$ je
    $\forall X.\;\bigvee_{\mu\in\omega} \varphi[\mu]$

*   $\omega$-abstrakce uzavřené QBF $\exists X\,\forall Y.\;\varphi$ je
    $\exists X.\;\bigwedge_{\mu\in\omega} \varphi[\mu]$

### Pozorování

*   libovolný výherní tah QBF $\varphi = QX\,\overline{Q}Y.\;\psi$ je také výherním
    tahem pro libovolnou $\omega$-abstrakci $\varphi$
*   opačný vztah neplatí

## CEGAR

**Myšlenka**: nalezneme výherní tah pro $\omega$-abstrakci formule $\varphi$ a
ověříme, že je je také výherním tahem pro $\varphi$

*   pokud je, rozhodli jsme (ne)pravdivost $\varphi$
*   pokud není, musíme zlepšit abstrakci a zkusit to znovu (refinement)

. . .

### Pozorování

Dosazení $\tau$ je výherním tahem pro uzavřenou QBF $\varphi =
QX\,\overline{Q}Y.\;\psi$ právě tehdy když $\overline{Q}.\;\psi[\tau]$ nemá
žádný výherní tah

*   tedy ověření vítězného tahu je opět QBF problém, ale s méně kvantifikátory
*   výherní tah pro $\overline{Q}.\;\psi[\tau]$ je **protipříkladem** pro to, aby
    $\tau$ byl výherním tahem pro $\varphi$

## CEGAR

máme $\omega$-abstrakci QBF $\varphi = QX\,\overline{Q}Y.\;\psi$, $\tau$ je výherní tah pro tuto
$\omega$-abstrakci

*   výherní tah $\mu$ pro $\overline{Q}.\;\psi[\tau]$ je **protipříkladem** pro to, aby
    $\tau$ byl výherním tahem pro $\varphi$
*   pak zesílíme abstrakci přidáním $\mu$ do $\omega$

## Příklad {.t}

$$ \exists x\,\forall y\,\exists z.\; x \land (y \lor z) \land (\lnot y \lor
\lnot z) $$

\begin{latex}
\only<1-2>{
Krok 1
\begin{itemize}
    \item formule: $\exists x\,\forall y\,\exists z.\; x \land (y \lor z) \land (\lnot y \lor \lnot z)$
    \item $\omega = \emptyset$
    \item abstrakce: $\exists x.\;\top$ $\rightsquigarrow$ řešení $\set{\lnot x}$
    \item potenciální výherní tah $\set{\lnot x}$
    \item ověření: $(\forall y\,\exists z.\; x \land (y \lor z) \land (\lnot y \lor \lnot z))[\set{\lnot x}]$ \\
        $=\forall y\,\exists z.\; \bot$
        \only<2>{
        \begin{itemize}
            \item protipříklad: $\set{y}$ ($\forall$ hráč má výtězný tah)
            \item $\omega \leftarrow \omega \cup \set{\set{y}}$
        \end{itemize}
        }
\end{itemize}
}
\only<3-4,10>{
Krok 2
\begin{itemize}
    \item formule: $\exists x\,\forall y\,\exists z.\; x \land (y \lor z) \land (\lnot y \lor \lnot z)$
    \item $\omega = \set{\set{y}}$
    \item abstrakce: $\exists x\,\exists z.\;x \land \lnot z$ $\rightsquigarrow$ řešení $\set{x, \lnot z}$
        \\ (řešení lze vypočítat SAT solverem)
    \item potenciální výherní tah \set{x} \\
        (bereme pouze proměnné z vnější kvantifikace)
    \item ověření: $(\forall y\,\exists z.\; x \land (y \lor z) \land (\lnot y
    \lor \lnot z))[\set{x}]$ \\
        $= \forall y\,\exists z.\;(y \lor z) \land (\lnot y \lor \lnot z)$
        \only<4>{
        \begin{itemize}
            \item řešíme rekurzivně
        \end{itemize}
        }
        \only<10->{
        \begin{itemize}
            \item protipříklad nenalezen (neexistuje výherní strategie pro
            $\forall$)
            \item máme ověřený výherní tah pro $\exists$, formule je pravdivá
        \end{itemize}
        }
\end{itemize}
}
\only<5-6>{
Krok 2 $\rightarrow$ rekurze 1
\begin{itemize}
    \item formule: $\forall y\,\exists z.\;(y \lor z) \land (\lnot y \lor \lnot z)$
    \item $\omega = \emptyset$
    \item abstrakce: $\forall y.\;\bot \rightsquigarrow$ řešení \set{\lnot y}
    \item potenciální výherní tah \set{\lnot y} (chceme výsledek $\bot$)
    \item ověření: $\exists z.\; z$
        \only<6>{
        \begin{itemize}
            \item protipříklad \set{z} ($\exists$ hráč má výtězný tah)
            \item $\omega \leftarrow \omega \cup \set{\set{z}}$
        \end{itemize}
        }
\end{itemize}
}
\only<7-8>{
Krok 2 $\rightarrow$ rekurze 2
\begin{itemize}
    \item formule: $\forall y\,\exists z.\;(y \lor z) \land (\lnot y \lor \lnot z)$
    \item $\omega = \set{\set{z}}$
    \item abstrakce: $\forall y.\; \lnot y \rightsquigarrow$ řešení \set{y}
    \item potenciální výherní tah \set{y} (chceme výsledek $\bot$)
    \item ověření: $\exists z.\; \lnot z$
        \only<8>{
        \begin{itemize}
            \item protipříklad \set{\lnot z} ($\exists$ hráč má výtězný tah)
            \item $\omega \leftarrow \omega \cup \set{\set{z}}$
        \end{itemize}
        }
\end{itemize}
}
\only<9>{
Krok 2 $\rightarrow$ rekurze 3
\begin{itemize}
    \item formule: $\forall y\,\exists z.\;(y \lor z) \land (\lnot y \lor \lnot z)$
    \item $\omega = \set{\set{z}, \set{\lnot z}}$
    \item abstrakce: $\forall y.\; \lnot y \lor y$ je $\top$ \\
        (zjistíme dotazem na SAT na negaci, tedy $y \land \lnot y$, ta je unsat
        a tedy neexistuje výherní tah pro $\forall$)
    \item výherní tah neexistuje
    \item vynoření z rekurze, protipříklad nenalezen
\end{itemize}
}
\end{latex}

## Algoritmus

\includegraphics[page=4, clip, trim=1cm 12.5cm 1cm 2cm, width=\textwidth]{paper}
