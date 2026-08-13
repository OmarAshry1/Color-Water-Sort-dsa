#include "WaterSortSolver.hpp"
#include <algorithm>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

using namespace std;

WaterSortSolver::WaterSortSolver(int capacity, State initial)
    : capacity_(capacity), initial_(move(initial)) {
    if (capacity_ <= 0) throw invalid_argument("Capacity must be positive");
    for (const auto& tube : initial_) {
        if (static_cast<int>(tube.size()) > capacity_) throw invalid_argument("Tube exceeds capacity");
        for (int color : tube) if (color <= 0) throw invalid_argument("Stored colors must be positive");
    }
}

bool WaterSortSolver::isSolved(const State& state, int capacity) {
    for (const auto& tube : state) {
        if (tube.empty()) continue;
        if (static_cast<int>(tube.size()) != capacity) return false;
        if (!all_of(tube.begin(), tube.end(), [&](int c){ return c == tube.front(); })) return false;
    }
    return true;
}

bool WaterSortSolver::canPour(const State& state, int capacity, int from, int to) {
    if (from == to || from < 0 || to < 0 || from >= static_cast<int>(state.size()) || to >= static_cast<int>(state.size())) return false;
    const auto& src = state[from];
    const auto& dst = state[to];
    if (src.empty() || static_cast<int>(dst.size()) >= capacity) return false;
    return dst.empty() || dst.back() == src.back();
}

Move WaterSortSolver::makePour(State& state, int capacity, int from, int to) {
    if (!canPour(state, capacity, from, to)) return Move{-1, -1, 0, 0};
    auto& src = state[from];
    auto& dst = state[to];
    const int color = src.back();
    int sameTop = 0;
    for (auto it = src.rbegin(); it != src.rend() && *it == color; ++it) ++sameTop;
    const int amount = min(sameTop, capacity - static_cast<int>(dst.size()));
    for (int i = 0; i < amount; ++i) {
        dst.push_back(src.back());
        src.pop_back();
    }
    return Move{from, to, amount, color};
}

string WaterSortSolver::encode(const State& state) {
    ostringstream out;
    for (const auto& tube : state) {
        out << '[';
        for (int c : tube) out << c << ',';
        out << ']';
    }
    return out.str();
}

SolveResult WaterSortSolver::solveMinimumMoves() {
    SolveResult result;
    if (isSolved(initial_, capacity_)) {
        result.solved = true;
        result.finalState = initial_;
        result.visitedStates = 1;
        result.message = "Puzzle is already solved.";
        return result;
    }

    struct Node { State state; int parent; Move move; };
    vector<Node> nodes;
    nodes.push_back({initial_, -1, {}});
    queue<int> frontier;
    frontier.push(0);
    unordered_set<string> seen;
    seen.insert(encode(initial_));

    int goal = -1;
    while (!frontier.empty()) {
        const int index = frontier.front(); frontier.pop();
        const State current = nodes[index].state;

        for (int from = 0; from < static_cast<int>(current.size()); ++from) {
            for (int to = 0; to < static_cast<int>(current.size()); ++to) {
                if (!canPour(current, capacity_, from, to)) continue;

                // Prune moves that merely transfer a complete uniform tube into an empty tube.
                const auto& src = current[from];
                const auto& dst = current[to];
                const bool uniformFull = static_cast<int>(src.size()) == capacity_ &&
                    all_of(src.begin(), src.end(), [&](int c){ return c == src.front(); });
                if (dst.empty() && uniformFull) continue;

                State next = current;
                Move move = makePour(next, capacity_, from, to);
                const string key = encode(next);
                if (!seen.insert(key).second) continue;
                nodes.push_back({std::move(next), index, move});
                const int child = static_cast<int>(nodes.size()) - 1;
                if (isSolved(nodes[child].state, capacity_)) { goal = child; break; }
                frontier.push(child);
            }
            if (goal != -1) break;
        }
        if (goal != -1) break;
    }

    result.visitedStates = seen.size();
    if (goal == -1) {
        result.solved = false;
        result.finalState = initial_;
        result.message = "No solution exists.";
        return result;
    }

    result.solved = true;
    result.finalState = nodes[goal].state;
    for (int cur = goal; nodes[cur].parent != -1; cur = nodes[cur].parent) result.moves.push_back(nodes[cur].move);
    reverse(result.moves.begin(), result.moves.end());
    result.message = "Minimum-move solution found.";
    return result;
}
