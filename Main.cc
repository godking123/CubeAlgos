#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "CubeState/CubeState.h"

// ─── ANSI color codes ──────────────────────────────────────────────────────────
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

// Face color index → ANSI background color
// 0=White(U) 1=Red(R) 2=Green(F) 3=Yellow(D) 4=Orange(L) 5=Blue(B)
static const char* FACE_BG[] = {
    "\033[47m",   // White  — U
    "\033[41m",   // Red    — R
    "\033[42m",   // Green  — F
    "\033[43m",   // Yellow — D
    "\033[48;5;208m", // Orange — L (256-color)
    "\033[44m",   // Blue   — B
};

static const char* FACE_LABEL[] = { "U", "R", "F", "D", "L", "B" };

// ─── Facelet layout ────────────────────────────────────────────────────────────
// Convert CubeState (cubie model) to 54 facelets
// Facelet order: U(0-8) R(9-17) F(18-26) D(27-35) L(36-44) B(45-53)
// Each face: top-left to bottom-right, row by row
//
// Corner sticker layout (face, sticker index on corner):
//   URF=0: U->8, R->0, F->2   UFL=1: U->6, F->0, L->2
//   ULB=2: U->0, L->0, B->2   UBR=3: U->2, B->0, R->2 (corrected)
//   DFR=4: D->2, F->8, R->6   DLF=5: D->0, L->8, F->6
//   DBL=6: D->6, B->8, L->6   DRB=7: D->8, R->8, B->6 (corrected)
//
// Edge sticker layout:
//   UR=0:  U->5, R->1    UF=1:  U->7, F->1    UL=2:  U->3, L->1    UB=3:  U->1, B->1
//   DR=4:  D->5, R->7    DF=5:  D->1, F->7    DL=6:  D->3, L->7    DB=7:  D->7, B->7
//   FR=8:  F->5, R->3    FL=9:  F->3, L->5    BL=10: B->3, L->3    BR=11: B->5, R->5

