#include <iostream>
#include <string>
#include "../../../CubeState/Scramble.h"
#include "../../../CubeState/SolverState.h"
#include "Cross.h"

// Interactive cross tester, Enter for the next scramble, q or EOF to quit
int main() {
    std::cout << "Scramble with " << orientationName(Orientation{}) << "\n\n";
    std::string line;
    int n = 1;
    while (true) {
        auto scramble = randomScramble();
        CubeState cube = CubeState::solved();
        for (auto m : scramble) cube = cube.apply(m);

        CrossResult best = Cross::bestCross(cube);
        SolverState view;
        view = faceToBottom(view, best.color);

        std::cout << "Scramble " << n++ << ": " << sequenceName(scramble) << "\n"
                  << "Color:    " << colorName(best.color) << "\n"
                  << "Rotation: " << (best.rotation[0] ? best.rotation : "(none)") << "\n"
                  << "Hold:     " << orientationName(view.view) << "\n"
                  << "Cross:    " << sequenceName(best.moves)
                  << "  (" << best.moves.size() << " moves)\n";

        std::cout << "[Enter] next, q quit > " << std::flush;
        if (!std::getline(std::cin, line) || line == "q") break;
        std::cout << "\n";
    }
    return 0;
}
