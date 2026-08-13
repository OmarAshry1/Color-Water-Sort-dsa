#pragma once
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

using namespace std; 
struct Move {
    int from{};
    int to{};
    int amount{};
    int color{};
};

using Tube = vector<int>; // bottom -> top, no zeros stored
using State = vector<Tube>;

struct SolveResult {
    bool solved{false};
    vector<Move> moves;
    State finalState;
    size_t visitedStates{0};
    string message;
};

class WaterSortSolver {
public:
    WaterSortSolver(int capacity, State initial);
    SolveResult solveMinimumMoves();

    static bool isSolved(const State& state, int capacity);
    static bool canPour(const State& state, int capacity, int from, int to);
    static Move makePour(State& state, int capacity, int from, int to);
    static string encode(const State& state);

private:
    int capacity_;
    State initial_;
};
