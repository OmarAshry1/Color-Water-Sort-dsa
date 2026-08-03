#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct Move {
    int source = -1;
    int destination = -1;
};

using Tube = std::vector<int>;      // Bottom to top. Empty slots are omitted internally.
using State = std::vector<Tube>;

struct SolveResult {
    bool solved = false;
    std::vector<Move> moves;
    State finalState;
    std::size_t exploredStates = 0;
};

class WaterSortSolver {
public:
    WaterSortSolver(int capacity, State initialState);

    [[nodiscard]] SolveResult solve() const;
    [[nodiscard]] bool isGoal(const State& state) const;
    [[nodiscard]] std::vector<Move> legalMoves(const State& state) const;
    [[nodiscard]] State applyMove(const State& state, const Move& move) const;

    [[nodiscard]] int capacity() const noexcept { return capacity_; }
    [[nodiscard]] const State& initialState() const noexcept { return initialState_; }

    static std::string encode(const State& state);
    static std::string formatTube(const Tube& tube, int capacity);

private:
    struct ParentInfo {
        std::string parentKey;
        Move move;
        bool hasParent = false;
    };

    int capacity_;
    State initialState_;

    [[nodiscard]] std::vector<Move> reconstructPath(
        const std::string& goalKey,
        const std::unordered_map<std::string, ParentInfo>& parents) const;
};
