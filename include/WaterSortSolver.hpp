#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;

struct Move {
    int source = -1;
    int destination = -1;
};

using Tube = vector<int>;      // Bottom to top. Empty slots are omitted internally.
using State = vector<Tube>;

struct SolveResult {
    bool solved = false;
    vector<Move> moves;
    State finalState;
    size_t exploredStates = 0;
};

class WaterSortSolver {
public:
    WaterSortSolver(int capacity, State initialState);

    [[nodiscard]] SolveResult solve() const;
    [[nodiscard]] bool isGoal(const State& state) const;
    [[nodiscard]] vector<Move> legalMoves(const State& state) const;
    [[nodiscard]] State applyMove(const State& state, const Move& move) const;

    [[nodiscard]] int capacity() const noexcept { return capacity_; }
    [[nodiscard]] const State& initialState() const noexcept { return initialState_; }

    static string encode(const State& state);
    static string formatTube(const Tube& tube, int capacity);

private:
    struct ParentInfo {
        string parentKey;
        Move move;
        bool hasParent = false;
    };

    int capacity_;
    State initialState_;

    [[nodiscard]] vector<Move> reconstructPath(
        const string& goalKey,
        const unordered_map<string, ParentInfo>& parents) const;
};
