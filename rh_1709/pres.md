---
vim: spell spelllang=en tw=80 fo+=t
title: "DIVINE: Analysis of C++ Programs"
author:
    - Vladimír Štill
header-includes:
    - \usepackage{divine}
    - \newcommand{\TODO}[1]{{\color{red}#1}}
    - \newcommand{\redon}[2]{{\only<#1>{\color{red}}#2}}
    - \setbeamersize{text margin left=1.2em, text margin right=1.2em}
    - \setbeamercovered{invisible}
    - \setbeamercovered{again covered={\opaqueness<1->{15}}}
    - \newenvironment{wrap}[1]{#1}{}
    - \newcommand{\gray}{}%\setbeamercolor{item}{fg=gray}\setbeamercolor{normal text}{fg=gray}}
lang: english
date: 22th September 2017
...

## DIVINE: Analysis of C++ Programs

*   DIVINE is an analyser for C and C++ programs

*   focus on language features

*   built on top of the LLVM infrastructure

*   tightly integrated compiler

## Current Status: Publications

waiting in journal review process:

*   Petr Ročkai, Vladimír Štill, Ivana Černá, Jiří Barnat:\
    **DiVM: Model Checking with LLVM and Graph Memory**

. . .

freshly published:

*   Vladimír Štill, Petr Ročkai, Jiří Barnat:\
    **Using Off-the-Shelf Exception Support Components in C++ Verification**

    . . .

*   **Model Checking of C and C++ with DIVINE 4**

    . . .

*   Katarína Kejstová, Petr Ročkai, Jiří Barnat:\
    **From Model Checking to Runtime Verification and Back**

. . .

reworking:

*   Petr Ročkai, Jiří Barnat: **A Simulator for LLVM Bitcode**

## Current Status: New Features

DIVINE Operating System (DiOS)

*   modularized for more flexibility and configurability
*   support for processes (`fork`)

. . .

Symbolic Data

*   symbolization of data structures
*   multiple data domains in the same program
*   refactoring and cleaning

. . .

Relaxed Memory Models

*   resurrected and refactored implementation
*   work on efficiency

. . .

Simulator

*   multiple usability improvements

. . .

Core

*   improved state space reduction

## Plans: Runtime Verification

-   TODO

## Relaxed Memory Models: Motivation

What is relaxed memory model?

*   describes how memory operations propagate between processors

    . . .

*   effects of cache, out-of-order execution

    . . .

*   depends on architecture

    . . .

    *   Intel `x86`: store buffering

    *   ARM/POWER: almost arbitrary reordering of loads and stores

    . . .

*   significantly increases theoretical complexity of verification

## Relaxed Memory Models

*   rest of my PhD work

*   thesis proposal submitted

    . . .

*   focus on mitigation of state space explosion

    . . .

    *   a lot of equivalent interleavings with conventional methods
    *   opportunity for static analysis, heuristics

    . . .

*   plans to support multiple memory models

    . . .

*   completeness of analysis

## Symbolic and Abstract Verification: Motivation

*   program's behaviour can depend on input values

    . . .

*   need for verification under arbitrary inputs

    . . .

*   cannot be handled explicitly: too many combinations

## Symbolic and Abstract Verification

main idea: transform the program to encode symbolic/abstracted data

*   Henrich's master's thesis, later PhD

    . . .

*   framework for using different abstractions

    . . .

*   master's thesis in December

    *   finalizing the initial implementation

    *   support for functions, data structures

    *   but no dynamic memory and aliased pointers

    *   testing and benchmarking

    . . .

*   planned PhD work

    *   full abstraction of programs with memory

    *   symbolic memory

    *   …
