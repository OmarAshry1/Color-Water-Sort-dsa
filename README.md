# Chromaflow - Colored Water Sort Solver

A C++17 project that solves the Colored Water Sort puzzle with a **minimum number of moves** using breadth-first search. It includes both the required console application and an optional polished SFML desktop interface.

## GUI highlights

- Animated glass tubes and vivid liquid layers
- Three ready-to-use puzzle presets
- Manual tube-to-tube editing by clicking
- Optimal BFS solving with move count and explored-state statistics
- Previous, next, autoplay and pause controls
- Keyboard shortcuts: `S` solve, `R` reset, arrow keys step, `Space` play/pause
- Responsive tube spacing for different puzzle sizes

## Build

### Console only (no external library)

```bash
cmake -S . -B build -DBUILD_GUI=OFF
cmake --build build
./build/water_sort_console < samples/example_a.txt
```

### GUI

The GUI uses SFML 2.6. CMake first looks for an installed SFML package; if unavailable, it downloads SFML automatically.

```bash
cmake -S . -B build -DBUILD_GUI=ON
cmake --build build --config Release
./build/water_sort_gui
```

On Windows with Visual Studio, run `build/Release/water_sort_gui.exe` after building.

## Input format

First line: number of tubes `N`. Second line: capacity `C`. Then `N` lines, each listing colors from bottom to top. Use `0` for empty positions; zeros must appear at the end.

## Algorithm

Each arrangement is a graph state. Every legal pour is an edge with equal cost. Breadth-first search explores this graph level by level, so the first solved state reached has the fewest moves. A hash set stores encoded states and prevents repeated exploration.

## Project structure

- `include/WaterSortSolver.hpp` - reusable solver API
- `src/WaterSortSolver.cpp` - BFS and pouring rules
- `src/main.cpp` - required console interface
- `src/gui.cpp` - SFML graphical interface
- `tests/test_solver.cpp` - automated correctness tests
- `samples/` - required sample inputs
- `docs/` - project report files