static void buildFacelets(const CubeState& s, uint8_t f[54]) {
    // Initialize all to face color (centers are fixed)
    // Centers: U=0,R=9,F=18,D=27,L=36,B=45 (index 4 of each face)
    for (int i = 0; i < 9;  i++) f[0+i]  = 0; // U face default white
    for (int i = 0; i < 9;  i++) f[9+i]  = 1; // R face default red
    for (int i = 0; i < 9;  i++) f[18+i] = 2; // F face default green
    for (int i = 0; i < 9;  i++) f[27+i] = 3; // D face default yellow
    for (int i = 0; i < 9;  i++) f[36+i] = 4; // L face default orange
    for (int i = 0; i < 9;  i++) f[45+i] = 5; // B face default blue

    // Corner stickers: [corner_idx][3] = {face1_sticker_idx, face2_sticker_idx, face3_sticker_idx}
    // Each corner has 3 stickers. Orientation 0 = standard mapping.
    // Sticker face assignments per corner position:
    //   pos 0 (URF): faces U,R,F  stickers U[8], R[0], F[2]
    //   pos 1 (UFL): faces U,F,L  stickers U[6], F[0], L[2]
    //   pos 2 (ULB): faces U,L,B  stickers U[0], L[0], B[2]  (corrected UBR->ULB)
    //   pos 3 (UBR): faces U,B,R  stickers U[2], B[0], R[2]
    //   pos 4 (DFR): faces D,F,R  stickers D[2], F[8], R[6]
    //   pos 5 (DLF): faces D,L,F  stickers D[0], L[8], F[6]
    //   pos 6 (DBL): faces D,B,L  stickers D[6], B[8], L[6]  (corrected DBR->DBL)
    //   pos 7 (DRB): faces D,R,B  stickers D[8], R[8], B[6]

    struct CornerFacelets {
        int sticker[3]; // indices into f[54]
    };

    static const CornerFacelets cornerFacelets[8] = {
        {{ 8,  9, 20}},   // pos 0: URF -> U[8], R[0], F[2]
        {{ 6, 18, 38}},   // pos 1: UFL -> U[6], F[0], L[2]
        {{ 0, 36, 47}},   // pos 2: ULB -> U[0], L[0], B[2]
        {{ 2, 45, 11}},   // pos 3: UBR -> U[2], B[0], R[2]
        {{29, 26, 15}},   // pos 4: DFR -> D[2], F[8], R[6]
        {{27, 44, 24}},   // pos 5: DLF -> D[0], L[8], F[6]
        {{33, 53, 42}},   // pos 6: DBL -> D[6], B[8], L[6]
        {{35, 17, 51}},   // pos 7: DRB -> D[8], R[8], B[6]
    };

    // Face colors per piece: which face color each sticker of the solved piece shows
    static const uint8_t cornerColors[8][3] = {
        {0,1,2}, // URF: U,R,F
        {0,2,4}, // UFL: U,F,L
        {0,4,5}, // ULB: U,L,B
        {0,5,1}, // UBR: U,B,R
        {3,2,1}, // DFR: D,F,R
        {3,4,2}, // DLF: D,L,F
        {3,5,4}, // DBL: D,B,L
        {3,1,5}, // DRB: D,R,B
    };

    for (int pos = 0; pos < 8; pos++) {
        int piece = s.cp[pos];
        int ori   = s.co[pos];
        for (int sticker = 0; sticker < 3; sticker++) {
            int colorIdx = cornerColors[piece][(sticker + ori) % 3];
            f[cornerFacelets[pos].sticker[sticker]] = colorIdx;
        }
    }

    // Edge stickers
    struct EdgeFacelets { int sticker[2]; };
    static const EdgeFacelets edgeFacelets[12] = {
        {{ 5, 10}},  // UR  -> U[5], R[1]
        {{ 7, 19}},  // UF  -> U[7], F[1]
        {{ 3, 37}},  // UL  -> U[3], L[1]
        {{ 1, 46}},  // UB  -> U[1], B[1]
        {{32, 16}},  // DR  -> D[5], R[7]
        {{28, 25}},  // DF  -> D[1], F[7]
        {{30, 43}},  // DL  -> D[3], L[7]
        {{34, 52}},  // DB  -> D[7], B[7]
        {{23, 12}},  // FR  -> F[5], R[3]
        {{21, 41}},  // FL  -> F[3], L[5]
        {{48, 39}},  // BL  -> B[3], L[3]
        {{50, 14}},  // BR  -> B[5], R[5]
    };

    static const uint8_t edgeColors[12][2] = {
        {0,1},{0,2},{0,4},{0,5},  // UR UF UL UB
        {3,1},{3,2},{3,4},{3,5},  // DR DF DL DB
        {2,1},{2,4},{5,4},{5,1},  // FR FL BL BR
    };

    for (int pos = 0; pos < 12; pos++) {
        int piece = s.ep[pos];
        int ori   = s.eo[pos];
        for (int sticker = 0; sticker < 2; sticker++) {
            int colorIdx = edgeColors[piece][(sticker + ori) % 2];
            f[edgeFacelets[pos].sticker[sticker]] = colorIdx;
        }
    }
}

// ─── Terminal visualizer ───────────────────────────────────────────────────────
// Prints the cube in cross layout:
//
//         U U U
//         U U U
//         U U U
//   L L L F F F R R R B B B
//   L L L F F F R R R B B B
//   L L L F F F R R R B B B
//         D D D
//         D D D
//         D D D

static void printCell(uint8_t colorIdx) {
    std::cout << FACE_BG[colorIdx] << "   " << RESET;
}

