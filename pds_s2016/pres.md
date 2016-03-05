---
title: Bounded Model Checking of Multi-threaded C Programs
subtitle: Lazy-CSeq and MU-CSeq
titleshort: \*-CSeq
author:
    - presented by Vladimír Štill
header-includes:
    - \usepackage{divine}
lang: english
date: 7th March 2015
...

## Lazy-CSeq and MU-CSeq

**bounded model checking of parallel C via *sequentialization* **

*   multi-threaded programs translated to nondeterministic sequential programs
    *   simulates all round-robin schedules
    *   bound on the number of rounds

*   also bounded in the number of cycle iterations and recursion depth
    *   cycle/function *unwinding*
    *   each thread represented by a single function without cycles

*   bounded and sequentialized program passed to a backend
    *   CBMC in this case

## Lazy-CSeq and MU-CSeq

**bounded model checking of parallel C via *sequentialization* **

*   exploit the simple structure of unwound programs
    *   no function calls
    *   no cycles $\rightarrow$ no back jumps
    *   very simple control flow (`if`, forward `goto`, thread creation/joining)
    *   every statement is executed at most once

*   the actual property violation detection left to the backend

*   sequentialization preserves safety properties *within the bounds*

# Lazy-CSeq \cite{LazyCAV14, Lazy16}

## Assumption about the Input

*   sequentially consistent memory access
*   each statement ($\sim$ line of code) is atomic
*   statically known thread function
    *   i.e. no computation of thread function pointers

*   *visible statement* -- involve read or write operation of a shared variable


## Sequentialization

1.  functions which appear in `pthread_create` and `main` are copied as *thread
    entries*
    *   every `pthread_create` corresponds to a new thread entry
    *   we will denote them $f_0$ to $f_n$ ($f_0$ = copy of `main`)

2.  loop and function calls are unwound in $f_0$ to $f_n$
    *   except for calls to `pthread_*`

3.  $f_0$ to $f_n$ are instrumented to allow partial execution and to preserve
    state between invocations
    *   all locals are turned into `static`
    *   control flow is instrumented (later)

4.  new `main` is added
    *   dispatches $f_0$ to $f_n$ repeatedly in the round-robin fashion for a
        nondeterministically guessed number of statements

## Thread Control Flow Instrumentation

\small

```{.cpp}


pthread_mutex_t m; int c = 0;
void *prod( void *b ) {
              int tmp = *((int*)b);
              pthread_mutex_lock( &m );
              if ( c > 0 )
                  c++;
              else {
                  c = 0;
                  while( tmp > 0 ) {
                      c++; tmp--;
                  }
              }
              pthread_mutex_unlock( &m );
              return 0;
}
```
\hfill

## Thread Control Flow Instrumentation

\small

```{.cpp}


pthread_mutex_t m; int c = 0;
void *prod( void *b ) {
              static int tmp = *((int*)b);
              pthread_mutex_lock( &m );
              if ( c > 0 )
                  c++;
              else {
                  c = 0;
                  while( tmp > 0 ) {
                      c++; tmp--;
                  }
              }
              pthread_mutex_unlock( &m );
              return 0;
}
```

Make variables `static`

\hfill

## Thread Control Flow Instrumentation

\small

```{.cpp}


pthread_mutex_t m; int c = 0;
void *prod( void *b ) {
              static int tmp = *((int*)b);
              pthread_mutex_lock( &m );
              if ( c > 0 )
                  c++;
              else {
                  c = 0;
                  if( !( tmp > 0 ) ) goto _l1;
                  c++; tmp--;
                  if( !( tmp > 0 ) ) goto _l1;
                  c++; tmp--;
                  assume( !( tmp > 0 ) );
              _l1: }
              pthread_mutex_unlock( &m );
```

Unwinding

## Thread Control Flow Instrumentation

\small

```{.cpp}

#define J(A, B) if (pc[t] > A || A >= cs) goto B;
pthread_mutex_t m; int c = 0;
void *prod( void *b ) {
  0: J(0,1)   static int tmp = *((int*)b);
  1: J(1,2)   pthread_mutex_lock( &m );
  2: J(2,3)   if ( c > 0 )
  3: J(3,4)       c++;
              else {
  4: J(4,5)       c = 0;
                  if( !( tmp > 0 ) ) goto _l1;
  5: J(5,6)       c++; tmp--;
                  if( !( tmp > 0 ) ) goto _l1;
  6: J(6,7)       c++; tmp--;
                  assume( !( tmp > 0 ) );
              _l1: }
  7: J(7,8)   pthread_mutex_unlock( &m );
```

Add support for jumping over statements (invisible not considered)

\hfill

## Thread Control Flow Instrumentation

\small

```{.cpp}
#define G(L) assume(cs >= G)
#define J(A, B) if (pc[t] > A || A >= cs) goto B;
pthread_mutex_t m; int c = 0;
void *prod( void *b ) {
  0: J(0,1)   static int tmp = *((int*)b);
  1: J(1,2)   pthread_mutex_lock( &m );
  2: J(2,3)   if ( c > 0 )
  3: J(3,4)       c++;
              else { G(4);
  4: J(4,5)       c = 0;
                  if( !( tmp > 0 ) ) goto _l1;
  5: J(5,6)       c++; tmp--;
                  if( !( tmp > 0 ) ) goto _l1;
  6: J(6,7)       c++; tmp--;
                  assume( !( tmp > 0 ) );
              _l1: G(7); }
  7: J(7,8)   pthread_mutex_unlock( &m );
```

Guard control flow validity.

\hfill

## Thread dispatch

In the new `main`

*   executes threads in the round-robin fashion with a fixed number of rounds
*   each time a thread executes for a nondeterministically guessed number of steps
*   executions of thread entry resumes where it left in last round thanks to
    control flow instrumentation (`J`) and saved `pc` values
*   thread executes until its PC equals `cs` (context-switch point)
*   keeps an array of active thread IDs, PCs for each thread

. . .

for K rounds:

*   for each active thread $t$
    1.  guess next context switch point (`cs`) nondeterministically
    #.  run $f_t$
    #.  set `pc[t] = cs`

## Lazy-CSeq

*   in principle very simple
*   but wins SV-COMP since 2014 (together with MU-CSeq)
*   works with many bounded model checkers
*   supports deadlock detection, counterexamples

. . .

### How to Use It

*   download at <http://users.ecs.soton.ac.uk/gp4/cseq/cseq.html>
*   requires CBMC (or BLITZ, ESBMC, LLBMC, …), Python 2, `pycparser`
    *   CBMC needs to be in `PATH`
*   `python2 ./cseq.py -i file.c`

## Does It Work?

. . .

```{.cpp}
int x;
void *thread( void *_ ) {
    ++x;
    return 0;
}
int main() {
    pthread_t t;
    pthread_create( &t, 0, thread, 0 );
    ++x;
    pthread_join( t, 0 );
    assert( x == 2 );
}
```

. . .

```
$ python2 cseq.py --rounds=10 --unwind=10 \
                  --softunwindbound -i inc.c
warning: warnings on stderr from the backend)
inc.c, SAFE, 0.89
```

## Bibliography

\bibliographystyle{plain}
\bibliography{pres}

