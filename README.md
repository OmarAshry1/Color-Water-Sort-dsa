# Colored Water Sort Solver

A C++17 console program that solves the Colored Water Sort puzzle using **breadth-first search (BFS)**. Because BFS explores states by increasing path length, the first solution found has the minimum number of moves.

## Move rule used

One move transfers exactly one top layer from a source tube to a destination tube. The destination must be empty or have the same color on top, and it must not be full. This interpretation matches the supplied examples.

## Build

### CMake (recommended)

```bash
cmake -S . -B build
cmake --build build
```

Run:

```bash
./build/water_sort < sample_input_A.txt
```

On Windows, the executable may be under `build/Debug/water_sort.exe` or `build/Release/water_sort.exe`.

### Direct g++ command

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/WaterSortSolver.cpp -o water_sort
./water_sort < sample_input_A.txt
```

## Input format

1. Number of tubes `N`
2. Capacity `C`
3. `N` lines, each containing exactly `C` integers from bottom to top
4. `0` represents an empty slot and zeros must be at the end of a tube line

Example:

```text
4
2
1 2
2 1
0 0
0 0
```

## Tests

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Project structure

```text
ColoredWaterSortSolver/
├── include/WaterSortSolver.hpp
├── src/WaterSortSolver.cpp
├── src/main.cpp
├── tests/test_solver.cpp
└── README.md
```

## Complexity

Let `S` be the number of reachable puzzle states and `N` the number of tubes.

- Time: `O(S * N^2)` in the worst case, because each state checks all source-destination pairs.
- Memory: `O(S)` states for the queue and parent map, with each encoded state containing up to `N * C` layers.

