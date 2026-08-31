#include "CubeState/CubeAlgos.h"
#include "Solver/Phase1.h"

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
    std::cout << GREEN << "Ready.\n" << RESET << "\n";

    std::cout << BOLD
              << "CFOP Phase 1 Solver\n"
              << "───────────────────\n"
              << RESET
              << "Press ENTER to generate a new scramble and solve Phase 1.\n"
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
        std::cout << YELLOW << "Solving Phase 1..." << RESET << "\n";

        auto start = std::chrono::high_resolution_clock::now();
        auto solution = Phase1::solve(state);
        auto end   = std::chrono::high_resolution_clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();

        // apply solution and verify
        CubeState afterPhase1 = state;
        for (auto m : solution)
            afterPhase1 = afterPhase1.apply(m);

        bool isG1 = Coords::encodeTwist(afterPhase1) == 0
                 && Coords::encodeFlip(afterPhase1)  == 0
                 && Coords::encodeSlice(afterPhase1) == 0;

        // print result
        std::cout << BOLD << GREEN
                  << "Phase 1:  " << RESET
                  << sequenceName(solution) << "\n";

        std::cout << BOLD << "Moves:    " << RESET << solution.size() << "\n";
        std::cout << BOLD << "Time:     " << RESET << ms << " ms\n";
        std::cout << BOLD << "G1:       " << RESET
                  << (isG1 ? GREEN "YES" : RED "NO") << RESET << "\n\n";
    }

    std::cout << "Bye.\n";
    return 0;
}
