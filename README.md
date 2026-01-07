# 15-puzzle-solver

Graph-based 15 puzzle solver using Breadth-First Search (BFS) and A* search with heuristic optimization and SVG visualization.
---

## Overview

This project implements a solver for the classic **15 Puzzle** by modeling the puzzle as an implicit graph search problem.  
Each puzzle configuration is treated as a graph node, and valid tile moves form edges between states.

The solver supports:
- **Breadth-First Search (BFS)** for guaranteed shortest-path solutions
- **A\*** search using Manhattan distance as an admissible heuristic for improved performance
- **SVG animation generation** to visualize the solution path step by step

The implementation is designed to be memory-efficient by generating puzzle states on demand rather than storing the full state space.

---

## Key Features

- Implicit graph representation (no full graph storage)
- BFS and A* shortest-path solvers
- Manhattan distance heuristic
- SVG-based solution visualization
- Clean, modular C++ design
- Built and tested using CMake

---

## PuzzleState Representation

The `PuzzleState` class represents a single configuration of the 15 Puzzle.

Implemented functionality includes:
- Construction from an array representation
- Conversion back to array form via `asArray()`
- Neighbor generation through valid tile moves
- `operator<` for use in ordered containers (e.g., `std::map`)
- Manhattan distance computation for heuristic search

This design allows efficient exploration of the state space without storing all possible configurations (16!).

---

## Search Algorithms

### Breadth-First Search (BFS)

- Implemented in `solveBFS`
- Explores the state space level by level
- Guarantees the shortest solution path
- Serves as a baseline uninformed search method

### A* Search

- Implemented in `solveAstar`
- Uses Manhattan distance as a heuristic function
- Significantly reduces the number of explored states compared to BFS
- Guarantees optimal solutions due to heuristic admissibility

---

## SVG Animation

The `PuzzleAnimation` class visualizes the solution path returned by BFS or A*.

Features include:
- Automatic detection of tile movement direction
- Support for numeric tiles or image-based tiles
- Configurable animation size and step duration
- Output as a standard `.svg` file viewable in modern browsers or VS Code

Each animation frame corresponds to one valid puzzle move.

---

## Build Instructions

This project uses **CMake**.

```bash
mkdir build
cd build
cmake ..
make

A detailed explanation of the 15 Puzzle is available on Wikipedia: https://en.wikipedia.org/wiki/15_puzzle