static void printCube(const CubeState& s) {
    uint8_t f[54];
    buildFacelets(s, f);

    // Helper: print a row of 3 cells from face starting at offset
    auto row = [&](int faceOffset, int rowIdx) {
        for (int c = 0; c < 3; c++)
            printCell(f[faceOffset + rowIdx * 3 + c]);
    };

    std::cout << "\n";

    // Top face (U) — indented by 9 cells
    for (int r = 0; r < 3; r++) {
        std::cout << "            "; // indent (3 cells * 3 chars + spacing)
        row(0, r);
        std::cout << "\n";
    }

    std::cout << "\n";

    // Middle band: L F R B
    for (int r = 0; r < 3; r++) {
        row(36, r); std::cout << " ";  // L
        row(18, r); std::cout << " ";  // F
        row(9,  r); std::cout << " ";  // R
        row(45, r);                    // B
        std::cout << "\n";
    }

    std::cout << "\n";

    // Bottom face (D) — indented
    for (int r = 0; r < 3; r++) {
        std::cout << "            ";
        row(27, r);
        std::cout << "\n";
    }

    std::cout << "\n";
}

// ─── Tests ────────────────────────────────────────────────────────────────────
static bool runTest(const char* name, bool condition) {
    if (condition) {
        std::cout << BOLD << "\033[32m  PASS\033[0m " << name << "\n";
    } else {
        std::cout << BOLD << "\033[31m  FAIL\033[0m " << name << "\n";
    }
    return condition;
}

static void runAllTests() {
    std::cout << BOLD << "\n=== Cube State Tests ===\n\n" << RESET;
    int passed = 0, total = 0;

    // Test 1: solved state
    {
        CubeState s = CubeState::solved();
        total++; if (runTest("solved() is solved", s.isSolved())) passed++;
    }

    // Test 2: R then R' = solved
    {
        CubeState s = CubeState::solved();
        s = s.apply(Move::R).apply(Move::Rp);
        total++; if (runTest("R then R' = solved", s.isSolved())) passed++;
    }

    // Test 3: U then U' = solved
    {
        CubeState s = CubeState::solved();
        s = s.apply(Move::U).apply(Move::Up);
        total++; if (runTest("U then U' = solved", s.isSolved())) passed++;
    }

    // Test 4: F then F' = solved
    {
        CubeState s = CubeState::solved();
        s = s.apply(Move::F).apply(Move::Fp);
        total++; if (runTest("F then F' = solved", s.isSolved())) passed++;
    }

    // Test 5: U4 = solved
    {
        CubeState s = CubeState::solved();
        for (int i = 0; i < 4; i++) s = s.apply(Move::U);
        total++; if (runTest("U x4 = solved", s.isSolved())) passed++;
    }

    // Test 6: R4 = solved
    {
        CubeState s = CubeState::solved();
        for (int i = 0; i < 4; i++) s = s.apply(Move::R);
        total++; if (runTest("R x4 = solved", s.isSolved())) passed++;
    }

    // Test 7: Sexy move x6 = solved (R U R' U')
    {
        CubeState s = CubeState::solved();
        for (int i = 0; i < 6; i++)
            s = s.apply(Move::R).apply(Move::U).apply(Move::Rp).apply(Move::Up);
        total++; if (runTest("Sexy move (R U R' U') x6 = solved", s.isSolved())) passed++;
    }

    // Test 8: Sune x6 = solved (R U R' U R U2 R')
    {
        CubeState s = CubeState::solved();
        for (int i = 0; i < 6; i++) {
            s = s.apply(Move::R).apply(Move::U).apply(Move::Rp)
                 .apply(Move::U).apply(Move::R).apply(Move::U2).apply(Move::Rp);
        }
        total++; if (runTest("Sune (R U R' U R U2 R') x6 = solved", s.isSolved())) passed++;
    }

    // Test 9: U2 = U applied twice
    {
        CubeState a = CubeState::solved().apply(Move::U2);
        CubeState b = CubeState::solved().apply(Move::U).apply(Move::U);
        bool match = true;
        for (int i = 0; i < 8;  i++) if (a.cp[i] != b.cp[i] || a.co[i] != b.co[i]) match = false;
        for (int i = 0; i < 12; i++) if (a.ep[i] != b.ep[i] || a.eo[i] != b.eo[i]) match = false;
        total++; if (runTest("U2 == U+U", match)) passed++;
    }

    // Test 10: R2 = R applied twice
    {
        CubeState a = CubeState::solved().apply(Move::R2);
        CubeState b = CubeState::solved().apply(Move::R).apply(Move::R);
        bool match = true;
        for (int i = 0; i < 8;  i++) if (a.cp[i] != b.cp[i] || a.co[i] != b.co[i]) match = false;
        for (int i = 0; i < 12; i++) if (a.ep[i] != b.ep[i] || a.eo[i] != b.eo[i]) match = false;
        total++; if (runTest("R2 == R+R", match)) passed++;
    }

    // Test 11: scramble is not solved
    {
        CubeState s = CubeState::solved();
        auto seq = parseSequence("R U R' F2 L D2 B' U2 R F");
        for (auto m : seq) s = s.apply(m);
        total++; if (runTest("Scramble is not solved", !s.isSolved())) passed++;
    }

    // Test 12: all 18 moves then their inverses = solved
    {
        Move pairs[][2] = {
            {Move::U,Move::Up},{Move::U2,Move::U2},{Move::R,Move::Rp},
            {Move::R2,Move::R2},{Move::F,Move::Fp},{Move::F2,Move::F2},
            {Move::D,Move::Dp},{Move::D2,Move::D2},{Move::L,Move::Lp},
            {Move::L2,Move::L2},{Move::B,Move::Bp},{Move::B2,Move::B2}
        };
        bool allPass = true;
        for (auto& p : pairs) {
            CubeState s = CubeState::solved().apply(p[0]).apply(p[1]);
            if (!s.isSolved()) allPass = false;
        }
        total++; if (runTest("All moves followed by inverse = solved", allPass)) passed++;
    }

    std::cout << "\n" << BOLD;
    if (passed == total)
        std::cout << "\033[32m  All " << total << "/" << total << " tests passed\033[0m\n";
    else
        std::cout << "\033[31m  " << passed << "/" << total << " tests passed\033[0m\n";
    std::cout << RESET << "\n";
}

