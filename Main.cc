#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "CubeState/CubeState.h"

// ─── ANSI color codes ──────────────────────────────────────────────────────────
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

// Face color index -> ANSI background color.
//
// The cubie model is orientation-agnostic; which color sits on which face is purely
// a rendering choice. This is the standard scheme held yellow-up / blue-front, which
// is the whole cube turned 180° about the R-L axis from the usual white-up /
// green-front picture (so U<->D and F<->B swap colors, R and L keep theirs).
//
// 0=Yellow(U) 1=Red(R) 2=Blue(F) 3=White(D) 4=Orange(L) 5=Green(B)
static const char* FACE_BG[] = {
    "\033[43m",   // Yellow — U
    "\033[41m",   // Red    — R
    "\033[44m",   // Blue   — F
    "\033[47m",   // White  — D
    "\033[48;5;208m", // Orange — L (256-color)
    "\033[42m",   // Green  — B
};


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
//   FR=8:  F->5, R->3    FL=9:  F->3, L->5    BL=10: B->5, L->3    BR=11: B->3, R->5
// B is drawn unfolded to the right of R, so on the B face the R-side column is on
// the left (index 3) and the L-side column is on the right (index 5).

static void buildFacelets(const CubeState& s, uint8_t f[54]) {
    // Initialize all to face color (centers are fixed)
    // Centers: U=0,R=9,F=18,D=27,L=36,B=45 (index 4 of each face)
    for (int i = 0; i < 9;  i++) f[0+i]  = 0; // U face
    for (int i = 0; i < 9;  i++) f[9+i]  = 1; // R face
    for (int i = 0; i < 9;  i++) f[18+i] = 2; // F face
    for (int i = 0; i < 9;  i++) f[27+i] = 3; // D face
    for (int i = 0; i < 9;  i++) f[36+i] = 4; // L face
    for (int i = 0; i < 9;  i++) f[45+i] = 5; // B face

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
            // co counts clockwise twists, so the piece's sticker j lands in slot
            // (j + ori) % 3 — slot `sticker` therefore shows sticker (sticker - ori).
            int colorIdx = cornerColors[piece][(sticker + 3 - ori) % 3];
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
        {{50, 39}},  // BL  -> B[5], L[3]
        {{48, 14}},  // BR  -> B[3], R[5]
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

    // Test 13: every move is a valid permutation (bijective) on both cp and ep
    {
        bool allValid = true;
        for (int mi = 0; mi < 18; mi++) {
            CubeState s = CubeState::solved().apply(static_cast<Move>(mi));
            bool seenC[8] = {false};
            for (int i = 0; i < 8; i++) {
                if (s.cp[i] > 7 || seenC[s.cp[i]]) allValid = false;
                else seenC[s.cp[i]] = true;
            }
            bool seenE[12] = {false};
            for (int i = 0; i < 12; i++) {
                if (s.ep[i] > 11 || seenE[s.ep[i]]) allValid = false;
                else seenE[s.ep[i]] = true;
            }
        }
        total++; if (runTest("Every move's cp/ep is a valid permutation", allValid)) passed++;
    }

    // Test 14: every move actually touches 4 corners and 4 edges (no move is a no-op on a piece type)
    {
        bool allTouch4 = true;
        for (int mi = 0; mi < 18; mi++) {
            CubeState s = CubeState::solved().apply(static_cast<Move>(mi));
            int cornersMoved = 0, edgesMoved = 0;
            for (int i = 0; i < 8;  i++) if (s.cp[i] != i || s.co[i] != 0) cornersMoved++;
            for (int i = 0; i < 12; i++) if (s.ep[i] != i || s.eo[i] != 0) edgesMoved++;
            if (cornersMoved != 4 || edgesMoved != 4) allTouch4 = false;
        }
        total++; if (runTest("Every move affects exactly 4 corners and 4 edges", allTouch4)) passed++;
    }

    // Test 15: quarter turns have order 4, half turns have order 2, for all six faces
    {
        Move quarters[] = {Move::U, Move::R, Move::F, Move::D, Move::L, Move::B};
        Move halves[]   = {Move::U2, Move::R2, Move::F2, Move::D2, Move::L2, Move::B2};
        bool allOrder = true;
        for (Move m : quarters) {
            CubeState s = CubeState::solved();
            for (int i = 0; i < 4; i++) s = s.apply(m);
            if (!s.isSolved()) allOrder = false;
        }
        for (Move m : halves) {
            CubeState s = CubeState::solved().apply(m).apply(m);
            if (!s.isSolved()) allOrder = false;
        }
        total++; if (runTest("Quarter turns order 4, half turns order 2 (all faces)", allOrder)) passed++;
    }

    // Test 16: opposite faces commute (U/D, R/L, F/B in either order give the same result)
    {
        auto sameState = [](const CubeState& a, const CubeState& b) {
            for (int i = 0; i < 8;  i++) if (a.cp[i] != b.cp[i] || a.co[i] != b.co[i]) return false;
            for (int i = 0; i < 12; i++) if (a.ep[i] != b.ep[i] || a.eo[i] != b.eo[i]) return false;
            return true;
        };
        Move pairs[][2] = {{Move::U, Move::D}, {Move::R, Move::L}, {Move::F, Move::B}};
        bool allCommute = true;
        for (auto& p : pairs) {
            CubeState a = CubeState::solved().apply(p[0]).apply(p[1]);
            CubeState b = CubeState::solved().apply(p[1]).apply(p[0]);
            if (!sameState(a, b)) allCommute = false;
        }
        total++; if (runTest("Opposite faces commute (U/D, R/L, F/B)", allCommute)) passed++;
    }

    // Test 17: adjacent faces do NOT commute (catches tables that are accidentally no-ops or identical)
    {
        auto sameState = [](const CubeState& a, const CubeState& b) {
            for (int i = 0; i < 8;  i++) if (a.cp[i] != b.cp[i] || a.co[i] != b.co[i]) return false;
            for (int i = 0; i < 12; i++) if (a.ep[i] != b.ep[i] || a.eo[i] != b.eo[i]) return false;
            return true;
        };
        Move pairs[][2] = {{Move::U, Move::R}, {Move::R, Move::F}, {Move::F, Move::D}};
        bool noneCommute = true;
        for (auto& p : pairs) {
            CubeState a = CubeState::solved().apply(p[0]).apply(p[1]);
            CubeState b = CubeState::solved().apply(p[1]).apply(p[0]);
            if (sameState(a, b)) noneCommute = false;
        }
        total++; if (runTest("Adjacent faces do NOT commute (U/R, R/F, F/D)", noneCommute)) passed++;
    }

    // Test 18: random walk invariants — permutation parity of cp must equal parity of ep,
    // and orientation sums must stay 0 mod 3 (corners) / 0 mod 2 (edges) — true for any
    // sequence of legal moves since these are the group invariants of the physical cube.
    {
        auto parity = [](const uint8_t* p, int n) {
            std::vector<bool> visited(n, false);
            int swaps = 0;
            for (int i = 0; i < n; i++) {
                if (visited[i]) continue;
                int len = 0, j = i;
                while (!visited[j]) { visited[j] = true; j = p[j]; len++; }
                swaps += (len - 1);
            }
            return swaps % 2;
        };
        // deterministic pseudo-random sequence so the test is reproducible
        unsigned seed = 12345;
        auto nextMove = [&]() {
            seed = seed * 1103515245u + 12345u;
            return static_cast<Move>((seed / 65536u) % 18);
        };
        CubeState s = CubeState::solved();
        bool allInvariant = true;
        for (int step = 0; step < 200; step++) {
            s = s.apply(nextMove());
            int coSum = 0; for (int i = 0; i < 8;  i++) coSum += s.co[i];
            int eoSum = 0; for (int i = 0; i < 12; i++) eoSum += s.eo[i];
            if (coSum % 3 != 0 || eoSum % 2 != 0) allInvariant = false;
            if (parity(s.cp, 8) != parity(s.ep, 12)) allInvariant = false;
        }
        total++; if (runTest("Random walk preserves orientation-sum and permutation-parity invariants", allInvariant)) passed++;
    }

    // Test 19: superflip. The canonical 20-move optimal solution must leave every
    // corner solved and all 12 edges in place but flipped. This pins down cp, co, ep
    // and eo simultaneously against an external, well-known result — a self-consistent
    // but wrongly-wired move table cannot pass it.
    {
        CubeState s = CubeState::solved();
        for (auto m : parseSequence("U R2 F B R B2 R U2 L B2 R U' D' R2 F R' L B2 U2 F2"))
            s = s.apply(m);
        bool ok = true;
        for (int i = 0; i < 8;  i++) if (s.cp[i] != i || s.co[i] != 0) ok = false;
        for (int i = 0; i < 12; i++) if (s.ep[i] != i || s.eo[i] != 1) ok = false;
        total++; if (runTest("Superflip = corners solved, all 12 edges flipped in place", ok)) passed++;
    }

    // Test 20: known algorithm orders. A move table whose edge cycle runs the wrong
    // way round still satisfies every self-consistency check, but changes these.
    {
        auto orderOf = [](const char* alg) {
            auto ms = parseSequence(alg);
            CubeState s = CubeState::solved();
            for (int k = 1; k <= 1000; k++) {
                for (auto m : ms) s = s.apply(m);
                if (s.isSolved()) return k;
            }
            return -1;
        };
        bool ok = orderOf("R U") == 105 && orderOf("R U'") == 63 &&
                  orderOf("R U2") == 30 && orderOf("R F") == 105 &&
                  orderOf("R U R' U' R' F R2 U' R' U' R U R' F'") == 2;  // T-perm
        total++; if (runTest("Known algorithm orders (R U)=105, (R U')=63, T-perm=2", ok)) passed++;
    }

    // Test 21: turning a face permutes that face's own nine stickers among themselves,
    // so the face must still render as one solid color. Covers buildFacelets, which the
    // pure cubie-model tests above never exercise.
    {
        bool ok = true;
        for (int face = 0; face < 6; face++) {
            for (int k = 0; k < 3; k++) {
                CubeState s = CubeState::solved().apply(static_cast<Move>(face * 3 + k));
                uint8_t f[54];
                buildFacelets(s, f);
                for (int i = 0; i < 9; i++) if (f[face * 9 + i] != face) ok = false;
            }
        }
        total++; if (runTest("Turning a face leaves that face a solid color", ok)) passed++;
    }

    // Test 22: any reachable state has exactly nine stickers of each of the six colors.
    {
        unsigned seed = 2024;
        auto nextMove = [&]() {
            seed = seed * 1103515245u + 12345u;
            return static_cast<Move>((seed / 65536u) % 18);
        };
        CubeState s = CubeState::solved();
        bool ok = true;
        for (int step = 0; step < 500; step++) {
            s = s.apply(nextMove());
            uint8_t f[54];
            buildFacelets(s, f);
            int count[6] = {0, 0, 0, 0, 0, 0};
            for (int i = 0; i < 54; i++) { if (f[i] > 5) { ok = false; break; } count[f[i]]++; }
            for (int c = 0; c < 6; c++) if (count[c] != 9) ok = false;
            for (int face = 0; face < 6; face++) if (f[face * 9 + 4] != face) ok = false;
        }
        total++; if (runTest("Rendered cube always has 9 of each color, centers fixed", ok)) passed++;
    }

    // Test 23: a random sequence undone move-by-move in reverse returns to solved.
    {
        auto inverseOf = [](Move m) {
            int i = static_cast<int>(m), f = i / 3, k = i % 3;
            return static_cast<Move>(f * 3 + (k == 1 ? 1 : 2 - k));
        };
        unsigned seed = 777;
        auto nextMove = [&]() {
            seed = seed * 1103515245u + 12345u;
            return static_cast<Move>((seed / 65536u) % 18);
        };
        bool ok = true;
        for (int trial = 0; trial < 100 && ok; trial++) {
            std::vector<Move> seq;
            for (int i = 0; i < 20; i++) seq.push_back(nextMove());
            CubeState s = CubeState::solved();
            for (auto m : seq) s = s.apply(m);
            for (int i = static_cast<int>(seq.size()) - 1; i >= 0; i--) s = s.apply(inverseOf(seq[i]));
            if (!s.isSolved()) ok = false;
        }
        total++; if (runTest("Random sequences undone by their reverse-inverse", ok)) passed++;
    }

    // Test 24: parse errors are reported, not silently accepted.
    {
        bool ok = true;
        try { parseMove("X");  ok = false; } catch (const std::invalid_argument&) {}
        try { parseMove("r");  ok = false; } catch (const std::invalid_argument&) {}
        try { parseSequence("R U BAD"); ok = false; } catch (const std::invalid_argument&) {}
        if (!parseSequence("").empty())            ok = false;
        if (parseSequence("  R   U  ").size() != 2) ok = false;
        for (int i = 0; i < 18; i++) {
            Move m = static_cast<Move>(i);
            if (parseMove(moveName(m)) != m) ok = false;
        }
        total++; if (runTest("Move parsing round-trips and rejects bad tokens", ok)) passed++;
    }

    // Test 25: the rendered net after each clockwise quarter turn, pinned against an
    // external ground truth (an independent 3D sticker model: each sticker carries a
    // position and an outward normal, and a turn rotates both). Test 21 only checks
    // the turning face itself, so it never sees the four side faces the turn writes
    // into — a facelet swapped between two side faces passes everything else.
    {
        struct Case { const char* move; const char* net; };
        static const Case cases[6] = {
            {"U", "000000000555111111111222222333333333222444444444555555"},
            {"R", "002002002111111111223223223335335335444444444055055055"},
            {"F", "000000444011011011222222222111333333443443443555555555"},
            {"D", "000000000111111222222222444333333333444444555555555111"},
            {"L", "500500500111111111022022022233233233444444444553553553"},
            {"B", "111000000113113113222222222333333444044044044555555555"},
        };
        bool ok = true;
        for (const auto& c : cases) {
            CubeState s = CubeState::solved().apply(parseMove(c.move));
            uint8_t f[54];
            buildFacelets(s, f);
            for (int i = 0; i < 54; i++)
                if (f[i] != c.net[i] - '0') ok = false;
        }
        total++; if (runTest("Rendered net after U/R/F/D/L/B matches external ground truth", ok)) passed++;
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