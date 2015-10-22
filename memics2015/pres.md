---
title: Weak Memory Models as LLVM-to-LLVM Transformations
author:
    - \textbf{Vladimír Štill}
    - Petr Ročkai
    - Jiří Barnat
authors-short: Vladimír Štill…
header-includes:
    - \usepackage{divine}
lang: english
date: 24th October 2015
...

## Introduction

\divine{} does

*   verification of C & C++ parallel programs

*   using LLVM bitcode

*   parallelism using `pthreads` or C++ standard threads

*   supports large portions of C & C++ standard library

*   verification of assertion/memory safety, LTL specification

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

\begin{latex}
    explores all relevant outcomes of program\only<2->{:}
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
\end{latex}

## Weak memory models {.fragile}

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

TSO lze simulovat pomocí store bufferů:

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

## Weak memory models as transformation

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
    \only<1>{model checking programs with \divine}
    \only<2>{input program does not specify memory model exactly}
    \only<3>{LLVM roughly copies C++11 memory model}
    \only<4>{DIVINE assumes sequential consistency}
    \only<5>{LART instruments LLVM bitcode with relaxed memory model}
    \only<6>{model is now verified with given relaxed memory model}
\end{latex}

## LART LLVM-to-LLVM transformations


