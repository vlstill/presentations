---
title: Weak Memory Models as LLVM-to-LLVM Transformations
titleshort: Weak Memory Models
author:
    - \textbf{Vladimír Štill}
    - Petr Ročkai
    - Jiří Barnat
authorshort: Vladimír Štill et al.
header-includes:
    - \usepackage{divine}
lang: english
date: 24th October 2015
...

## Introduction

\divine{} model checker:

*   verification of C & C++ parallel programs

*   using LLVM bitcode
    *   kind of an assembler produced by compilers, such as `clang`

*   parallelism using `pthreads` or C++ standard threads

*   supports large portions of C & C++ standard library

*   verification of assertion/memory safety, LTL specification

*   verification of largely unmodified codes

## Introduction

\begin{latex}
    \center
    \begin{tikzpicture}[>=stealth',shorten >=1pt,auto,node distance=4em,initial text=, ->]
        \tikzstyle{every state} = [rectangle, minimum size = 4.5em]
        \path[use as bounding box] (-1.5,0.8) rectangle (9,-5.2);

        \node[state, onslide={<2> line width = 3pt}] (c) at (0, 0) {\begin{minipage}{8em}\center
                \textbf{C/C++}\\
                with threads\\
                (.c/.cpp)
            \end{minipage}
        };
        \node[state, onslide={<3> line width = 3pt}] (llvm) at (0, -3) {\begin{minipage}{8em}\center
                \textbf{LLVM bitcode}\\
                (.bc)
            \end{minipage}
        };

        \node[state, onslide={<2> line width = 3pt}] (prop) at (6, 0) {\begin{minipage}{8em}\center
                \textbf{Property}\\
                assertion, memory safety, LTL
            \end{minipage}
        };

        \node[state, onslide={<4> line width = 3pt}] (divine) at (6, -3) {\begin{minipage}{8em}\center
                \textbf{DIVINE}\\
                model checker
            \end{minipage}
        };

        \node[state, minimum size = 1.5em, onslide={<5> line width = 3pt}]
        (ok) at (4.5, -5) {\color{paradisegreen}\textbf{OK}};
        \node[state, minimum size = 1.5em, onslide={<5> line width = 3pt}]
        (nok) at (7.5, -5) {\color{red}\textbf{Counterexample}};

        \path[onslide={<3> line width = 3pt}] (c) edge (llvm);
        \node at (0.1,-1.5) [anchor=west] {\begin{minipage}{8em}
                clang\\+ DIVINE libs
            \end{minipage}};
        \path (llvm) edge (divine);
        \path (prop) edge (divine);
        \path[onslide={<5> line width = 3pt}] (divine) edge (ok) edge (nok);
    \end{tikzpicture}

    \bigskip
    \only<1>{model checking programs with \divine}
    \only<2>{programmer gives inputs: source code and specification}
    \only<3>{the program is compiled into LLVM bitcode}
    \only<4>{\divine{} explores all relevant interleavings}
    \only<5>{verification results}

\end{latex}

## Explicit-State Model Checking

\begin{minipage}[t][18em]{\textwidth}
    explores all relevant outcomes of program:
    \pause
    \begin{itemize}
        \item starts from an initial state
        \only<3->{\item looks at possible actions that can be taken in each state}
    \end{itemize}

    \bigskip
    \begin{tikzpicture}[>=stealth',shorten >=1pt,auto,node distance=3em,initial text=, ->]
        \tikzstyle{every state} = [ellipse, minimum size = 1.5em]
        \path[use as bounding box] (-5.2,0.3) rectangle (5.2,-3.2);

        \node[state] (init) {\texttt{x = 0; y = 0}};
        \node<3->[state, below left = of init] (l) {\texttt{x = 1; y = 0}};
        \node<3->[state, below right = of init] (r) {\texttt{x = 0; y = 1}};
        \node<4->[state, below right = of l] (c) {\texttt{x = 1; y = 1}};

        \path<3-> (init) edge node[above left] {\texttt{x := 1}} (l)
                         edge node {\texttt{y := 1}} (r);
        \path<4-> (l) edge node[below left] {\texttt{y := 1}} (c);
        \path<4-> (r) edge node {\texttt{x := 1}} (c);
    \end{tikzpicture}

    \onslide<5->{
    \begin{itemize}
        \item builds state space
        \only<6>{\item graph exploration}
    \end{itemize}
    }
\end{minipage}

# Demo

## Memory Models

*   the order of reads and writes in code need not match the order of their execution
    *   compiler optimizations
    *   optimizations on CPU, cache hierarchy

. . .

*   it is hard to reason about memory models
*   parallelism is hard even under **Sequential Consistency**
    *   reads and writes are immediate and cannot be reordered
    *   not realistic, expensive to enforce

. . .

*   newer revisions of C/C++ have support for specifying atomic variables with
    memory access ordering
    *   same for Java, LLVM bitcode,…

. . .

*   verifiers often assume sequential consistency
    *   so does DIVINE

## Weak Memory Models {.fragile}

What is the semantics of concurrent accesses to shared memory (in C++11)?

\begin{minipage}[t][14em]{\textwidth}
\only<1-2,5-6>{\texttt{\textbf{int} x = 0, y = 0;}}
\only<3,7>{\texttt{\textbf{volatile int} x = 0, y = 0;}}
\only<4,8>{\texttt{\textbf{std::atomic< int >} x = 0, y = 0;}}

\begin{lstlisting}
void thread1() { x = 1; y = 2; }
void thread2() {
    while ( y == 0 ) { }
    cout << "y = " << y << endl;
    cout << "x = " << x << endl;
}
\end{lstlisting}

\only<2>{
\begin{itemize}
    \item no guarantees
    \item \texttt{thread2} can read \texttt{y} only once
    \item undefined behaviour
\end{itemize}
}
\only<3>{
\begin{itemize}
\item no atomicity guarantees
\item variables must be read from memory on every access
\item undefined behaviour
\end{itemize}
}
\only<4>{
\begin{itemize}
\item atomicity guaranteed
\item variables must be atomically read from memory on every access
\end{itemize}
}

\only<5->{
\begin{itemize}
    \item can the output be \texttt{y = 2} and \texttt{x = 0}?

    \only<6>{\item yes, both CPU and compiler can reorder instuctions}
    \only<7>{\item yes, CPU can reorder instuctions}
    \only<8>{\item no, the order of atomic loads and stores is guarteed to
    match order in source code}
\end{itemize}
}
\end{minipage}

## Weak Memory Models

**many different models in different architectures**

*   details often unknown
*   details can vary between CPUs of same architecture

. . .

**theoretical memory models**

*   Total Store Order (TSO)
    *   similar to memory model used by x86_64
    *   the order of execution of stores is guaranteed to match their order in
        machine code
        *   compiler might still reorder stores
    *   independent loads can be reordered

. . .

*   Partial Store Order (PSO)
    *   weaker than TSO
    *   independent stores can be reordered too
    *   enables more optimizations
    *   harder to reason about

## Weak Memory Models {.fragile}

\begin{latex}
\begin{lstlisting}[belowskip=0pt]
int x = 0, y = 0;
\end{lstlisting}

\begin{minipage}[t]{0.45\textwidth}
\begin{lstlisting}[numbers=left,aboveskip=0pt]
void thread0() {
    y = 1;
    cout << "x = " << x;
  }
\end{lstlisting}
\end{minipage}%
\hfill%
\begin{minipage}[t]{0.45\textwidth}
\begin{lstlisting}[numbers=left,aboveskip=0pt]
void thread1() {
    x = 1;
    cout << "y = " << y;
}
\end{lstlisting}
\end{minipage}
\end{latex}

Total Store Order can be simulate using store buffers:

\begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                   , semithick
                   , scale=0.65
                   ]
  \draw [-] (-10,0) -- (-6,0) -- (-6,-2) -- (-10,-2) -- (-10,0);
  \draw [-] (-10,-1) -- (-6,-1);
  \draw [-] (-8,0) -- (-8,-2);
  \node () [anchor=west] at (-10,0.5) {main memory};
  \node () [anchor=west] at (-10,-0.5)  {\texttt{0x04}};
  \node () [anchor=west] at (-8,-0.5)  {\texttt{0x08}};
  \node () [anchor=west] at (-10,-1.5)  {\texttt{x = 0}};
  \node () [anchor=west] at (-8,-1.5)  {\texttt{y = 0}};

  \node () [anchor=west] at (-10,-3.5) {store buffer for thread 0};
  \node () [anchor=west] at (0,-3.5) {store buffer for thread 1};

  \draw [-] (-10,-4) -- (-4, -4) -- (-4,-5) -- (-10,-5) -- (-10,-4);
  \draw [-] (0,-4) -- (6, -4) -- (6,-5) -- (0,-5) -- (0,-4);
  \draw [-] (-8,-4) -- (-8,-5);
  \draw [-] (-6,-4) -- (-6,-5);
  \draw [-] (2,-4) -- (2,-5);
  \draw [-] (4,-4) -- (4,-5);

  \node<2-> () [anchor=west] at (-10,-4.5)  {\texttt{0x08}};
  \node<2-> () [anchor=west] at (-8,-4.5)  {\texttt{1}};
  \node<2-> () [anchor=west] at (-6,-4.5)  {\texttt{32}};

  \node<4-> () [anchor=west] at (0,-4.5)  {\texttt{0x04}};
  \node<4-> () [anchor=west] at (2,-4.5)  {\texttt{1}};
  \node<4-> () [anchor=west] at (4,-4.5)  {\texttt{32}};

  \node () [] at (-4, 0.5) {thread 0};
  \draw [->] (-4,0) -- (-4,-2);
  \node () [anchor=west, onslide={<2> font=\bf, color=red}] at (-3.5, -0.5) {\texttt{store y 1;}};
  \node () [anchor=west, onslide={<3> font=\bf, color=red}] at (-3.5, -1.5) {\texttt{load x;}};

  \node () [] at (2, 0.5) {thread 1};
  \draw [->] (2,0) -- (2,-2);
  \node () [anchor=west, onslide={<4> font=\bf, color=red}] at (2.5, -0.5) {\texttt{store x 1;}};
  \node () [anchor=west, onslide={<5> font=\bf, color=red}] at (2.5, -1.5) {\texttt{load y;}};

  \draw<2-> [->, dashed] (-0.5,-0.5) to[in=0, out=0] (-4,-4.5);
  \draw<3-> [->, dashed] (-9,-2) to[in=0, out=-90, out looseness=0.7] (-1.3,-1.5);
  \draw<4-> [->, dashed] (5.5,-0.5) to[in=0, out=0] (6,-4.5);
  \draw<5-> [->, dashed] (-7,-2) to[in=0, out=-90, out looseness=0.5] (4.7,-1.5);

\end{tikzpicture}

## Weak Memory Models as Transformation

\begin{latex}
    \center
    \begin{tikzpicture}[>=stealth',shorten >=1pt,auto,node distance=4em,initial text=, ->]
        \tikzstyle{every state} = [rectangle, minimum size = 4.5em]
        \path[use as bounding box] (-1.5,0.8) rectangle (9.5,-5.2);

        \node[state, onslide={<2> line width = 3pt}] (c) at (0, 0) {\begin{minipage}{8em}\center
                \textbf{C/C++}\\
                with threads\\
                (.c/.cpp)
            \end{minipage}
        };
        \node[state, onslide={<3> line width = 3pt}] (llvm) at (0, -3) {\begin{minipage}{8em}\center
                \textbf{LLVM bitcode}\\
                (.bc)
            \end{minipage}
        };

        \node[state] (prop) at (7.5, 0) {\begin{minipage}{8em}\center
                \textbf{Property}\\
                assertion, memory safety, LTL
            \end{minipage}
        };

        \node[state, onslide={<4,6> line width = 3pt}] (divine) at (7.5, -3) {\begin{minipage}{8em}\center
                \textbf{DIVINE}\\
                model checker
            \end{minipage}
        };

        \node<5->[state, onslide={<5> line width = 3pt}] (lart) at (3.75, -3) {\begin{minipage}{8em}\center
              \textbf{LART}

              LLVM-to-LLVM transormation
            \end{minipage}
        };

        \node[state, minimum size = 1.5em]
        (ok) at (4.5, -5) {\color{paradisegreen}\textbf{OK}};
        \node[state, minimum size = 1.5em]
        (nok) at (7.5, -5) {\color{red}\textbf{Counterexample}};

        \path (c) edge (llvm);
        \node at (0.1,-1.5) [anchor=west] {\begin{minipage}{8em}
                clang\\+ DIVINE libs
            \end{minipage}};
        \path<1-4> (llvm) edge (divine);
        \path<5-> (llvm) edge (lart);
        \path<5-> (lart) edge (divine);
        \path (prop) edge (divine);
        \path (divine) edge (ok) edge (nok);
    \end{tikzpicture}

    \bigskip
    \makebox[\textwidth][c]{
    \only<1>{model checking programs with \divine}
    \only<2>{input program does not specify memory model exactly}
    \only<3>{LLVM roughly copies C++11 memory model}
    \only<4>{DIVINE assumes sequential consistency}
    \only<5>{LART instruments LLVM bitcode with relaxed memory model}
    \only<6>{model is now verified with given relaxed memory model}
    }
\end{latex}

## TSO Under-Approximation

*   TSO can be simulated with unbounded store buffer
*   this can easily make state space infinite

. . .

*   can be under-approximated with bounded store buffer
    *   if a bug is found, it can occur on TSO (or more relaxed) hardware
    *   if bug is not found, there is no guarantee

    . . .

    *   integrates well with explicit state model checker like DIVINE

## Why LLVM-to-LLVM Transformation?

*   memory model support could be integrated in verifier
    *   complicates verifier
    *   impractical if more memory models are to be supported

. . .

*   can be implemented in program-to-be-verified
    *   manual implementation is tedious
    *   automatic transformation is hard to do (especially for C++)

. . .

*   transformation of LLVM
    *   LLVM is assembly-like language
    *   abstracts away from machine registers, caches…
    *   significantly simpler than high-level programming languages
    *   API for transformations

    . . .

    *   ecosystem of code generators for many languages
    *   analysis tools other than DIVINE can use this transformation

## The Transformation

memory manipulating instructions need to be replaced to enable TSO simulation

*   add store buffer for each thread
*   `store` instruction saved data to store buffer
*   `load` instruction first looks up data in local store buffer, then in memory
*   `fence` (memory barrier) instructions flushes store buffer
*   atomic instructions (`atomicrmw`, `cmpxchg`) flush store buffer and perform
    given action

. . .

*   memory manipulations functions (`memcpy`, `memmove`, `memset`) needs to be
    replaced
*   partial loads/stores need to be handled

## Store Buffer Flushing

store buffer needs to be flushed nondeterministically

. . .

*   dedicated flushing thread
*   interleaving ensures all possible outcomes are checked

. . .

*   works well for safety properties
    *   every counterexample is finite

. . .

*   for LTL infinitely delayed flush is a problem

## Infinite Delay Problem {.fragile}

$\varphi = FG( \lnot w_0 \land \lnot w_1 )$

\begin{latex}
\begin{lstlisting}[belowskip=0pt]
bool x = false, y = false;
\end{lstlisting}
\begin{minipage}[t]{0.45\textwidth}
\begin{lstlisting}[numbers=left, aboveskip=0pt]
void thread0() {
  y = true;
  while (!x) { AP(w0); }
  for (;;) { /*work*/ }
}
\end{lstlisting}
\end{minipage}%
\hfill%
\begin{minipage}[t]{0.45\textwidth}
\begin{lstlisting}[numbers=left, aboveskip=0pt]
void thread1() {
  x = true;
  while (!y) { AP(w1); }
  for (;;) { /*work*/ }
}
\end{lstlisting}
\end{minipage}
\end{latex}

*   $\varphi$ does no hold on a run where flush is delayed infinitely

. . .

**Solution**

*   for each program thread, we add dedicated flushing thread which flushes
    its store buffer
*   weak fairness used to avoid infinite flush delay

## Writes To Invalidated Memory

store buffer can be flushed when the memory it writes to is already invalid

*   write to freed dynamic memory
*   write to stack frame of function after it returns

. . .

*   for the first case, we can replace `free` such that it evicts freed
    memory addresses from store buffers

. . . 

*   for stack memory, cleanup needs to be added at the end of every function
    which uses stack memory
    *   exception handlers need to be added

. . .

*   currently not implemented
    *   DIVINE cannot verify memory safety with TSO currently

# Demo

## State Space Reductions

*   DIVINE employs $\tau+$ and heap symmetry reductions
    *   $\tau+$ hides actions which are not observable, such as loads and
        stores to thread local memory
    *   this enables verification of real-world code

. . .

*   with TSO simulation every load and store is visible
    *   this makes reduction inefficient

. . .

*   thread local memory need not be stored in store buffer
    *   this information can be obtained from DIVINE and store buffer
        bypassed

## Experimental Results

\begin{latex}
\makebox[\textwidth][c]{
\small
\begin{tabular}{|c|c|c|c|c|c|c|} \hline
    model & buffer & assertion  & \# of  & reduced & memory & time \\
          & size   & found & states  & \# of states  & [GB]   & [s] \\ \hline
    \texttt{simple\_sc} & N/A & no & 205 & N/A & 0.16 & 1 \\
    \texttt{simple\_mtso} & 1 & \textbf{yes} & 6.89\,k & N/A & 0.17 & 3 \\
    \texttt{simple\_stso} & 1 & \textbf{yes} & 10.7\,k & 10.7\,k & 0.17 & 6 \\
    \texttt{simple\_tso} & 1 & \textbf{yes} & 24.7\,M & 537.2\,k & 3.18 & 20318 \\ \hline
    \texttt{peterson\_sc} & N/A & no & 1.68\,k & N/A & 0.16 & 1 \\
    \texttt{peterson\_tso} & 0 & no & 55.9\,k & N/A & 0.17 & 38 \\
    \texttt{peterson\_tso} & 2 & \textbf{yes} & 2.86\,M & 95.7\,k & 0.79 & 990 \\
    \texttt{peterson\_tso} & 3 & \textbf{yes} & 4.70\,M & 129.9\,k & 1.21 & 1610 \\ \hline
    \texttt{fifo\_sc} & 0  & no & 6951 & N/A & 0.73 & 20 \\
    \texttt{fifo\_tso} & 1 & no & -- & 44\,M & -- & -- \\ \hline
\end{tabular}
}
\end{latex}

## Conclusion

*   fully automatic instrumentation of LLVM bitcode with Total Store Order
    approximation
*   enables TSO verification in DIVINE, or other verifiers assuming Sequential
    Consistency
*   verification of assertion safety and LTL properties
*   we were able to verify some interesting properties with this
    transformation, despite the state space growth

. . .

**Future Work**

*   enable memory safety verification with TSO
*   implementation of other weak memory models, such as Partial Store Order
*   state space reduction

. . .

\hfill{}Thank You!

