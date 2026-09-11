#include "Cross.h"
#include <unordered_map>
#include <queue>

namespace Cross {

bool isSolved(const CubeState& s) {
    return s.ep[4]==4 && s.eo[4]==0   // DR
        && s.ep[5]==5 && s.eo[5]==0   // DF
        && s.ep[6]==6 && s.eo[6]==0   // DL
        && s.ep[7]==7 && s.eo[7]==0;  // DB
}

// Slot and Flip of Each D Edge, Five Bits Apiece
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

// One Side of the Bidirectional Search
struct Frontier {
    std::queue<std::pair<CubeState, std::vector<Move>>> queue;
    std::unordered_map<uint32_t, std::vector<Move>> seen;   // Cross Key : Moves to Get There

    Frontier(const CubeState& start) {
        seen[encodeCross(start)] = {};
        queue.push({start, {}});
    }
};

// Forward path, then the backward path reversed and inverted
static std::vector<Move> joinPaths(const std::vector<Move>& forward,
                                   const std::vector<Move>& backward) {
    std::vector<Move> solution = forward;
    for (int j = backward.size() - 1; j >= 0; j--) {
        solution.push_back(inverseMove(backward[j]));
    }
    return solution;
}

// One Level of BFS
static std::vector<Move> expand(Frontier& mine, const Frontier& other, bool isForward) {
    int levelSize = mine.queue.size();
    for (int i = 0; i < levelSize; i++) {
        // Grab Current Node
        auto [state, path] = mine.queue.front();
        mine.queue.pop();

        // Expand Into Node's Neighbors
        for (int m = 0; m < 18; m++) {
            CubeState next = state.apply(static_cast<Move>(m));
            uint32_t key = encodeCross(next);

            if (mine.seen.count(key) == 1) continue;
            std::vector<Move> newPath = path;
            newPath.push_back(static_cast<Move>(m));

            // Found a Solution
            auto hit = other.seen.find(key);
            if (hit != other.seen.end()) {
                return isForward ? joinPaths(newPath, hit->second)
                                 : joinPaths(hit->second, newPath);
            }

            // Update Frontier
            mine.seen[key] = newPath;
            mine.queue.push({next, newPath});
        }
    }
    return {};
}

std::vector<Move> solveCross(const CubeState& s) {
    if (isSolved(s)) return {};
    Frontier forward(s);
    Frontier backward(CubeState::solved());

    // Bidirectional BFS, Always Grow the Smaller Side
    while (!forward.queue.empty() && !backward.queue.empty()) {
        std::vector<Move> result;
        if (forward.queue.size() <= backward.queue.size())
            result = expand(forward, backward, true);
        else
            result = expand(backward, forward, false);
        if (!result.empty()) return result;
    }
    return {};
}

// Solve every colour in its own frame, keep the shortest
// Ties go to the lowest colour index, so white beats yellow beats the sides
CrossResult bestCross(const CubeState& scrambled) {
    CrossResult best;
    bool found = false;
    for (int c = 0; c < 6; c++) {
        Orientation hold = orientationWithBottom(c);
        std::vector<Move> moves = solveCross(scrambled.rotate(hold));
        if (!found || moves.size() < best.moves.size()) {
            best.color = c;
            best.hold  = hold;
            best.moves = moves;
            found = true;
        }
    }
    return best;
}

} // namespace Cross
