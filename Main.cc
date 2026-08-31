#include "CubeState/CubeAlgos.h"
#include "Solvers/Method.h"

#define RESET "\033[0m"
#define BOLD  "\033[1m"
#define GREEN "\033[32m"
#define CYAN  "\033[36m"
#define YELLOW "\033[33m"
#define RED   "\033[31m"

// print the method list, marking the one in use and the ones not written yet
static void printMethods(int current) {
    for (int i = 0; i < METHOD_COUNT; i++) {
        std::cout << "  " << (i == current ? GREEN "*" RESET : " ")
                  << " " << (i + 1) << ") " << BOLD << METHODS[i].name << RESET
                  << " — " << METHODS[i].description;
        if (!METHODS[i].implemented)
            std::cout << YELLOW << " (not implemented)" << RESET;
        std::cout << "\n";
    }
}

// read a method number from the user; returns -1 if they did not pick a new one
static int readMethod() {
    std::string input;
    std::cout << "Method number: ";
    if (!std::getline(std::cin, input)) return -1;

    int choice = 0;
    try {
        choice = std::stoi(input);
    } catch (const std::exception&) {
        std::cout << RED << "Not a number." << RESET << "\n\n";
        return -1;
    }
    if (choice < 1 || choice > METHOD_COUNT) {
        std::cout << RED << "No such method." << RESET << "\n\n";
        return -1;
    }
    if (!METHODS[choice - 1].implemented) {
        std::cout << YELLOW << METHODS[choice - 1].name
                  << " is not implemented yet." << RESET << "\n\n";
        return -1;
    }
    return choice - 1;
}

int main() {
    std::cout << BOLD << "Building tables..." << RESET << "\n";
    for (int i = 0; i < METHOD_COUNT; i++)
        METHODS[i].buildTables();
    std::cout << GREEN << "Ready.\n" << RESET << "\n";

    std::cout << BOLD
              << "Rubik's Cube Solver\n"
              << "───────────────────\n"
              << RESET;

    // pick a starting method before any solving happens
    int method = 0;
    printMethods(-1);
    std::cout << "\n";
    int picked = readMethod();
    if (picked >= 0) method = picked;

    std::cout << "\nSolving with " << BOLD << METHODS[method].name << RESET << ".\n"
              << "Press ENTER to generate a new scramble and solve it.\n"
              << "Type 'm' to change method, 'q' to quit.\n\n";

    std::string input;
    uint64_t seed = 42;

    while (true) {
        std::cout << "[ Press ENTER ] ";
        if (!std::getline(std::cin, input)) break;
        if (input == "q" || input == "Q") break;
        if (input == "m" || input == "M") {
            std::cout << "\n";
            printMethods(method);
            std::cout << "\n";
            picked = readMethod();
            if (picked >= 0) {
                method = picked;
                std::cout << "Now solving with " << BOLD << METHODS[method].name
                          << RESET << ".\n\n";
            }
            continue;
        }

        // generate scramble
        auto scrambleMoves = randomScramble(20, seed++);
        std::string scrambleStr = sequenceName(scrambleMoves);

        std::cout << "\n" << BOLD << CYAN
                  << "Scramble: " << RESET << scrambleStr << "\n";

        // apply scramble
        CubeState state = CubeState::solved();
        for (auto m : scrambleMoves)
            state = state.apply(m);

        // solve with the chosen method
        std::cout << YELLOW << "Solving with " << METHODS[method].name
                  << "..." << RESET << "\n";

        auto start = std::chrono::high_resolution_clock::now();
        auto solution = METHODS[method].solve(state);
        auto end   = std::chrono::high_resolution_clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();

        // replay the whole solution against the scramble, so the check covers the
        // sequence actually printed rather than any intermediate state
        CubeState check = state;
        for (auto m : solution)
            check = check.apply(m);

        bool solved = check.isSolved();

        // print result
        std::cout << BOLD << GREEN
                  << "Method:   " << RESET << METHODS[method].name << "\n";
        std::cout << BOLD << GREEN
                  << "Solution: " << RESET
                  << sequenceName(solution) << "\n";

        std::cout << BOLD << "Moves:    " << RESET << solution.size() << "\n";
        std::cout << BOLD << "Time:     " << RESET << ms << " ms\n";
        std::cout << BOLD << "Solved:   " << RESET
                  << (solved ? GREEN "YES" : RED "NO") << RESET << "\n\n";
    }

    std::cout << "Bye.\n";
    return 0;
}
