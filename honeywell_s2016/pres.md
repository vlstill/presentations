---
title: DIVINE
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
lang: english
date: 11th March 2015
...

## DIVINE

*   explicit-state model checker for verification of parallel programs
    *   main focus on C++/\llvm
    *   also many other inputs: DVE, CESMI, timed automata
*   verifies safety and LTL properties
    *   specification depends on the input formalism
*   parallel (and distributed) verification
*   reduction strategies
*   <https://divine.fi.muni.cz>

## DIVINE: Input Formalisms

LLVM

\vspace{-\medskipamount}

*   for verification of C and C++
*   more later

DVE

\vspace{-\medskipamount}

*   simple input formalism for communicating processes
*   each process has finitely many states
*   channels, guards for communication and synchronization

CESMI

\vspace{-\medskipamount}

*   user-implemented, compiled models, using C API

Timed automata

\vspace{-\medskipamount}

*   using UPPALL formalism

## DIVINE: State Space Output

DIVINE can either run a verification algorithm over the state space, *or
calculate and save the state space*

\bigskip

**DESS (DIVINE Explicit State Space)**

*   binary format of explicit state space
*   DIVINE can materialize any state space into DESS
*   state flags (assertion violation, atomic propositions) can be read from
    DESS


## DIVINE: LLVM as an Input Language

*   used for verification of C and C++ programs
*   since \divine{} 3
*   C and C++ library, C++ exceptions support (since 3.2)
*   threads through `pthreads` or C++11
*   state space size reductions
*   safety properties: assertion safety, memory safety, uninitialized variables
    tracking, `pthreads` deadlock detection
*   rudimentary LTL support

## DIVINE: Input Formalisms

\framesubtitle{LTL in LLVM}

*   explicitly activated atomic propositions
*   hold only in the state they were raised
    *   two APs cannot hold at the same time
*   better support is planned in DIVINE 4

```{.cpp}
#include <divine.h>
enum APs { c1in, c1out, c2in, c2out };
LTL(exclusion,
    G( (c1in -> (!c2in W c1out))
      && (c2in -> (!c1in W c2out))) );

void critical1() { AP( c1in ); AP( c1out ); }
void critical2() { AP( c2in ); AP( c2out ); }
```

## DIVINE Verification Workflow

\begin{latex}
    \medskip
    \footnotesize
    \makebox[\textwidth][c]{
    \begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                       , semithick
                       , scale=0.7
                       , state/.style={ rectangle, draw=black, very thick,
                         minimum height=2em, minimum width = 4em, inner
                         sep=2pt, text centered, node distance = 2em }
                       ]
      \node[state, minimum width = 5em] (lib) {Libraries};

      \node[state, below = of lib.south west, anchor = north west] (cpp) {C++};
      \node[state, right = 4em of cpp, rounded corners] (clang) {Clang};
      \node[state, right = 4em of clang] (llvm) {LLVM IR};
      \node[state, right = 4em of llvm, rounded corners] (divine) {\divine};

      \node[state, above = of divine.north east, anchor = south east, minimum width = 15em] (ltl) {Verified property: safety, LTL};
      \node[state, below = of divine] (valid) {\color{paradisegreen!70!black}OK};
      \node[state, left = of valid, minimum width = 8em] (ce) {\color{red!70!black}Counterexample};

      \path (ltl.south) edge[out=270, in=90] (divine.north)
            (cpp) edge (clang)
            (clang) edge (llvm)
            (lib) edge [out=0, in=90, looseness = 1] (clang)
            (llvm) edge (divine)
            (divine.south) edge (valid) edge[out=270, in=90] (ce.north)
            ;
    \end{tikzpicture}
    }
\end{latex}

```{.bash}
divine compile --lib # needed only once
divine compile --pre=. test.cpp --cflags="-std=c++11"
divine info test.bc # list properties
divine verify test.bc --compress --display-counterex
```

## C++/LLVM as a Modelling Language

LLVM interpreter in DIVINE supports several intrinsic functions (as of DIVINE 3.3.2)

*   `__divine_choice( int n )` splits state space into `n` copies, in each
    returns a number from $[0, n)$
*   `__divine_interrupt_mask()` starts an atomic section
    *   `__divine_interrupt` should be called before it
    *   atomic sections ends when the function which called
        `__divine_interrupt_mask` returns
    *   everything from the call until the atomic section ends is
        uninterruptible (one edge in the state space)
*   `__divine_interrupt_unmask` immediately ends the atomic section
    *   should be called only in functions which called
        `__divine_interrupt_mask`
*   `__divine_problem` report a problem to the interpreter

## C++/LLVM as a Modelling Language

```{.c}
void *malloc( size_t size ) {
    if ( __divine_choice( 2 ) )
        return __divine_malloc( size );
    return NULL;
}
```

## C++/LLVM as a Modelling Language

\footnotesize

```{.cpp}
#include <divine/problem.h>
#include <divine.h>
struct Mutex {
    void unlock() {
        __divine_interrupt(); __divine_interrupt_mask();
        if ( _locktid == 0 )
            __divine_problem( Other, "mutex not locked" );
        _locktid = 0;
    }
    void lock() {
        __divine_interrupt(); __divine_interrupt_mask();
        while ( _locktid ) {
            if ( _locktid == __divine_get_tid() + 1 )
                __divine_problem( Other, "mutex re-locked" );
            __divine_interrupt_unmask(); // allow other threads to run
            __divine_interrupt_mask();
        }
        _locktid = __divine_get_tid() + 1;
    }
    private: int _locktid;
};
```

# Demo I

## LLVM Transformations

*   LLVM IR can be easily transformed before the verification
*   can be used to extend model checker's abilities, reduce state space

### Weak Memory Model Verification

*   more realistic memory access
    *   in CPUs, write from one thread need not be visible by other threads
        immediately
*   
    ```{.bash}
    divine compile --pre=. test.cpp
    lart test.bc test-tso.bc weakmem:tso
    divine verify test-tso.bc
    ``` 
*   in development
*   <https://divine.fi.muni.cz/2016/weakmem/>

# Demo II: Memory Models

\iffalse

* sumarizace verzi DIVINE, a jejich schopnosti
- vstupy CESMI, DVE, LLVM, ..., vystupy DESS
* zakladni workflow pro testovani paralelnich C/C++ programu
* pouziti LLVM jako modelovaciho jazyka
    (divine choice, maskovani preruseni, makro-kroky)
* atomicke propozice a LTL
* LLVM-LLVM transformace (priklad relaxovana pamet)
* ukazka 

\fi
