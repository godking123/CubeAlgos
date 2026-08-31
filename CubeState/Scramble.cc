#include "Scramble.h"
#include <random>
#include <stdexcept>

// moves are grouped three per face in the Move enum, so face = move / 3:
// 0=U 1=R 2=F 3=D 4=L 5=B, and opposite faces sit 3 apart

// a face is legal when it is not the previous face, and — if it is the opposite of
// the previous face — not the one before that either. the second rule drops R L R,
// which commutes down to R2 L and so is really one move shorter than it counts
static bool faceAllowed(int face, int prevFace, int prevPrevFace) {
    if (face == prevFace) return false;
    if (face == (prevFace + 3) % 6 && face == prevPrevFace) return false;
    return true;
}

// pick from the legal faces rather than drawing from all 18 and rejecting, so each
// move costs one draw. at most 2 of the 6 faces are ever excluded
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

// one engine per thread, seeded once. re-seeding per call from a clock hands out
// the same scramble to every call in a tight loop
std::vector<Move> randomScramble(int length) {
    static thread_local std::mt19937_64 rng(std::random_device{}());
    return generate(length, rng);
}

std::vector<Move> randomScramble(int length, uint64_t seed) {
    std::mt19937_64 rng(seed);
    return generate(length, rng);
}

std::string sequenceName(const std::vector<Move>& moves) {
    std::string result;
    for (size_t i = 0; i < moves.size(); i++) {
        if (i > 0) result += ' ';
        result += moveName(moves[i]);
    }
    return result;
}
