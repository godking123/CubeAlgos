#include <iostream>
#include <string>
#include "../../../CubeState/Scramble.h"
#include "../../../CubeState/Rotation.h"
#include "../../../Solvers/KociembaNaive/Kociemba.h"
#include "../../../Scramblers/WCA.h"
#include "Cross.h"

// Interactive cross tester, Enter for the next scramble, q or EOF to quit
// Scrambles are WCA random state, so they need the solver's tables
int main() {
    Kociemba::buildTables();
    std::cout << "Scramble with " << orientationName(Orientation{}) << "\n\n";
    std::string line;
    int n = 1;
    while (true) {
        auto scramble = WCA::scramble();
        CubeState cube = CubeState::solved();
        for (auto m : scramble) cube = cube.apply(m);

        CrossResult best = Cross::bestCross(cube);
        std::string rotation = rotationsTo(best.hold);

        std::cout << "Scramble " << n++ << ": " << sequenceName(scramble) << "\n"
                  << "Color:    " << colorName(best.color) << "\n"
                  << "Rotation: " << (rotation.empty() ? "(none)" : rotation) << "\n"
                  << "Hold:     " << orientationName(best.hold) << "\n"
                  << "Cross:    " << sequenceName(best.moves)
                  << "  (" << best.moves.size() << " moves)\n";

        std::cout << "[Enter] next, q quit > " << std::flush;
        if (!std::getline(std::cin, line) || line == "q") break;
        std::cout << "\n";
    }
    return 0;
}
