#include <iostream>
#include <cassert>
#include "CubeState.h"
#include "Rotation.h"

#define PASS "\033[32m  PASS\033[0m "
#define FAIL "\033[31m  FAIL\033[0m "

static bool runTest(const char* name, bool condition) {
    std::cout << (condition ? PASS : FAIL) << name << "\n";
    return condition;
}

static bool isSame(const CubeState& a, const CubeState& b) {
    for (int i = 0; i < 8;  i++) if (a.cp[i]!=b.cp[i] || a.co[i]!=b.co[i]) return false;
    for (int i = 0; i < 12; i++) if (a.ep[i]!=b.ep[i] || a.eo[i]!=b.eo[i]) return false;
    return true;
}

int main() {
    int passed = 0, total = 0;
    CubeState s = CubeState::solved();

    // order 4
    total++; if (runTest("y x4 = identity", isSame(Rotation::y(Rotation::y(Rotation::y(Rotation::y(s)))), s))) passed++;
    total++; if (runTest("x x4 = identity", isSame(Rotation::x(Rotation::x(Rotation::x(Rotation::x(s)))), s))) passed++;
    total++; if (runTest("z x4 = identity", isSame(Rotation::z(Rotation::z(Rotation::z(Rotation::z(s)))), s))) passed++;

    // inverses
    total++; if (runTest("y then y' = identity", isSame(Rotation::yp(Rotation::y(s)), s))) passed++;
    total++; if (runTest("x then x' = identity", isSame(Rotation::xp(Rotation::x(s)), s))) passed++;
    total++; if (runTest("z then z' = identity", isSame(Rotation::zp(Rotation::z(s)), s))) passed++;

    // doubles
    total++; if (runTest("y2 = y+y",  isSame(Rotation::y2(s), Rotation::y(Rotation::y(s))))) passed++;
    total++; if (runTest("x2 = x+x",  isSame(Rotation::x2(s), Rotation::x(Rotation::x(s))))) passed++;
    total++; if (runTest("z2 = z+z",  isSame(Rotation::z2(s), Rotation::z(Rotation::z(s))))) passed++;

    // rotations dont affect solved state isSolved check
    total++; if (runTest("rotated solved cube is still solved", Rotation::x(Rotation::y(Rotation::z(s))).isSolved())) passed++;

    // commutators — opposite axis rotations commute
    total++; if (runTest("x2 y2 = y2 x2", isSame(Rotation::x2(Rotation::y2(s)), Rotation::y2(Rotation::x2(s))))) passed++;

    // faceToBottom — applying rotation puts correct center on bottom
    // center of D face (bottom) is always the D-face center piece
    // after faceToBottom(color), the piece that was on that color face should be on bottom
    // we verify by checking the rotation doesnt break isSolved on a solved cube
    for (int c = 0; c < 6; c++) {
        CubeState rotated = Rotation::faceToBottom(s, c);
        total++; if (runTest(("faceToBottom color " + std::to_string(c) + " preserves solved").c_str(),
            rotated.isSolved())) passed++;
    }

    // rotation then inverse on scrambled state
    {
        CubeState sc = s.apply(Move::R).apply(Move::U).apply(Move::Rp).apply(Move::F);
        total++; if (runTest("x then x' on scrambled = identity", isSame(Rotation::xp(Rotation::x(sc)), sc))) passed++;
        total++; if (runTest("y then y' on scrambled = identity", isSame(Rotation::yp(Rotation::y(sc)), sc))) passed++;
        total++; if (runTest("z then z' on scrambled = identity", isSame(Rotation::zp(Rotation::z(sc)), sc))) passed++;
    }

    // x y z relationships — x = z' y z etc (standard identities)
    total++; if (runTest("x = z' y z", isSame(Rotation::x(s), Rotation::z(Rotation::y(Rotation::zp(s)))))) passed++;

    std::cout << "\n" << (passed==total ? "\033[32m" : "\033[31m")
              << "  " << passed << "/" << total << " passed\033[0m\n";
    return passed == total ? 0 : 1;
}