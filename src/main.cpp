#include "WaterSortSolver.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace {
State readState(int tubeCount, int capacity) {
    State state(tubeCount);

    for (int tubeIndex = 0; tubeIndex < tubeCount; ++tubeIndex) {
        bool zeroSeen = false;
        for (int position = 0; position < capacity; ++position) {
            int value = 0;
            if (!(std::cin >> value)) {
                throw std::runtime_error("Not enough tube values were provided.");
            }
            if (value < 0) {
                throw std::runtime_error("Color codes cannot be negative.");
            }
            if (value == 0) {
                zeroSeen = true;
            } else {
                if (zeroSeen) {
                    throw std::runtime_error(
                        "Invalid tube: zeros must appear only at the end of a tube line.");
                }
                state[tubeIndex].push_back(value);
            }
        }
    }

    return state;
}

void printFinalState(const State& state, int capacity) {
    std::cout << "Final state:\n";
    for (int i = 0; i < static_cast<int>(state.size()); ++i) {
        std::cout << "Tube " << i + 1 << ": "
                  << WaterSortSolver::formatTube(state[i], capacity) << '\n';
    }
}
}  // namespace

int main() {
    try {
        int tubeCount = 0;
        int capacity = 0;

        if (!(std::cin >> tubeCount) || tubeCount <= 0) {
            std::cerr << "Input error: number of tubes must be positive.\n";
            return 1;
        }
        if (!(std::cin >> capacity) || capacity <= 0) {
            std::cerr << "Input error: tube capacity must be positive.\n";
            return 1;
        }

        State initialState = readState(tubeCount, capacity);
        WaterSortSolver solver(capacity, std::move(initialState));
        const SolveResult result = solver.solve();

        if (!result.solved) {
            std::cout << "No solution exists.\n";
            return 0;
        }

        std::cout << "Solution found.\n";
        std::cout << "Minimum number of moves: " << result.moves.size() << '\n';
        for (std::size_t i = 0; i < result.moves.size(); ++i) {
            std::cout << i + 1 << ". Tube " << result.moves[i].source + 1
                      << " -> Tube " << result.moves[i].destination + 1 << '\n';
        }
        printFinalState(result.finalState, capacity);
        std::cout << "Explored states: " << result.exploredStates << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Input error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
