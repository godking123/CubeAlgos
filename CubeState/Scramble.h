#ifndef SCRAMBLE_H
#define SCRAMBLE_H

#include <cstdint>
#include <string>
#include <vector>
#include "CubeState.h"

// Random Moves
std::vector<Move> randomScramble(int length = 25);            // Seeded Once From random_device
std::vector<Move> randomScramble(int length, uint64_t seed);  // Same Sequence for the Same Seed

// Random State
// Uniform over every legal cube, the 43 quintillion reachable by face turns
CubeState randomState();               // Seeded Once From random_device
CubeState randomState(uint64_t seed);  // Same State for the Same Seed

// Same permutation, fewest moves: same-face turns merge, also across the opposite face,
// and what cancels drops out. Each axis run comes out U before D, R before L, F before B
std::vector<Move> canonicalize(const std::vector<Move>& moves);

std::string sequenceName(const std::vector<Move>& moves);     // Sequence to "R U R' F2"

#endif // SCRAMBLE_H
