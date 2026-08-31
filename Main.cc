#include "CubeState/CubeAlgos.h"
#include "Solver/Phase1.h"
#include "Solver/Phase2.h"

#define RESET "\033[0m"
#define BOLD  "\033[1m"
#define GREEN "\033[32m"
#define CYAN  "\033[36m"
#define YELLOW "\033[33m"
#define RED   "\033[31m"

int main() {
    std::cout << BOLD << "Building tables..." << RESET << "\n";
    buildCoordTables();
    buildPruningTables();
    buildPhase2Tables();
    buildPhase2PruningTables();
    std::cout << GREEN << "Ready.\n" << RESET << "\n";

    std::cout << BOLD
              << "Two-Phase Solver\n"
              << "────────────────\n"
              << RESET
              << "Press ENTER to generate a new scramble and solve it.\n"
              << "Type 'q' and ENTER to quit.\n\n";

    std::string input;
    uint64_t seed = 42;

    while (true) {
        std::cout << "[ Press ENTER ] ";
        if (!std::getline(std::cin, input)) break;
        if (input == "q" || input == "Q") break;

        // generate scramble
        auto scrambleMoves = randomScramble(20, seed++);
        std::string scrambleStr = sequenceName(scrambleMoves);

        std::cout << "\n" << BOLD << CYAN
                  << "Scramble: " << RESET << scrambleStr << "\n";

        // apply scramble
        CubeState state = CubeState::solved();
        for (auto m : scrambleMoves)
            state = state.apply(m);

        // run phase 1
        std::cout << YELLOW << "Solving..." << RESET << "\n";

        auto start = std::chrono::high_resolution_clock::now();
        auto phase1 = Phase1::solve(state);

        // apply phase 1 and verify it landed in G1
        CubeState afterPhase1 = state;
        for (auto m : phase1)
            afterPhase1 = afterPhase1.apply(m);

        bool isG1 = Coords::encodeTwist(afterPhase1) == 0
                 && Coords::encodeFlip(afterPhase1)  == 0
                 && Coords::encodeSlice(afterPhase1) == 0;

        // run phase 2 from there
        auto phase2 = Phase2::solve(afterPhase1);
        auto end   = std::chrono::high_resolution_clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();

        // the full solve is the two phases end to end
        std::vector<Move> solution = phase1;
        solution.insert(solution.end(), phase2.begin(), phase2.end());

        // replay the whole solution against the scramble, not against the phase 1
        // result, so the check covers the joined sequence rather than each half
        CubeState check = state;
        for (auto m : solution)
            check = check.apply(m);

        bool solved = check.isSolved();

        // print result
        std::cout << BOLD << GREEN
                  << "Phase 1:  " << RESET
                  << sequenceName(phase1) << "\n";
        std::cout << BOLD << GREEN
                  << "Phase 2:  " << RESET
                  << sequenceName(phase2) << "\n";
        std::cout << BOLD << GREEN
                  << "Solution: " << RESET
                  << sequenceName(solution) << "\n";

        std::cout << BOLD << "Moves:    " << RESET
                  << phase1.size() << " + " << phase2.size()
                  << " = " << solution.size() << "\n";
        std::cout << BOLD << "Time:     " << RESET << ms << " ms\n";
        std::cout << BOLD << "G1:       " << RESET
                  << (isG1 ? GREEN "YES" : RED "NO") << RESET << "\n";
        std::cout << BOLD << "Solved:   " << RESET
                  << (solved ? GREEN "YES" : RED "NO") << RESET << "\n\n";
    }

    std::cout << "Bye.\n";
    return 0;
}
