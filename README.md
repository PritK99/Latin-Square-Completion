# Latin-Square-Completion using Parallel Maximum Independent Set Algorithm

<img src = "assets/img/latin_sq.png" alt= "latin square problem">

# Table of Contents

- [Project](#latin-square-completion-using-parallel-maximum-independent-set-algorithm)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Approach](#approach)
  - [File Structure](#file-structure)
  - [Results](#results)
  - [Getting Started](#getting-started)
  - [Output](#output)
  - [References](#references)
  - [License](#license)


## Introduction

## Latin Sqaure Completion

A **Latin Square** of order `n` is an `n x n` array filled with `n` distinct symbols, typically represented by the set `{1, 2, ..., n}`, such that each symbol appears exactly once in each row and each column. Formally, an `n x n` array `L = [l_ij]` is a Latin square if for all `i, j, k` in `{1, 2, ..., n}`:

- `l[i][j] != l[k][j]` for `i != k` (distinct symbols in every column),
- `l[i][j] != l[i][k]` for `j != k` (distinct symbols in every row).

**Latin Square Completion (LSC)** refers to the process of filling in an incomplete Latin square, where some entries are missing, such that the filled entries satisfy the Latin square properties. Specifically, given a partially filled `n x n` array `L = [l_ij]` with some missing entries, the goal is to fill in the missing entries with symbols from the set `{1, 2, ..., n}` such that the resulting array is a Latin square.

Latin Square Completion is an NP Complete problem and can be framed as Constraint Satisfaction Problem. This is because it can be solved only in non-deterministic polynomial time, but easy to verify in linear time `(O(n^2))` time complexity, where `n` is the dimension of problem. 

It can be defined as a Constraint Satisfaction Problem (CSP) as follows: 

```Variables```: Each cell in NxN matrix

```Domains```: {1, 2, . . . , n}

```Constraints```: Each symbol (1-n) can occur exactly once in a row and column

In this paper, we aim to explore MIS Graph Coloring Algorithm for solving a latin square.

### Maximum Independent Set Algorithm

<i>PENDING</i>

## Approach

### Pre-Processing

1) Transforming the problem as a Graph Colouring problem

<img src = "assets/img/graph-color.jpg" alt= "latin square problem">

Here, the nodes represt the cells, and the edges represent the constraints. The color of the node represents the value of the symbol in the cell.

2) Reduction of Graph

<img src = "assets/img/reduction.jpg" alt= "latin square problem">

We reduce the search space by filling those cells for which the domain is not greater than 1. This allows us to significantly reduce the search space.

### Graph Coloring

<i>PENDING</i>

## File Structure
```
👨‍💻Latin-Square-Completion
 ┣ 📂assets                            // Contains all the reference gifs, images
 ┃ ┣ 📂img
 ┃ ┣ 📂scripts
 ┣ 📂include                           
 ┃ ┣ 📄lsc.hpp                         // Header file for LSC
 ┃ ┣ 📄plits.hpp                       // Header file for PLITS 
 ┣ 📂src                          
 ┃ ┣ 📄bfs.cpp                         // Breadth-First Search
 ┃ ┣ 📄dfs.cpp                         // Depth-First Search 
 ┃ ┣ 📄bestfs.cpp                      // Best-First Search
 ┃ ┣ 📄plits_seq.cpp                   // Partial Legal and Illegal Tabu Search (PLITS), Sequential
 ┃ ┣ 📄plits_par.cpp                   // PLITS, Parallelised using OpenMP
 ┃ ┣ 📄mis_par.cpp                     // Proposed Approach, MIS Parallelised using OpenMP
 ┣ 📄Makefile       
 ┣ 📄README.md   
``` 

## Results

<img src="assets/img/results.png" alt="results">

## Getting Started

### Prerequisites
To download and use this code, the minimum requirements are:

* make: A build automation tool.
* g++: The GNU C++ compiler.
* Operating System: Windows 7 or later (64-bit) or Ubuntu 20.04 or later.

### Installation

Clone the project by typing the following command in your Terminal/CommandPrompt

```
git clone https://github.com/PritK99/Latin-Square-Completion
```
Navigate to the Latin-Square-Completion folder

```
cd Latin-Square-Completion
```

### Usage

Once you have installed the prerequisites and cloned the project, you can start using it. Follow these commands:

To compile all targets / algorithms, run

```sh
make all
```

To list all targets, run

```sh
$ make list
Targets (Algorithms): 
- bfs (Breadth First Search)
- dfs (Depth First Search)
- bestfs (Best First Search)
- plits_seq (Partial Legal & Illegal Tabu Search)
- plits_par (Parallelised Partial Legal & Illegal Tabu Search)
- miss_par (Parallelised Partial Legal & Illegal Tabu Search)
```

To compile only a specific target / algorithm (from BFS, DFS, BestFS or PLITS {sequential & parallel}), run (e.g. for bestfs)

```sh
$ make bestfs
```

To execute any program, run (e.g. for bfs)

```sh
$ ./bfs
```

<b>Note</b>: When running any parallel algorithm, you can specify the number of threads as an argument. For example:

```sh
$ ./plits_par 4
```

To remove any executables, run 

```sh
$ make clean
```

## Output

<i>PENDING</i>

### 1. BFS implementation

https://github.com/PritK99/Latin-Square-Completion/assets/103848930/e11bd36a-c30c-4a15-8a34-94802e0505b8

### 2. DFS implementation

https://github.com/PritK99/Latin-Square-Completion/assets/103848930/ffdf1d8a-355e-49a3-a615-7b98704570bb

### 3. Best First Search Implementation

https://github.com/PritK99/Latin-Square-Completion/assets/103848930/e7c7b531-2bf7-48bd-8bfc-bc58b2d0e795

### 4. Reduction and Smart Solving Techniques

https://github.com/PritK99/Latin-Square-Completion/assets/103848930/6131050e-931a-4c0d-aca0-84f5be7df869

### 5. 4x4 LSC using Heuristics and Reductions

https://github.com/PritK99/Latin-Square-Completion/assets/103848930/01a084e4-61a1-4614-b01a-fa5425ecb1bf

## References

<i>PENDING</i>

A Fast Local Search Algorithm for the Latin Square Completion Problem (AAAI 22) by Shiwei Pan, Yiyuan Wang, Minghao Yin

## License
[MIT License](https://opensource.org/licenses/MIT)