// ─── Main ─────────────────────────────────────────────────────────────────────
int main() {
    // Run tests
    runAllTests();

    // Show solved cube
    std::cout << BOLD << "=== Solved Cube ===" << RESET << "\n";
    CubeState solved = CubeState::solved();
    printCube(solved);

    // Show cube after R move
    std::cout << BOLD << "=== After R ===" << RESET << "\n";
    printCube(solved.apply(Move::R));

    // Show cube after U move
    std::cout << BOLD << "=== After U ===" << RESET << "\n";
    printCube(solved.apply(Move::U));

    // Show cube after scramble
    std::cout << BOLD << "=== After R U R' F2 L D2 B' U2 ===" << RESET << "\n";
    CubeState scrambled = solved;
    for (auto m : parseSequence("R U R' F2 L D2 B' U2"))
        scrambled = scrambled.apply(m);
    printCube(scrambled);

    // Interactive mode
    std::cout << BOLD << "=== Interactive Mode ===" << RESET << "\n";
    std::cout << "Enter moves (e.g. R U R' F2) or 'reset' or 'quit':\n\n";

    CubeState current = CubeState::solved();
    printCube(current);

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "quit" || line == "q") break;
        if (line == "reset" || line == "r") {
            current = CubeState::solved();
            std::cout << "Reset to solved.\n";
            printCube(current);
            continue;
        }
        try {
            auto moves = parseSequence(line);
            for (auto m : moves) current = current.apply(m);
            printCube(current);
            if (current.isSolved())
                std::cout << "\033[32m" << BOLD << "  Cube is solved!\033[0m\n\n";
        } catch (const std::exception& e) {
            std::cout << "\033[31mError: " << e.what() << "\033[0m\n";
            std::cout << "Valid moves: U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'\n\n";
        }
    }

    return 0;
}