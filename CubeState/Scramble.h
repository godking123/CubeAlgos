#ifndef SCRAMBLE_H
#define SCRAMBLE_H

#include <cstdint>
#include <string>
#include <vector>
#include "CubeState.h"

std::vector<Move> randomScramble(int length = 25);            // seeded once from random_device
std::vector<Move> randomScramble(int length, uint64_t seed);  // same sequence for the same seed
std::string sequenceName(const std::vector<Move>& moves);     // sequence -> "R U R' F2"

#endif // SCRAMBLE_H
