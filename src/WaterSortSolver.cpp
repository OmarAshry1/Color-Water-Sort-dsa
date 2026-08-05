#include "WaterSortSolver.hpp"

#include <algorithm>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

WaterSortSolver::WaterSortSolver(int capacity, State initialState)
    : capacity_(capacity), initialState_(std::move(initialState)) {
    if (capacity_ <= 0) {
        throw std::invalid_argument("Tube capacity must be positive.");
    }
    for (const auto& tube : initialState_) {
        if (static_cast<int>(tube.size()) > capacity_) {
            throw std::invalid_argument("A tube contains more layers than its capacity.");
        }
        if (std::any_of(tube.begin(), tube.end(), [](int color) { return color <= 0; })) {
            throw std::invalid_argument("Internal tube representation accepts positive color codes only.");
        }
    }
}

bool WaterSortSolver::isGoal(const State& state) const {
    for (const auto& tube : state) {
        if (tube.empty()) {
            continue;
        }
        if (static_cast<int>(tube.size()) != capacity_) {
            return false;
        }
        if (!std::all_of(tube.begin(), tube.end(), [&](int color) {
                return color == tube.front();
            })) {
            return false;
        }
    }
    return true;
}

std::vector<Move> WaterSortSolver::legalMoves(const State& state) const {
    std::vector<Move> moves;

    for (int source = 0; source < static_cast<int>(state.size()); ++source) {
        if (state[source].empty()) {
            continue;
        }

        const int topColor = state[source].back();
        for (int destination = 0; destination < static_cast<int>(state.size()); ++destination) {
            if (source == destination || static_cast<int>(state[destination].size()) >= capacity_) {
                continue;
            }

            // A layer may be poured into an empty tube or onto the same color.
            if (state[destination].empty() || state[destination].back() == topColor) {
                moves.push_back({source, destination});
            }
        }
    }

    return moves;
}

State WaterSortSolver::applyMove(const State& state, const Move& move) const {
    if (move.source < 0 || move.destination < 0 ||
        move.source >= static_cast<int>(state.size()) ||
        move.destination >= static_cast<int>(state.size()) ||
        move.source == move.destination) {
        throw std::invalid_argument("Move contains invalid tube indices.");
    }

    State next = state;
    if (next[move.source].empty()) {
        throw std::invalid_argument("Cannot pour from an empty tube.");
    }
    if (static_cast<int>(next[move.destination].size()) >= capacity_) {
        throw std::invalid_argument("Cannot pour into a full tube.");
    }

    const int color = next[move.source].back();
    if (!next[move.destination].empty() && next[move.destination].back() != color) {
        throw std::invalid_argument("A layer can only be poured onto the same color or an empty tube.");
    }

    next[move.source].pop_back();
    next[move.destination].push_back(color);
    return next;
}

std::string WaterSortSolver::encode(const State& state) {
    std::ostringstream out;
    for (const auto& tube : state) {
        out << '[';
        for (int color : tube) {
            out << color << ',';
        }
        out << ']';
    }
    return out.str();
}

std::string WaterSortSolver::formatTube(const Tube& tube, int capacity) {
    std::ostringstream out;
    out << '[';
    for (int i = 0; i < capacity; ++i) {
        if (i != 0) {
            out << ',';
        }
        out << (i < static_cast<int>(tube.size()) ? tube[i] : 0);
    }
    out << ']';
    return out.str();
}

std::vector<Move> WaterSortSolver::reconstructPath(
    const std::string& goalKey,
    const std::unordered_map<std::string, ParentInfo>& parents) const {
    std::vector<Move> path;
    std::string current = goalKey;

    while (true) {
        const auto it = parents.find(current);
        if (it == parents.end() || !it->second.hasParent) {
            break;
        }
        path.push_back(it->second.move);
        current = it->second.parentKey;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

SolveResult WaterSortSolver::solve() const {
    SolveResult result;
    result.finalState = initialState_;

    const std::string startKey = encode(initialState_);
    if (isGoal(initialState_)) {
        result.solved = true;
        result.exploredStates = 1;
        return result;
    }

    std::queue<State> frontier;
    std::unordered_map<std::string, ParentInfo> parents;
    frontier.push(initialState_);
    parents.emplace(startKey, ParentInfo{});

    while (!frontier.empty()) {
        State current = std::move(frontier.front());
        frontier.pop();
        ++result.exploredStates;

        const std::string currentKey = encode(current);
        for (const Move& move : legalMoves(current)) {
            State next = applyMove(current, move);
            const std::string nextKey = encode(next);

            if (parents.find(nextKey) != parents.end()) {
                continue;
            }

            parents.emplace(nextKey, ParentInfo{currentKey, move, true});
            if (isGoal(next)) {
                result.solved = true;
                result.moves = reconstructPath(nextKey, parents);
                result.finalState = std::move(next);
                return result;
            }

            frontier.push(std::move(next));
        }
    }

    result.solved = false;
    return result;
}
