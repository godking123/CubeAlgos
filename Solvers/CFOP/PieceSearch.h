#ifndef PIECE_SEARCH_H
#define PIECE_SEARCH_H

#include <cstdint>
#include <vector>
#include "../../CubeState/CubeState.h"

// Shortest Moves That Put a Set of Pieces in Place
namespace PieceSearch {
    // Masks For Edges/Corners To Include in Encoding/Solved State
    uint64_t encode(const CubeState& s, uint16_t edges, uint8_t corners);
    bool isSolved(const CubeState& s, uint16_t edges, uint8_t corners);

    // Bidirectional BFS
    std::vector<Move> solve(const CubeState& s, uint16_t edges, uint8_t corners);
} // namespace PieceSearch

#endif // PIECE_SEARCH_H
