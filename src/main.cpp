#include "WaterSortSolver.hpp"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        int n, capacity;
        if (!(std::cin >> n >> capacity) || n <= 0 || capacity <= 0) {
            std::cerr << "Invalid number of tubes or capacity.\n";
            return 1;
        }
        State state(n);
        for (int i = 0; i < n; ++i) {
            bool zeroSeen = false;
            for (int j = 0; j < capacity; ++j) {
                int value; std::cin >> value;
                if (!std::cin || value < 0 || (zeroSeen && value != 0)) {
                    std::cerr << "Invalid tube data. Zeros must be at the end.\n";
                    return 1;
                }
                if (value == 0) zeroSeen = true;
                else state[i].push_back(value);
            }
        }

        WaterSortSolver solver(capacity, state);
        auto result = solver.solveMinimumMoves();
        if (!result.solved) {
            std::cout << "No solution exists.\n";
            return 0;
        }
        std::cout << "Solution found.\nMinimum number of moves: " << result.moves.size() << "\n";
        for (std::size_t i = 0; i < result.moves.size(); ++i) {
            std::cout << i + 1 << ". Tube " << result.moves[i].from + 1
                      << " -> Tube " << result.moves[i].to + 1 << '\n';
        }
        std::cout << "Final state:\n";
        for (std::size_t i = 0; i < result.finalState.size(); ++i) {
            std::cout << "Tube " << i + 1 << ": [";
            for (int j = 0; j < capacity; ++j) {
                if (j) std::cout << ',';
                std::cout << (j < static_cast<int>(result.finalState[i].size()) ? result.finalState[i][j] : 0);
            }
            std::cout << "]\n";
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
