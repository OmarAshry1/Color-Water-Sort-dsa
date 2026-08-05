#include "../include/WaterSortSolver.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
namespace {
State readState(int tubeCount, int capacity) {
    State state(tubeCount);

    for (int tubeIndex = 0; tubeIndex < tubeCount; ++tubeIndex) {
        bool zeroSeen = false;
        for (int position = 0; position < capacity; ++position) {
            int value = 0;
            if (!(cin >> value)) {
                throw runtime_error("Not enough tube values were provided.");
            }
            if (value < 0) {
                throw runtime_error("Color codes cannot be negative.");
            }
            if (value == 0) {
                zeroSeen = true;
            } else {
                if (zeroSeen) {
                    throw runtime_error(
                        "Invalid tube: zeros must appear only at the end of a tube line.");
                }
                state[tubeIndex].push_back(value);
            }
        }
    }

    return state;
}

void printFinalState(const State& state, int capacity) {
    cout << "Final state:\n";
    for (int i = 0; i < static_cast<int>(state.size()); ++i) {
        cout << "Tube " << i + 1 << ": "
                  << WaterSortSolver::formatTube(state[i], capacity) << '\n';
    }
}
}  // namespace

int main() {
    try {
        int tubeCount = 0;
        int capacity = 0;

        if (!(cin >> tubeCount) || tubeCount <= 0) {
            cerr << "Input error: number of tubes must be positive.\n";
            return 1;
        }
        if (!(cin >> capacity) || capacity <= 0) {
            cerr << "Input error: tube capacity must be positive.\n";
            return 1;
        }

        State initialState = readState(tubeCount, capacity);
        WaterSortSolver solver(capacity, move(initialState));
        const SolveResult result = solver.solve();

        if (!result.solved) {
            cout << "No solution exists.\n";
            return 0;
        }

        cout << "Solution found.\n";
        cout << "Minimum number of moves: " << result.moves.size() << '\n';
        for (size_t i = 0; i < result.moves.size(); ++i) {
            cout << i + 1 << ". Tube " << result.moves[i].source + 1
                      << " -> Tube " << result.moves[i].destination + 1 << '\n';
        }
        printFinalState(result.finalState, capacity);
        cout << "Explored states: " << result.exploredStates << '\n';
    } catch (const exception& error) {
        cerr << "Input error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
