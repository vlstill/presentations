---
vim: spell spelllang=en fo+=t tw=80
title: Infitine-State Liveness-to-Safety via Implict Abstraction and Well-Founded Relations \cite{Daniel2016}
author:
    - Jakub Daniel
    - Alessandro Cimatti
    - Alberto Griggio
    - StefanoTonetta
    - Sergio Mover
    - \
    - presented by Vladimír Štill
header-includes:
    - \newcommand{\redon}[2]{{\only<#1>{\color{red}}#2}}
    - \newcommand{\FG}{\textrm{\textbf{FG}}}
lang: english
date: 18th March 2017
...

## Intro {.t}

\begin{latex}
\vspace*{3em}
\redon{2}{Infinite-State} \redon{3}{Liveness-to-Safety} via \redon{4}{Implicit
Abstraction} and \redon{5}{Well-Founded Relations}

\bigskip
\only<1>{
\begin{itemize}
    \item so what does this mean?
\end{itemize}
}

\only<2>{
\begin{itemize}
    \item we are concerned with infinite state systems
    \item more precisely infinite state transition systems described implicitly
          using first order logic formulas
    \item transition system $S = \langle X, I, T \rangle$
        \begin{itemize}
            \item $X$ -- set of state variables
            \item $I(X)$ is formula which describes initial states
            \item $T(X, X')$ is formula which describes transition (from state
            described by $X$ to state described by $X'$)
        \end{itemize}
\end{itemize}
}

\only<3>{
\begin{itemize}
    \item verification of liveness properties, namely LTL
    \item by translating them to safety properties
    \item and using solvers for safety properties (invariant  model checking
          problem)
\end{itemize}
}

\only<4>{
\begin{itemize}
    \item using predicate abstraction to obtain finite state abstract system
    \item but the abstraction is \emph{implicit} -- we don't actually generate
          the abstract transition system
\end{itemize}
}

\only<5>{
\begin{itemize}
    \item well-founded relations will be used in counterexample refinement
    \item relation ${\prec} \subseteq Q \times Q$ is \emph{well-founded} if
    every non-empty subset $U \subseteq Q$ has a minimal element with respect to
    $\prec$
\end{itemize}
}

\only<6>{
\begin{itemize}
    \item … how?
\end{itemize}
}
\end{latex}

## LTL Model Checking Using Fair Path Detection

*   given a system $S = \langle X, I, T \rangle$ and LTL formula $\phi$ over $X$
    we want to check that for all infinite paths $\pi$ of $S$, $\phi$ is true in
    $\pi$
    *   $S \models \phi$

. . .

*   build transition system $S_{\lnot \phi}$ and fairness condition
    $f_{\lnot\phi}$ such that
    *   $S \models \phi$ iff $S \times S_{\lnot \phi} \models \FG\lnot f_{\lnot\phi}$
    *   counterexample is \emph{fair path} -- path that visits $f_{\lnot\phi}$
        infinitely often
    *   \pause{}weird?\pause{} Not really… $S_{\lnot\phi}$ can be Büchi automaton
        for $\lnot\phi$ and $f$ property "state of product is accepting"

. . .

*   from now on, we will expect the transition system to be the product already
    (and simplify $f_{\lnot\phi}$ to $f$)

    *   $S \models \FG\lnot f$

## Liveness To Safety (L2S)

a technique for reducing LTL model checking problem on *finite-state* system to
an invariant model checking problem

*   $S \models \FG\lnot f$ reduced to
    $S_{L2S} \models \lnot \mathit{bad}_{L2S}$ \pause
*   finiteness is required -- counterexample needs to be lasso shaped \pause

*   idea: non-deterministically guess state where lasso starts, check that state
    satisfying fairness condition $f$ is encountered between two visits to this
    state

    \includegraphics[page=5, clip, trim=6cm 12.3cm 5.1cm 14.5cm, width=\textwidth]{paper}

    *   *seen* indicates that lasso start was seen
    *   *triggered* indicates $f$ was seen after lasso start
    *   *loop* indicates lasso was closed after $f$ was seen

## Disjunctive Well-Founded Transition Invariants

*   let $S$ be transition system with reachable states $R$
*   a relation $\rho$ over $S$ is *transition invariant* if it contains the transitive
    closure of $T$ restricted to $R$
    *   $T^+ \cap (R \times R) \subseteq \rho$
*   relation ${\prec} \subseteq Q \times Q$ is *well-founded* if every
    non-empty subset $U \subseteq Q$ has a minimal element with respect to
    $\prec$
*   relation $\rho$ is *disjunctively well-founded* if it is a finite union of
    well-founded relations

. . .

*   *termination* of program can be reduced to finding disjunctively well-founded
    transition invariant for it

## \textsc{IC3} with Implicit Abstraction

*   \textsc{IC3} works with transitions systems encoded into SAT
*   to verify invariant (= safety) properties

. . .

*   generalized to SMT with implicit predicate abstraction (\textsc{IC3ia})
    *   can introduce spurious error paths -- abstraction has to be refined
    *   can deal with infinite transition systems

## Liveness-to-Safety with Implicit Abstraction I

*   original L2S does not work on infinite systems
*   we want to find sufficiently precise predicate abstraction of the infinite
    system with does not admin path visiting fairness condition infinitely often
    *   abstracted system is always finite (as number of predicates is finite)
    *   if $\FG\lnot f$ holds in abstraction it holds also in the concrete
        system
    *   set of predicates $P = \set{ p_1(X), p_2(X), … }$

. . .

*   abstraction and L2S encoding are done together -- proving absence of
    *abstract fair loop*

## Liveness-to-Safety with Implicit Abstraction II

**Original L2S:**

\includegraphics[page=5, clip, trim=6cm 12.3cm 5.1cm 14.5cm, width=\textwidth]{paper}

**Abstract L2S ($\alpha$L2S):**

\includegraphics[page=8, clip, trim=5cm 19.0cm 4.5cm 7.7cm, width=\textwidth]{paper}

*   $c_{p_i}$ describes state (described by predicates) where abstract lasso starts

## Counterexamples and Refinement

*   if $\lnot \textit{bad}_{\alpha L2S}$ holds we are done, system is safe
*   otherwise, counterexample has to be validated
    *   violation of property implies lasso shaped CE in *abstract* state space
    *   search for concrete lasso by bounded model checking
    *   if it fails, try to prove infeasibility of this lasso
        *   for increasing but finite number of unrollings of the lasso
        *   when found infeasibility, derive new predicates from interpolants of
            unrollings

## Bibliography {.allowframebreaks}

\bibliographystyle{plain}
\bibliography{pres}

