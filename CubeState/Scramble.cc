#include "Scramble.h"
#include <random>
#include <stdexcept>

// Moves group three per face in the Move enum, so face = move / 3
// 0=U 1=R 2=F 3=D 4=L 5=B, opposite faces sit 3 apart

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

// One Engine Per Thread
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
