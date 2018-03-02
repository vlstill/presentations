---
vim: spell spelllang=en fo+=t tw=80
title: "Effective Stateless Model Checking for C/C++ Concurrency"
author:
    - Michalis Kokologiannakis
    - Ori Lahav
    - Konstantinos Sagonas
    - Viktor Vafeiadis
    - \newline
    - presented by Vladimír Štill
header-includes:
    - \input{defs}
    - \institute{}
lang: english
date: 5th March 2018
...

## Concurrency in C and C++

### Pre C11/C++11

- no support for concurrency in the language and standard library
- no standard-defined behaviour for parallel programs

### C11/C++11

- standard defines behaviour of parallel programs
- standard library defines:
  - thread manipulation: threads, futures
  - synchronization: mutexes, condition variables, **atomics**

## C11 Variable Access Modes in Parallel Programs

- 
