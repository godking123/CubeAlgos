#ifndef SCRAMBLE_H
#define SCRAMBLE_H

#include <cstdint>
#include <string>
#include <vector>
#include "CubeState.h"

std::vector<Move> randomScramble(int length = 25);            // Seeded Once From random_device
std::vector<Move> randomScramble(int length, uint64_t seed);  // Same Sequence for the Same Seed
std::string sequenceName(const std::vector<Move>& moves);     // Sequence to "R U R' F2"

#endif // SCRAMBLE_H
