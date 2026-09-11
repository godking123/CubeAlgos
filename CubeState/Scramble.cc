#include "Scramble.h"
#include <algorithm>
#include <random>
#include <stdexcept>

// Moves group three per face in the Move enum, so face = move / 3
// 0=U 1=R 2=F 3=D 4=L 5=B, opposite faces sit 3 apart

// One Engine Per Thread
static std::mt19937_64& defaultRng() {
    static thread_local std::mt19937_64 rng(std::random_device{}());
    return rng;
}

// Legal When Not The Previous Face
static bool faceAllowed(int face, int prevFace, int prevPrevFace) {
    if (face == prevFace) return false;
    if (face == (prevFace + 3) % 6 && face == prevPrevFace) return false;
    return true;
}

// Draw From The Legal Faces
static std::vector<Move> generate(int length, std::mt19937_64& rng) {
    if (length < 0) throw std::invalid_argument("randomScramble: length must be non-negative");

    std::vector<Move> moves;
    moves.reserve(length);

    std::uniform_int_distribution<int> turnPick(0, 2);
    int prevFace = -1;
    int prevPrevFace = -1;

    for (int i = 0; i < length; i++) {
        int legal[6];
        int n = 0;
        for (int f = 0; f < 6; f++) {
            if (faceAllowed(f, prevFace, prevPrevFace)) legal[n++] = f;
        }

        std::uniform_int_distribution<int> facePick(0, n - 1);
        int face = legal[facePick(rng)];
        moves.push_back(static_cast<Move>(face * 3 + turnPick(rng)));

        prevPrevFace = prevFace;
        prevFace = face;
    }
    return moves;
}

std::vector<Move> randomScramble(int length) {
    return generate(length, defaultRng());
}

std::vector<Move> randomScramble(int length, uint64_t seed) {
    std::mt19937_64 rng(seed);
    return generate(length, rng);
}

// Parity of a Permutation, 1 When Odd
static int parity(const uint8_t* p, int n) {
    int inversions = 0;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (p[j] < p[i]) inversions++;
    return inversions % 2;
}

// Each array is drawn uniformly, then the last freedom is spent on the invariant
//
// Corner and edge permutations must share a parity. Swapping two edges is a bijection
// between the odd and even edge permutations, so fixing a mismatch that way leaves the
// draw uniform over the legal pairs. Orientations work the same way: the last piece
// takes whatever twist or flip closes the sum, and every combination of the others is
// equally likely
static CubeState draw(std::mt19937_64& rng) {
    CubeState s = CubeState::solved();
    std::shuffle(s.cp, s.cp + 8,  rng);
    std::shuffle(s.ep, s.ep + 12, rng);
    if (parity(s.cp, 8) != parity(s.ep, 12)) std::swap(s.ep[0], s.ep[1]);

    std::uniform_int_distribution<int> twist(0, 2);
    std::uniform_int_distribution<int> flip(0, 1);
    int twistSum = 0, flipSum = 0;
    for (int i = 0; i < 7;  i++) { s.co[i] = twist(rng); twistSum += s.co[i]; }
    for (int i = 0; i < 11; i++) { s.eo[i] = flip(rng);  flipSum  += s.eo[i]; }
    s.co[7]  = (3 - twistSum % 3) % 3;
    s.eo[11] = flipSum % 2;
    return s;
}

CubeState randomState() {
    return draw(defaultRng());
}

CubeState randomState(uint64_t seed) {
    std::mt19937_64 rng(seed);
    return draw(rng);
}

// A stack of axis runs, each holding the net quarter turns of its two faces
//
// Adjacent runs always differ in axis: a move joins the top run when the axis matches
// and starts a new one otherwise, and a run that cancels to nothing is popped so the
// next move can fall through to the run beneath it
std::vector<Move> canonicalize(const std::vector<Move>& moves) {
    struct Run { int axis; int turns[2]; };  // turns[0] for U R F, turns[1] for D L B
    std::vector<Run> runs;

    for (Move m : moves) {
        int face = static_cast<int>(m) / 3;
        int axis = face % 3, side = face / 3;
        int turns = static_cast<int>(m) % 3 + 1;  // X=1 X2=2 X'=3

        if (runs.empty() || runs.back().axis != axis) runs.push_back({axis, {0, 0}});
        Run& top = runs.back();
        top.turns[side] = (top.turns[side] + turns) % 4;
        if (top.turns[0] == 0 && top.turns[1] == 0) runs.pop_back();
    }

    std::vector<Move> result;
    for (const Run& run : runs) {
        for (int side = 0; side < 2; side++) {
            if (run.turns[side] == 0) continue;
            int face = side * 3 + run.axis;
            result.push_back(static_cast<Move>(face * 3 + run.turns[side] - 1));
        }
    }
    return result;
}

std::string sequenceName(const std::vector<Move>& moves) {
    std::string result;
    for (size_t i = 0; i < moves.size(); i++) {
        if (i > 0) result += ' ';
        result += moveName(moves[i]);
    }
    return result;
}
