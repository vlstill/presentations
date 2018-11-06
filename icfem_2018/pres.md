---
vim: spell spelllang=en tw=80 fo+=t
title: "Model Checking of C++ Programs under\\newline the\ \\xtso Memory Model"
author:
    - Vladimír Štill
    - Jiří Barnat
header-includes:
    - \input{defs}
lang: english
date: 13th November 2018
...

## Relaxed Memory {.fragile}

- caches and inter-core communication in modern CPUs

* out-of-order execution

- has observable effects in multithreaded environments

- can cause nasty and hard to detect bugs

. . .

- relaxed memory will probably be here to haunt us for a long time
  \begin{center}
      \includegraphics[width=8em]{spectre}
  \end{center}

## Analysis of Parallel Programs under Relaxed Memory

- designing fast correct parallel programs is hard
- formal analysis can help

. . .

\bigskip
**Our Focus**

- \xtso memory model of the Intel and AMD CPUs (\xtso)
- C and C++

## The \xtso Memory Model

\begin{latex}
\begin{minipage}[t]{0.25\textwidth}
\tt
\textcolor{gray}{int} x = 0; \\
\textcolor{gray}{int} y = 0;
\par\smallskip
\textcolor{gray}{void} thread0() \{ \\
\indent{}y = 1; \\
\indent{}\textcolor{gray}{int} a = x; \\
\indent{}\textcolor{gray}{int} c = y; \\
\}
\par\smallskip
\textcolor{gray}{void} thread1() \{ \\
\indent{}x = 1; \\
\indent{}\textcolor{gray}{int} b = y; \\
\indent{}\textcolor{gray}{int} d = x; \\
\}
\end{minipage}
%
\hfill
%
\begin{minipage}[t]{0.73\textwidth}
\begin{itemize}
    \item every write to the memory can be delayed
    \item writes are not reordered with each other but a CPU will not see writes done by
      other CPUs instantly
\end{itemize}\pause
\begin{center}
\smallskip\noindent
Is $a = 0 \land b = 0$ reachable?\\[2.5ex]
\end{center}
\begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                   , semithick
                   , scale=0.5
                   ]

  \draw [-] (-10,0) rectangle (-7,-5);
  \draw [-] (-10,-1) -- (-7,-1)
            (-10,-2) -- (-7,-2)
            (-10,-3) -- (-7,-3)
            (-10,-4) -- (-7,-4);
  \draw [-] (-9,0) -- (-9,-5);
  \node () [] at (-8.5,0.5) {shared memory};
  \node () [anchor=west] at (-10,-2.5)  {\texttt{\color{blue}x}};
  \node () [anchor=west] at (-9,-2.5) {\texttt{\color{blue}0}};

  \node () [anchor=west] at (-10,-3.5)  {\texttt{\color{blue}y}};
  \node () [anchor=west] at (-9,-3.5)  {\texttt{\color{blue}0}};

  \node () [anchor=center] at (-2.5,-3.5) {store buffer};
  \draw [-] (-4.5,-4) rectangle (-0.5,-5);
  \draw [-] (-2.5,-4) -- (-2.5,-5);

  \node () [anchor=center] at (3.5,-3.5) {store buffer};
  \draw [-] (1.5,-4) rectangle (5.5,-5);
  \draw [-] (3.5,-4) -- (3.5,-5);

  \node () [anchor=west] at (-4.5,-4.5)  {\only<3->{\texttt{\color{red}y}}};
  \node () [anchor=west] at (-2.5,-4.5)  {\only<3->{\texttt{\color{red}1}}};

  \node () [anchor=west] at (1.5,-4.5)  {\only<9->{\texttt{\color{red}x}}};
  \node () [anchor=west] at (3.5,-4.5)  {\only<9->{\texttt{\color{red}1}}};

  \node () [anchor = west, xshift = -1em] at (-4.5, 0.5) {thread 0};
  \draw [->] (-4.5,0) -- (-4.5,-3);
  \node () [anchor=west] at (-4, -0.5) {\texttt{\only<3->{\color{red}}y = 1;}};
  \node () [anchor=west] at (-4, -1.5)
      {\only<4->{\texttt{\only<5->{\color{blue}}load x; \only<5->{\textrightarrow 0}}}};
  \node () [anchor=west] at (-4, -2.5)
      {\only<6->{\texttt{\only<7->{\color{frombuf}}load y; \only<7->{\textrightarrow 1}}}};

  \node () [anchor = west, xshift = -1em] at (1.5, 0.5) {thread 1};
  \draw [->] (1.5,0) -- (1.5,-3);
  \node () [anchor=west] at (2, -0.5) {\only<8->{\texttt{\only<9->{\color{red}}x = 1;}}};
  \node () [anchor=west] at (2, -1.5)
      {\only<10->{\texttt{\only<11->{\color{blue}}load y; \only<11->{\textrightarrow 0}}}};
  \node () [anchor=west] at (2, -2.5)
      {\only<12->{\texttt{\only<13->{\color{frombuf}}load x; \only<13->{\textrightarrow 1}}}};

