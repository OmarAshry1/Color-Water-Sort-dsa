#pragma once
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

struct Move {
    int from{};
    int to{};
    int amount{};
    int color{};
};

using Tube = std::vector<int>; // bottom -> top, no zeros stored
using State = std::vector<Tube>;

struct SolveResult {
    bool solved{false};
    std::vector<Move> moves;
    State finalState;
    std::size_t visitedStates{0};
    std::string message;
};

class WaterSortSolver {
public:
    WaterSortSolver(int capacity, State initial);
    SolveResult solveMinimumMoves();

    static bool isSolved(const State& state, int capacity);
    static bool canPour(const State& state, int capacity, int from, int to);
    static Move makePour(State& state, int capacity, int from, int to);
    static std::string encode(const State& state);

private:
    int capacity_;
    State initial_;
};
