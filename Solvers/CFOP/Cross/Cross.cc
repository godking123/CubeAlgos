#include "Cross.h"
#include "../../../CubeState/CubeState.h"
#include <unordered_map>
#include <queue>
#include "../../../CubeState/SolverState.h"

namespace Cross {

// Edge Slots Around Each Physical Face, U R F D L B
static const int CROSS_SLOTS[6][4] = {
    {0, 1, 2, 3},    // U: UR UF UL UB
    {0, 8, 4, 11},   // R: UR FR DR BR
    {1, 8, 5, 9},    // F: UF FR DF FL
    {4, 5, 6, 7},    // D: DR DF DL DB
    {2, 9, 6, 10},   // L: UL FL DL BL
    {3, 10, 7, 11},  // B: UB BL DB BR
};

// The cross face is whichever physical face the view puts on the bottom
bool isSolved(const SolverState& s) {
    const int* slots = CROSS_SLOTS[s.view.faces[3]];
    for (int i = 0; i < 4; i++) {
        if (s.cube.ep[slots[i]] != slots[i] || s.cube.eo[slots[i]] != 0) return false;
    }
    return true;
}

static uint32_t encodeCross(const SolverState& s) {
    const int* pieces = CROSS_SLOTS[s.view.faces[3]];
    uint32_t key = 0;
    for (int i = 0; i < 4; i++) {
        for (int slot = 0; slot < 12; slot++) {
            if (s.cube.ep[slot] == pieces[i]) {
                key = (key << 5) | (slot << 1) | s.cube.eo[slot];
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
static std::vector<Move> bfsCross(std::queue<std::pair<SolverState, std::vector<Move>>>& frontier,
    std::unordered_map<uint32_t, std::vector<Move>>& mine, 
    std::unordered_map<uint32_t, std::vector<Move>>& other,
    bool isForward) {
    
    int levelSize = frontier.size();
    for (int i = 0; i < levelSize; i++) {
        // Grab Current Node
        auto [ss, path] = frontier.front();
        frontier.pop();

        // Expand Into Node's Neighbors
        for (int m = 0; m < 18; m++) {
            SolverState next = applyMove(ss, static_cast<Move>(m));
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
                    for (int j = other[key].size() - 1; j >= 0; j--) {
                        solution.push_back(inverseOf(other[key][j]));
                    }
                } else {
                    solution = other[key];
                    for (int j = newPath.size() - 1; j>= 0; j--) {
                        solution.push_back(inverseOf(newPath[j]));
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

std::vector<Move> solveCross(const SolverState& ss) {
    if (isSolved(ss)) return {};
    SolverState solvedSS;
    solvedSS.cube = CubeState::solved();
    solvedSS.view = ss.view;

    // Map Current State : Moves to Get There
    std::unordered_map<uint32_t, std::vector<Move>> forward;
    std::unordered_map<uint32_t, std::vector<Move>> backward;

    // Create Frontiers
    std::queue<std::pair<SolverState, std::vector<Move>>> forwardF;
    std::queue<std::pair<SolverState, std::vector<Move>>> backwardF;

    // Input Initial Vals
    forward[encodeCross(ss)] = {};
    backward[encodeCross(solvedSS)] = {};
    forwardF.push({ss, {}});
    backwardF.push({solvedSS, {}});

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

CrossResult bestCross(const CubeState& scrambled) {
    CrossResult best;
    best.color    = 3;
    best.rotation = "";
    best.moves.resize(20);
    for (int c = 0; c < 6; c++) {
        SolverState ss;
        ss.cube = scrambled;
        ss.view = Orientation{};
        ss = faceToBottom(ss, c);
        auto sol = solveCross(ss);
        if (sol.size() < best.moves.size()) {
            best.color    = c;
            best.rotation = faceToBottomName(c);
            best.moves    = sol;
        }
    }
    return best;
}

} // namespace Cross