\end{tikzpicture}
\end{minipage}
\end{latex}

## Our Approach

- \xtso allows arbitrary delaying of stores

    . . .

- but only some possibilities are meaningful and observable

    . . .

- consider flushing the store buffer only if someone reads buffered values

    . . .

- still, store buffer can be large $\rightarrow$ many ways to flush it

    . . .

- problem: store buffer keeps order

  . . .

- solution: *delayed flushing* -- some values can behave as flushed, but their order
  respective to other buffered values can be undetermined

## Delayed Flushing {.fragile}

\begin{latex}
\begin{minipage}[t]{0.4\textwidth}

\begin{tikzpicture}[ ->, >=stealth', shorten >=1pt, auto, node distance=3cm
                   , semithick
                   , scale=0.5
                   , thck/.style = { thick, decoration={markings,mark=at position 1 with {\arrow[scale=4]{>}}}, postaction={decorate}, },
                   ]

  \draw [-] (-10,0) rectangle (-7,-4);
  \draw [-] (-10,-1) -- (-7,-1)
            (-10,-2) -- (-7,-2)
            (-10,-3) -- (-7,-3);
  \node () [anchor=center] at (-8.5, 0.5) {s.b. 1};
  \node () [anchor=center] at (-8.5,-0.5) {\only<-4>{\only<3->{\color{flushed}}\texttt{x $\leftarrow$ 1}}};
  \node () [anchor=center] at (-8.5,-1.5) {\only<-2>{\texttt{y $\leftarrow$ 1}}\only<3-4>{\color{flushed}\texttt{x $\leftarrow$ 2}}};
  \node () [anchor=center] at (-8.5,-2.5) {\only<-2>{\texttt{x $\leftarrow$ 2}}};
  \node () [anchor=center] at (-8.5,-3.5) {\only<-2>{\only<2>{\color{frombuf}}\texttt{y $\leftarrow$ 2}}};

  \draw [-] (-6,0) rectangle (-3,-4);
  \draw [-] (-6,-1) -- (-3,-1)
            (-6,-2) -- (-3,-2)
            (-6,-3) -- (-3,-3);
  \node () [anchor=center] at (-4.5, 0.5) {s.b. 2};
  \node () [anchor=center] at (-4.5,-0.5) {\only<-5>{\only<4-5>{\color{frombuf}}\texttt{x $\leftarrow$ 3}}\only<6->{\texttt{y $\leftarrow$ 3}}};
  \node () [anchor=center] at (-4.5,-1.5) {\only<-5>{\texttt{y $\leftarrow$ 3}}};

\end{tikzpicture}

\begingroup
    \tt
    \textcolor{gray}{void} thread0() \{ \\
    \indent{}\textcolor{gray}{int} a = y; \only<2->{\only<2>{\textcolor{frombuf}}{// \textrightarrow 2}} \\
    \indent{}\textcolor{gray}{int} b = x; \only<4->{\only<4-5>{\textcolor{frombuf}}{// \textrightarrow 3}} \\
    \}
\endgroup
\end{minipage}
\begin{minipage}[t]{0.55\textwidth}
    \begin{itemize}
        \only<1>{\item three threads, thread 0 executing}
        \only<2>{\item nondeterministically choose to load value from the store buffer of thread~1}
        \only<3>{\item older values for \texttt{y} in s.b. 1 removed}
        \only<3>{\item other older values in s.b. 1 marked as flushed}
        \only<4>{\item nondeterministically choose to load value from s.b.~2}
        \only<5>{\item flushed values for \texttt{x} has to be removed (overwritten)}
        \only<6>{\item \texttt{x} is propagated to the memory}
        \only<7>{\item \texttt{y $\leftarrow$ 3} stays in buffer unless \texttt{y} is read again}
    \end{itemize}
\end{minipage}
\end{latex}

## Results

- evaluation on nonsymbolic SV-COMP concurrency benchmarks
- compared with CBMC and Nidhugg

    \begin{latex}
    \vspace{2ex}
    \setlength\tabcolsep{0.5em} %
    \begin{tabular}{lrrr} \toprule
        \textcolor{gray}{(total = 54)} & CBMC & \textbf{DIVINE} & Nidhugg \\ \midrule
        finished                & 20   & 23     & 24 \\
        TSO errors              &  3   & 10     &  3 \\
        uniqly solved           &  5   &  6     &  8 \\
        \bottomrule
    \end{tabular}
    \end{latex}

## Conclusion & Future Work


**Conclusion**

- an efficient explicit-state approach to verification under the \xtso memory
  model
- works with C and C++ programs
- analysis of non-terminating programs (with finite state space)

. . .

\bigskip
**Future Work**

- combination with symbolic verification
- focus on lock-free programming

. . .


\bigskip \hfill Thank You!
