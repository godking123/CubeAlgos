#include "Cross.h"
#include "../../../CubeState/CubeState.h"
#include <unordered_map>
#include <queue>

namespace Cross {

bool isSolved(const CubeState& s) {
    return s.ep[4]==4 && s.eo[4]==0   // DR
        && s.ep[5]==5 && s.eo[5]==0   // DF
        && s.ep[6]==6 && s.eo[6]==0   // DL
        && s.ep[7]==7 && s.eo[7]==0;  // DB
}

static uint32_t encodeCross(const CubeState& s) {
    uint32_t key = 0;
    for (int piece = 4; piece <= 7; piece++) {
        for (int slot = 0; slot < 12; slot++) {
            if (s.ep[slot] == piece) {
                key = (key << 5) | (slot << 1) | s.eo[slot];
                break;
            }
        }
    }
    return key;
}

static Move inverseOf(Move m) {
    int i = static_cast<int>(m);
    int face = i / 3, kind = i % 3;
    return static_cast<Move>(face * 3 + (kind == 1 ? 1 : 2 - kind));
}

// One Level of BFS
static std::vector<Move> bfsCross(std::queue<std::pair<CubeState, std::vector<Move>>>& frontier,
    std::unordered_map<uint32_t, std::vector<Move>>& mine, 
    std::unordered_map<uint32_t, std::vector<Move>>& other,
    bool isForward) {
    
    int levelSize = frontier.size();
    for (int i = 0; i < levelSize; i++) {
        // Grab Current Node
        auto [state, path] = frontier.front();
        frontier.pop();

        // Expand Into Node's Neighbors
        for (int m = 0; m < 18; m++) {
            CubeState next = state.apply(static_cast<Move>(m));
            uint32_t key = encodeCross(next);
            
            if (mine.count(key) == 1) continue;
            std::vector<Move> newPath = path;
            newPath.push_back(static_cast<Move>(m));

            // Found a Solution
            if (other.count(key) == 1) {
                std::vector<Move> solution;
                // Other map is Backward
                // Concat Inverse of That to Get Sol
                if (isForward) {
                    solution = newPath;
                    for (const auto& fm : other[key]) {
                        solution.push_back(inverseOf(fm));
                    }
                } else {
                    solution = other[key];
                    for (const auto& bm : newPath) {
                        solution.push_back(inverseOf(bm));
                    }
                }
                return solution;
            }
            
            // Update Frontier
            mine[key] = newPath;
            frontier.push({next, newPath});
        }
    }

    return {};             
}



std::vector<Move> solveCross(const CubeState& scrambled) {
    if (isSolved(scrambled)) return {};
    const CubeState solved = CubeState::solved();

    // Map Current State : Moves to Get There
    std::unordered_map<uint32_t, std::vector<Move>> forward;
    std::unordered_map<uint32_t, std::vector<Move>> backward;

    // Create Frontiers
    std::queue<std::pair<CubeState, std::vector<Move>>> forwardF;
    std::queue<std::pair<CubeState, std::vector<Move>>> backwardF;

    // Input Initial Vals
    forward[encodeCross(scrambled)] = {};
    backward[encodeCross(solved)] = {};
    forwardF.push({scrambled, {}});
    backwardF.push({solved, {}});

    // Bidirectional BFS
    while (!forwardF.empty() && !backwardF.empty()) {
        std::vector<Move> result;
        if (forwardF.size() <= backwardF.size())
            result = bfsCross(forwardF, forward, backward, true);
        else
            result = bfsCross(backwardF, backward, forward, false);
        if (!result.empty()) return result;
    }
    return {};
}

} // namespace Cross
