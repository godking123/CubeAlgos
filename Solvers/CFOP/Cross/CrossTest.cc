#include <iostream>
#include <ctime>
#include "Cross.h"
#include "../../../CubeState/CubeAlgos.h"

int main() {
    unsigned seed = (unsigned)std::time(nullptr);
    
    for (int i = 0; i < 5; i++) {
        auto scramble = randomScramble(20, seed++);
        CubeState s = CubeState::solved();
        for (auto m : scramble) s = s.apply(m);

        std::cout << "Scramble: " << sequenceName(scramble) << "\n";
        auto sol = Cross::solveCross(s);
        std::cout << "Cross:    " << sequenceName(sol) << "\n";
        CubeState after = s;
        for (auto m : sol) after = after.apply(m);
        std::cout << "Solved:   " << (Cross::isSolved(after) ? "YES" : "NO") << "\n\n";
    }
    return 0;
}