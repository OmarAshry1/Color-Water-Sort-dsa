#include "WaterSortSolver.hpp"

#include <cassert>
#include <iostream>

int main() {
    {
        WaterSortSolver solver(2, {{1, 2}, {2, 1}, {}, {}});
        const SolveResult result = solver.solve();
        assert(result.solved);
        assert(result.moves.size() == 3);
        assert(solver.isGoal(result.finalState));
    }

    {
        WaterSortSolver solver(2, {{1, 2}, {2, 1}});
        const SolveResult result = solver.solve();
        assert(!result.solved);
    }

    {
        WaterSortSolver solver(3, {{1, 1, 1}, {2, 2, 2}, {}});
        const SolveResult result = solver.solve();
        assert(result.solved);
        assert(result.moves.empty());
    }

    {
        WaterSortSolver solver(2, {{1, 2}, {1, 2}, {}, {}});
        const SolveResult result = solver.solve();
        assert(result.solved);
        assert(result.moves.size() == 3);
    }

    std::cout << "All tests passed.\n";
    return 0;
}
