#include "CubeState/CubeAlgos.h"
#include "Solvers/Method.h"
#include "Solvers/Kociemba/Coords.h"
#include "Solvers/Kociemba/CoordTables.h"
#include "Solvers/Kociemba/Kociemba.h"
#include "Solvers/Kociemba/Phase1.h"
#include "Solvers/Kociemba/Phase2.h"

// ─── ANSI Color Codes ──────────────────────────────────────────────────────────
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

// ─── Tests ────────────────────────────────────────────────────────────────────
static bool runTest(const char* name, bool condition) {
    if (condition) {
        std::cout << BOLD << "\033[32m  PASS\033[0m " << name << "\n";
    } else {
        std::cout << BOLD << "\033[31m  FAIL\033[0m " << name << "\n";
    }
    return condition;
}

// True when every test passed, so the exit status carries the result to `make test`
static bool runAllTests() {
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

    // Test 14: every move touches 4 corners and 4 edges, no move is a no-op on a piece type
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

    // Test 17: adjacent faces do NOT commute, catching no-op or duplicated tables
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

    // Test 18: random walk invariants — cp parity equals ep parity, orientation sums stay
    // 0 mod 3 for corners and 0 mod 2 for edges
    // These hold for any legal sequence, they are the group invariants of the cube
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
        // Deterministic sequence so the test is reproducible
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

    // Test 19: superflip — the canonical 20-move solution leaves every corner solved and
    // all 12 edges in place but flipped
    // Pins cp, co, ep and eo at once against an external result, which a self-consistent
    // but wrongly-wired table cannot pass
    {
        CubeState s = CubeState::solved();
        for (auto m : parseSequence("U R2 F B R B2 R U2 L B2 R U' D' R2 F R' L B2 U2 F2"))
            s = s.apply(m);
        bool ok = true;
        for (int i = 0; i < 8;  i++) if (s.cp[i] != i || s.co[i] != 0) ok = false;
        for (int i = 0; i < 12; i++) if (s.ep[i] != i || s.eo[i] != 1) ok = false;
        total++; if (runTest("Superflip = corners solved, all 12 edges flipped in place", ok)) passed++;
    }

    // Test 20: known algorithm orders — an edge cycle running the wrong way still passes
    // every self-consistency check, but changes these
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


    // Test 21: a random sequence undone in reverse returns to solved
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

    // Test 22: parse errors are reported, not silently accepted
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

    // ─── Coord + Pruning Table Tests ──────────────────────────────────────────────

    // Test 23: encodeFlip of solved state = 0
    {
        CubeState s = CubeState::solved();
        total++; if (runTest("encodeFlip(solved) == 0", Coords::encodeFlip(s) == 0)) passed++;
    }

    // Test 24: encodeTwist of solved state = 0
    {
        CubeState s = CubeState::solved();
        total++; if (runTest("encodeTwist(solved) == 0", Coords::encodeTwist(s) == 0)) passed++;
    }

    // Test 25: encodeSlice of solved state = 0
    {
        CubeState s = CubeState::solved();
        total++; if (runTest("encodeSlice(solved) == 0", Coords::encodeSlice(s) == 0)) passed++;
    }

    // Test 26: encodeFlip range — every reachable flip value stays in [0, 2047]
    {
        bool ok = true;
        unsigned seed = 99;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 500; i++) {
            seed = seed * 1103515245u + 12345u;
            s = s.apply(static_cast<Move>((seed / 65536u) % 18));
            int f = Coords::encodeFlip(s);
            if (f < 0 || f > 2047) ok = false;
        }
        total++; if (runTest("encodeFlip always in [0, 2047]", ok)) passed++;
    }

    // Test 27: encodeTwist range — every reachable twist value stays in [0, 2186]
    {
        bool ok = true;
        unsigned seed = 42;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 500; i++) {
            seed = seed * 1103515245u + 12345u;
            s = s.apply(static_cast<Move>((seed / 65536u) % 18));
            int t = Coords::encodeTwist(s);
            if (t < 0 || t > 2186) ok = false;
        }
        total++; if (runTest("encodeTwist always in [0, 2186]", ok)) passed++;
    }

    // Test 28: encodeSlice range — every reachable slice value stays in [0, 494]
    {
        bool ok = true;
        unsigned seed = 77;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 500; i++) {
            seed = seed * 1103515245u + 12345u;
            s = s.apply(static_cast<Move>((seed / 65536u) % 18));
            int sl = Coords::encodeSlice(s);
            if (sl < 0 || sl > 494) ok = false;
        }
        total++; if (runTest("encodeSlice always in [0, 494]", ok)) passed++;
    }

    // Test 29: decodeFlip round-trip — encode then decode then re-encode matches
    {
        bool ok = true;
        unsigned seed = 55;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 200; i++) {
            seed = seed * 1103515245u + 12345u;
            s = s.apply(static_cast<Move>((seed / 65536u) % 18));
            int flip = Coords::encodeFlip(s);
            CubeState tmp = CubeState::solved();
            Coords::decodeFlip(tmp, flip);
            if (Coords::encodeFlip(tmp) != flip) ok = false;
        }
        total++; if (runTest("decodeFlip round-trip: encode→decode→encode matches", ok)) passed++;
    }

    // Test 30: decodeTwist round-trip
    {
        bool ok = true;
        unsigned seed = 33;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 200; i++) {
            seed = seed * 1103515245u + 12345u;
            s = s.apply(static_cast<Move>((seed / 65536u) % 18));
            int twist = Coords::encodeTwist(s);
            CubeState tmp = CubeState::solved();
            Coords::decodeTwist(tmp, twist);
            if (Coords::encodeTwist(tmp) != twist) ok = false;
        }
        total++; if (runTest("decodeTwist round-trip: encode→decode→encode matches", ok)) passed++;
    }

    // Test 31: decodeSlice round-trip
    {
        bool ok = true;
        unsigned seed = 11;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 200; i++) {
            seed = seed * 1103515245u + 12345u;
            s = s.apply(static_cast<Move>((seed / 65536u) % 18));
            int sl = Coords::encodeSlice(s);
            CubeState tmp = CubeState::solved();
            Coords::decodeSlice(tmp, sl);
            if (Coords::encodeSlice(tmp) != sl) ok = false;
        }
        total++; if (runTest("decodeSlice round-trip: encode→decode→encode matches", ok)) passed++;
    }

    // Test 32: superflip has flip=2047 (all edges flipped), twist=0, slice=0
    {
        CubeState s = CubeState::solved();
        for (auto m : parseSequence("U R2 F B R B2 R U2 L B2 R U' D' R2 F R' L B2 U2 F2"))
            s = s.apply(m);
        bool ok = Coords::encodeFlip(s)  == 2047
               && Coords::encodeTwist(s) == 0
               && Coords::encodeSlice(s) == 0;
        total++; if (runTest("Superflip: flip=2047, twist=0, slice=0", ok)) passed++;
    }

    // Test 33: flipMove table — applying move via table matches applying move directly
    {
        bool ok = true;
        unsigned seed = 22;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 300; i++) {
            seed = seed * 1103515245u + 12345u;
            int m = (seed / 65536u) % 18;
            int flip = Coords::encodeFlip(s);
            int expected = Coords::encodeFlip(s.apply(static_cast<Move>(m)));
            int fromTable = flipMove[flip][m];
            if (fromTable != expected) ok = false;
            s = s.apply(static_cast<Move>(m));
        }
        total++; if (runTest("flipMove table matches direct encodeFlip(apply(move))", ok)) passed++;
    }

    // Test 34: twistMove table — applying move via table matches applying move directly
    {
        bool ok = true;
        unsigned seed = 44;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 300; i++) {
            seed = seed * 1103515245u + 12345u;
            int m = (seed / 65536u) % 18;
            int twist = Coords::encodeTwist(s);
            int expected = Coords::encodeTwist(s.apply(static_cast<Move>(m)));
            int fromTable = twistMove[twist][m];
            if (fromTable != expected) ok = false;
            s = s.apply(static_cast<Move>(m));
        }
        total++; if (runTest("twistMove table matches direct encodeTwist(apply(move))", ok)) passed++;
    }

    // Test 35: sliceMove table — applying move via table matches applying move directly
    {
        bool ok = true;
        unsigned seed = 66;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 300; i++) {
            seed = seed * 1103515245u + 12345u;
            int m = (seed / 65536u) % 18;
            int sl = Coords::encodeSlice(s);
            int expected = Coords::encodeSlice(s.apply(static_cast<Move>(m)));
            int fromTable = sliceMove[sl][m];
            if (fromTable != expected) ok = false;
            s = s.apply(static_cast<Move>(m));
        }
        total++; if (runTest("sliceMove table matches direct encodeSlice(apply(move))", ok)) passed++;
    }

    // Test 36: pruneFlipSlice[0][0] == 0 (G1 needs 0 moves)
    {
        total++; if (runTest("pruneFlipSlice[0][0] == 0", pruneFlipSlice[0][0] == 0)) passed++;
    }

    // Test 37: pruneTwistSlice[0][0] == 0 (G1 needs 0 moves)
    {
        total++; if (runTest("pruneTwistSlice[0][0] == 0", pruneTwistSlice[0][0] == 0)) passed++;
    }

    // Test 38: pruning table values are all non-negative (no unvisited entries)
    {
        bool ok = true;
        for (int f = 0; f < 2048; f++)
            for (int s = 0; s < 495; s++)
                if (pruneFlipSlice[f][s] < 0) ok = false;
        total++; if (runTest("pruneFlipSlice fully filled (no -1 entries)", ok)) passed++;
    }

    // Test 39: pruneTwistSlice fully filled
    {
        bool ok = true;
        for (int t = 0; t < 2187; t++)
            for (int s = 0; s < 495; s++)
                if (pruneTwistSlice[t][s] < 0) ok = false;
        total++; if (runTest("pruneTwistSlice fully filled (no -1 entries)", ok)) passed++;
    }

    // Test 40: pruning table values never exceed 12 (known max for phase 1)
    {
        bool ok = true;
        for (int f = 0; f < 2048; f++)
            for (int s = 0; s < 495; s++)
                if (pruneFlipSlice[f][s] > 12) ok = false;
        for (int t = 0; t < 2187; t++)
            for (int s = 0; s < 495; s++)
                if (pruneTwistSlice[t][s] > 12) ok = false;
        total++; if (runTest("All pruning table values <= 12", ok)) passed++;
    }

    // Test 41: pruning table is admissible — the heuristic never exceeds the true
    // distance to G1, checked on solved and on superflip
    {
        CubeState sol = CubeState::solved();
        int h_sol = std::max(
            pruneFlipSlice[Coords::encodeFlip(sol)][Coords::encodeSlice(sol)],
            pruneTwistSlice[Coords::encodeTwist(sol)][Coords::encodeSlice(sol)]
        );
        CubeState superflip = CubeState::solved();
        for (auto m : parseSequence("U R2 F B R B2 R U2 L B2 R U' D' R2 F R' L B2 U2 F2"))
            superflip = superflip.apply(m);
        int h_sf = std::max(
            pruneFlipSlice[Coords::encodeFlip(superflip)][Coords::encodeSlice(superflip)],
            pruneTwistSlice[Coords::encodeTwist(superflip)][Coords::encodeSlice(superflip)]
        );
        // Solved is already G1, so 0; superflip has every edge flipped, so > 0
        bool ok = (h_sol == 0) && (h_sf > 0);
        total++; if (runTest("Heuristic = 0 at G1, > 0 for superflip", ok)) passed++;
    }

    // Test 42: coordinate move tables are consistent — a move applied twice via table
    // matches the double move entry
    {
        bool ok = true;
        // flipMove[x][U] applied twice must equal flipMove[x][U2]
        int U  = static_cast<int>(Move::U);
        int U2 = static_cast<int>(Move::U2);
        int R  = static_cast<int>(Move::R);
        int R2 = static_cast<int>(Move::R2);
        for (int f = 0; f < 2048; f++) {
            if (flipMove[flipMove[f][U]][U] != flipMove[f][U2]) ok = false;
            if (flipMove[flipMove[f][R]][R] != flipMove[f][R2]) ok = false;
        }
        for (int t = 0; t < 2187; t++) {
            if (twistMove[twistMove[t][U]][U] != twistMove[t][U2]) ok = false;
            if (twistMove[twistMove[t][R]][R] != twistMove[t][R2]) ok = false;
        }
        total++; if (runTest("Move tables: applying X twice == X2 for U and R", ok)) passed++;
    }

    // Test 43: a scramble has the requested length, uses legal moves only, and repeats
    // for the same seed
    {
        auto a = randomScramble(25, 777);
        auto b = randomScramble(25, 777);
        auto c = randomScramble(25, 778);
        bool ok = (a.size() == 25) && (a == b) && (a != c);
        for (Move m : a)
            if (static_cast<int>(m) < 0 || static_cast<int>(m) >= 18) ok = false;
        total++; if (runTest("Scramble: length, determinism by seed, legal moves", ok)) passed++;
    }

    // Test 44: scrambles are canonical — no two consecutive moves on one face, and no
    // `R L R` triple, which commutes down to R2 L and is really one move shorter
    {
        bool ok = true;
        for (uint64_t seed = 0; seed < 200; seed++) {
            auto sc = randomScramble(25, seed);
            for (size_t i = 1; i < sc.size(); i++) {
                int f = static_cast<int>(sc[i]) / 3, pf = static_cast<int>(sc[i - 1]) / 3;
                if (f == pf) ok = false;
                if (i >= 2) {
                    int ppf = static_cast<int>(sc[i - 2]) / 3;
                    if (f == (pf + 3) % 6 && f == ppf) ok = false;
                }
            }
        }
        total++; if (runTest("Scramble is canonical (no same-face or A B A repeats)", ok)) passed++;
    }

    // Test 45: scrambles actually scramble and round-trip through notation — a 25-move
    // canonical sequence cannot cancel back to solved
    {
        bool ok = true;
        for (uint64_t seed = 0; seed < 200; seed++) {
            auto sc = randomScramble(25, seed);
            CubeState st = CubeState::solved();
            for (Move m : sc) st = st.apply(m);
            if (st.isSolved()) ok = false;
            if (parseSequence(sequenceName(sc)) != sc) ok = false;
        }
        total++; if (runTest("Scramble leaves cube unsolved; notation round-trips", ok)) passed++;
    }

    // Test 46: edge cases — length 0 is empty, length 1 is one move, negative is rejected
    {
        bool ok = randomScramble(0, 1).empty() && sequenceName({}).empty()
               && randomScramble(1, 1).size() == 1;
        try { randomScramble(-1, 1); ok = false; }
        catch (const std::invalid_argument&) { /* expected */ }
        total++; if (runTest("Scramble edge cases (length 0, 1, negative throws)", ok)) passed++;
    }

    // ─── Phase 2 Coord + Table Tests ──────────────────────────────────────────────

    // The G1 generators <U, D, R2, L2, F2, B2>, the only moves phase 2 plays and the
    // only ones EP and SlicePerm are defined over
    static const int PHASE2_MOVES[] = {
        static_cast<int>(Move::U),  static_cast<int>(Move::U2), static_cast<int>(Move::Up),
        static_cast<int>(Move::D),  static_cast<int>(Move::D2), static_cast<int>(Move::Dp),
        static_cast<int>(Move::R2), static_cast<int>(Move::L2),
        static_cast<int>(Move::F2), static_cast<int>(Move::B2)
    };
    static const int PHASE2_MOVE_COUNT = 10;

    // Test 47: encodeCP of solved state = 0
    {
        CubeState s = CubeState::solved();
        total++; if (runTest("encodeCP(solved) == 0", Coords::encodeCP(s) == 0)) passed++;
    }

    // Test 48: encodeEP of solved state = 0
    {
        CubeState s = CubeState::solved();
        total++; if (runTest("encodeEP(solved) == 0", Coords::encodeEP(s) == 0)) passed++;
    }

    // Test 49: encodeSlicePerm of solved state = 0
    {
        CubeState s = CubeState::solved();
        total++; if (runTest("encodeSlicePerm(solved) == 0", Coords::encodeSlicePerm(s) == 0)) passed++;
    }

    // Test 50: encodeCP range stays in [0, 40319]
    {
        bool ok = true;
        unsigned seed = 101;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 500; i++) {
            seed = seed * 1103515245u + 12345u;
            s = s.apply(static_cast<Move>((seed / 65536u) % 18));
            int v = Coords::encodeCP(s);
            if (v < 0 || v > 40319) ok = false;
        }
        total++; if (runTest("encodeCP always in [0, 40319]", ok)) passed++;
    }

    // Test 51: encodeEP range stays in [0, 40319]
    {
        bool ok = true;
        unsigned seed = 202;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 500; i++) {
            seed = seed * 1103515245u + 12345u;
            s = s.apply(static_cast<Move>((seed / 65536u) % 18));
            int v = Coords::encodeEP(s);
            if (v < 0 || v > 40319) ok = false;
        }
        total++; if (runTest("encodeEP always in [0, 40319]", ok)) passed++;
    }

    // Test 52: encodeSlicePerm range stays in [0, 23]
    {
        bool ok = true;
        unsigned seed = 303;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 500; i++) {
            seed = seed * 1103515245u + 12345u;
            s = s.apply(static_cast<Move>((seed / 65536u) % 18));
            int v = Coords::encodeSlicePerm(s);
            if (v < 0 || v > 23) ok = false;
        }
        total++; if (runTest("encodeSlicePerm always in [0, 23]", ok)) passed++;
    }

    // Test 53: decodeCP round-trip — all 40320 values
    {
        bool ok = true;
        for (int v = 0; v < 40320; v++) {
            CubeState tmp = CubeState::solved();
            Coords::decodeCP(tmp, v);
            if (Coords::encodeCP(tmp) != v) { ok = false; break; }
        }
        total++; if (runTest("decodeCP round-trip: all 40320 values", ok)) passed++;
    }

    // Test 54: decodeEP round-trip — all 40320 values
    {
        bool ok = true;
        for (int v = 0; v < 40320; v++) {
            CubeState tmp = CubeState::solved();
            Coords::decodeEP(tmp, v);
            if (Coords::encodeEP(tmp) != v) { ok = false; break; }
        }
        total++; if (runTest("decodeEP round-trip: all 40320 values", ok)) passed++;
    }

    // Test 55: decodeSlicePerm round-trip — all 24 values
    {
        bool ok = true;
        for (int v = 0; v < 24; v++) {
            CubeState tmp = CubeState::solved();
            Coords::decodeSlicePerm(tmp, v);
            if (Coords::encodeSlicePerm(tmp) != v) { ok = false; break; }
        }
        total++; if (runTest("decodeSlicePerm round-trip: all 24 values", ok)) passed++;
    }

    // Test 56: cpMove table matches direct encodeCP(apply(move))
    {
        bool ok = true;
        unsigned seed = 88;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 300; i++) {
            seed = seed * 1103515245u + 12345u;
            int m = (seed / 65536u) % 18;
            int cp = Coords::encodeCP(s);
            int expected = Coords::encodeCP(s.apply(static_cast<Move>(m)));
            if (cpMove[cp][m] != expected) ok = false;
            s = s.apply(static_cast<Move>(m));
        }
        total++; if (runTest("cpMove table matches direct encodeCP(apply(move))", ok)) passed++;
    }

    // Test 57: epMove table matches direct encodeEP(apply(move))
    // G1 generators only: EP covers the eight U/D-layer edges, so a quarter turn of
    // R/F/L/B trades a slice edge for a U/D one and leaves the coordinate's domain
    // Those columns are meaningless, and phase 2 never plays them
    {
        bool ok = true;
        unsigned seed = 99;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 300; i++) {
            seed = seed * 1103515245u + 12345u;
            int m = PHASE2_MOVES[(seed / 65536u) % PHASE2_MOVE_COUNT];
            int ep = Coords::encodeEP(s);
            int expected = Coords::encodeEP(s.apply(static_cast<Move>(m)));
            if (epMove[ep][m] != expected) ok = false;
            s = s.apply(static_cast<Move>(m));
        }
        total++; if (runTest("epMove table matches direct encodeEP(apply(move))", ok)) passed++;
    }

    // Test 58: slicePermMove table matches direct encodeSlicePerm(apply(move))
    // Same restriction as Test 57, for the same reason
    {
        bool ok = true;
        unsigned seed = 111;
        CubeState s = CubeState::solved();
        for (int i = 0; i < 300; i++) {
            seed = seed * 1103515245u + 12345u;
            int m = PHASE2_MOVES[(seed / 65536u) % PHASE2_MOVE_COUNT];
            int sp = Coords::encodeSlicePerm(s);
            int expected = Coords::encodeSlicePerm(s.apply(static_cast<Move>(m)));
            if (slicePermMove[sp][m] != expected) ok = false;
            s = s.apply(static_cast<Move>(m));
        }
        total++; if (runTest("slicePermMove table matches direct encodeSlicePerm(apply(move))", ok)) passed++;
    }

    // Test 59: Phase 2 move tables are consistent — R2 applied twice via table = identity
    {
        bool ok = true;
        int R2 = static_cast<int>(Move::R2);
        int U2 = static_cast<int>(Move::U2);
        for (int cp = 0; cp < 40320; cp++) {
            if (cpMove[cpMove[cp][R2]][R2] != cp) ok = false;
            if (cpMove[cpMove[cp][U2]][U2] != cp) ok = false;
        }
        total++; if (runTest("Phase 2 cpMove: R2 and U2 applied twice = identity", ok)) passed++;
    }

    // Test 60: in a G1 state, Phase 2 restricted moves keep CP/EP/SlicePerm valid
    {
        // Solved is a valid G1 state, orientations zero and slice edges in the slice
        bool ok = true;
        CubeState s = CubeState::solved();
        for (int m : PHASE2_MOVES) {
            CubeState next = s.apply(static_cast<Move>(m));
            if (Coords::encodeCP(next) < 0 || Coords::encodeCP(next) > 40319) ok = false;
            if (Coords::encodeEP(next) < 0 || Coords::encodeEP(next) > 40319) ok = false;
            if (Coords::encodeSlicePerm(next) < 0 || Coords::encodeSlicePerm(next) > 23) ok = false;
        }
        total++; if (runTest("Phase 2 moves keep CP/EP/SlicePerm in valid range", ok)) passed++;
    }

    // ─── Phase 2 Pruning Table Tests ──────────────────────────────────────────────

    // Test 61: pruneCPSlicePerm fully filled
    {
        bool ok = true;
        for (int c = 0; c < 40320; c++)
            for (int s = 0; s < 24; s++)
                if (pruneCPSlicePerm[c][s] < 0) ok = false;
        total++; if (runTest("pruneCPSlicePerm fully filled (no -1 entries)", ok)) passed++;
    }

    // Test 62: pruneEPSlicePerm fully filled
    {
        bool ok = true;
        for (int e = 0; e < 40320; e++)
            for (int s = 0; s < 24; s++)
                if (pruneEPSlicePerm[e][s] < 0) ok = false;
        total++; if (runTest("pruneEPSlicePerm fully filled (no -1 entries)", ok)) passed++;
    }

    // Test 63: pruning table values never exceed 18 (known max for phase 2)
    {
        bool ok = true;
        for (int c = 0; c < 40320; c++)
            for (int s = 0; s < 24; s++)
                if (pruneCPSlicePerm[c][s] > 18) ok = false;
        for (int e = 0; e < 40320; e++)
            for (int s = 0; s < 24; s++)
                if (pruneEPSlicePerm[e][s] > 18) ok = false;
        total++; if (runTest("All phase 2 pruning table values <= 18", ok)) passed++;
    }

    // Test 64: the heuristic is 0 at solved and > 0 away from it — a table filled in the
    // wrong direction still looks full, so this pins the origin
    {
        CubeState sol = CubeState::solved();
        int h_sol = std::max(
            pruneCPSlicePerm[Coords::encodeCP(sol)][Coords::encodeSlicePerm(sol)],
            pruneEPSlicePerm[Coords::encodeEP(sol)][Coords::encodeSlicePerm(sol)]
        );
        CubeState g1 = CubeState::solved().apply(Move::R2).apply(Move::F2).apply(Move::U);
        int h_g1 = std::max(
            pruneCPSlicePerm[Coords::encodeCP(g1)][Coords::encodeSlicePerm(g1)],
            pruneEPSlicePerm[Coords::encodeEP(g1)][Coords::encodeSlicePerm(g1)]
        );
        bool ok = (h_sol == 0) && (h_g1 > 0);
        total++; if (runTest("Phase 2 heuristic = 0 at solved, > 0 after R2 F2 U", ok)) passed++;
    }

    // Test 65: the heuristic never overestimates — a state reached in n G1 moves is at
    // most n from solved, so an admissible heuristic must be <= n
    // An inadmissible one makes the search return non-optimal solutions
    {
        bool ok = true;
        unsigned seed = 555;
        for (int trial = 0; trial < 200; trial++) {
            CubeState s = CubeState::solved();
            int n = 1 + (trial % 8);
            for (int i = 0; i < n; i++) {
                seed = seed * 1103515245u + 12345u;
                s = s.apply(static_cast<Move>(PHASE2_MOVES[(seed / 65536u) % 10]));
            }
            int hv = std::max(
                pruneCPSlicePerm[Coords::encodeCP(s)][Coords::encodeSlicePerm(s)],
                pruneEPSlicePerm[Coords::encodeEP(s)][Coords::encodeSlicePerm(s)]
            );
            if (hv > n) ok = false;
        }
        total++; if (runTest("Phase 2 heuristic never overestimates true distance", ok)) passed++;
    }

    // ─── Solver Tests ─────────────────────────────────────────────────────────────

    // Test 66: phase 1 returns nothing for a state already in G1, recognising the goal at
    // depth 0 rather than walking away and back
    {
        auto sol = Phase1::solve(CubeState::solved());
        total++; if (runTest("Phase1::solve(solved) returns empty", sol.empty())) passed++;
    }

    // Test 67: phase 1 lands every scramble in G1 — orientations zero and the four slice
    // edges in the slice, the property the phase exists for
    {
        bool ok = true;
        for (uint64_t seed = 0; seed < 30; seed++) {
            CubeState s = CubeState::solved();
            for (Move m : randomScramble(20, seed)) s = s.apply(m);
            for (Move m : Phase1::solve(s)) s = s.apply(m);
            if (Coords::encodeTwist(s) != 0) ok = false;
            if (Coords::encodeFlip(s)  != 0) ok = false;
            if (Coords::encodeSlice(s) != 0) ok = false;
        }
        total++; if (runTest("Phase1::solve lands 30 scrambles in G1", ok)) passed++;
    }

    // Test 68: phase 1 never exceeds 12 moves, the known maximum
    // IDA* with an admissible heuristic is optimal, so a longer one means the heuristic
    // or the move tables are wrong
    {
        bool ok = true;
        for (uint64_t seed = 0; seed < 30; seed++) {
            CubeState s = CubeState::solved();
            for (Move m : randomScramble(20, seed)) s = s.apply(m);
            if (Phase1::solve(s).size() > 12) ok = false;
        }
        total++; if (runTest("Phase1::solve never exceeds 12 moves", ok)) passed++;
    }

    // Test 69: phase 2 returns nothing for the solved state
    {
        auto sol = Phase2::solve(CubeState::solved());
        total++; if (runTest("Phase2::solve(solved) returns empty", sol.empty())) passed++;
    }

    // Test 70: phase 2 plays G1 generators only — a quarter turn of R/F/L/B would leave
    // G1 and invalidate the coordinates it searches on
    {
        bool ok = true;
        for (uint64_t seed = 0; seed < 20; seed++) {
            CubeState s = CubeState::solved();
            for (Move m : randomScramble(20, seed)) s = s.apply(m);
            for (Move m : Phase1::solve(s)) s = s.apply(m);
            for (Move m : Phase2::solve(s)) {
                bool legal = false;
                for (int i = 0; i < 10; i++)
                    if (static_cast<int>(m) == PHASE2_MOVES[i]) legal = true;
                if (!legal) ok = false;
            }
        }
        total++; if (runTest("Phase2::solve uses only G1 generators", ok)) passed++;
    }

    // Test 71: the two phases together solve the cube, the end-to-end check every table,
    // coordinate and search feeds into
    {
        bool ok = true;
        for (uint64_t seed = 0; seed < 20; seed++) {
            CubeState s = CubeState::solved();
            for (Move m : randomScramble(20, seed)) s = s.apply(m);
            for (Move m : Phase1::solve(s)) s = s.apply(m);
            for (Move m : Phase2::solve(s)) s = s.apply(m);
            if (!s.isSolved()) ok = false;
        }
        total++; if (runTest("Phase 1 + Phase 2 solves 20 scrambles", ok)) passed++;
    }

    // Test 72: Kociemba::solve is the two phases joined — the combined sequence must solve
    // the cube when replayed, not just each half in turn
    {
        bool ok = true;
        for (uint64_t seed = 0; seed < 20; seed++) {
            CubeState s = CubeState::solved();
            for (Move m : randomScramble(20, seed)) s = s.apply(m);
            CubeState check = s;
            for (Move m : Kociemba::solve(s)) check = check.apply(m);
            if (!check.isSolved()) ok = false;
        }
        total++; if (runTest("Kociemba::solve solves 20 scrambles end to end", ok)) passed++;
    }

    // Test 73: forEachSolution returns solutions of exactly the length asked for, all
    // landing in G1
    // Kociemba::solve budgets phase 2 off that length, so a wrong length or a state short
    // of G1 corrupts every total it compares
    {
        bool ok = false, lengthsRight = true, allInG1 = true;
        CubeState s = CubeState::solved();
        for (Move m : randomScramble(20, 3)) s = s.apply(m);

        int shortest = (int)Phase1::solve(s).size();
        Phase1::forEachSolution(s, shortest, [&](const std::vector<Move>& p1) {
            ok = true;
            if ((int)p1.size() != shortest) lengthsRight = false;

            CubeState g1 = s;
            for (Move m : p1) g1 = g1.apply(m);
            if (Coords::encodeTwist(g1) != 0) allInG1 = false;
            if (Coords::encodeFlip(g1)  != 0) allInG1 = false;
            if (Coords::encodeSlice(g1) != 0) allInG1 = false;
            return true;
        });

        total++; if (runTest("Phase1::forEachSolution yields exact-length G1 solutions",
                             ok && lengthsRight && allInG1)) passed++;
    }

    // Test 74: forEachSolution finds nothing below the optimal length, and stops when the
    // callback says so
    // The first lets Kociemba::solve start at zero, the second lets it quit early
    {
        CubeState s = CubeState::solved();
        for (Move m : randomScramble(20, 5)) s = s.apply(m);
        int shortest = (int)Phase1::solve(s).size();

        bool foundTooShort = false;
        Phase1::forEachSolution(s, shortest - 1, [&](const std::vector<Move>&) {
            foundTooShort = true;
            return true;
        });

        int seen = 0;
        bool ranToEnd = Phase1::forEachSolution(s, shortest, [&](const std::vector<Move>&) {
            seen++;
            return false;  // stop at the first one
        });

        total++; if (runTest("Phase1::forEachSolution respects length and early stop",
                             !foundTooShort && seen == 1 && !ranToEnd)) passed++;
    }

    // Test 75: phase 2 honours its move budget — it gets one move less than the best total
    // so far, so overshooting would return a solution that lengthens the answer
    {
        bool ok = true;
        for (uint64_t seed = 0; seed < 10; seed++) {
            CubeState s = CubeState::solved();
            for (Move m : randomScramble(20, seed)) s = s.apply(m);
            for (Move m : Phase1::solve(s)) s = s.apply(m);

            int optimal = (int)Phase2::solve(s).size();
            if ((int)Phase2::solve(s, optimal).size() != optimal) ok = false;
            if (optimal > 0 && !Phase2::solve(s, optimal - 1).empty()) ok = false;
        }
        total++; if (runTest("Phase2::solve never exceeds its move budget", ok)) passed++;
    }

    // ─── Method Table Tests ───────────────────────────────────────────────────────

    // Test 76: every method in the table is wired up — a null pointer here crashes Main
    // the moment the method is picked
    {
        bool ok = (METHOD_COUNT == 3);
        for (int i = 0; i < METHOD_COUNT; i++) {
            if (METHODS[i].name == nullptr || METHODS[i].description == nullptr) ok = false;
            if (METHODS[i].buildTables == nullptr || METHODS[i].solve == nullptr) ok = false;
        }
        total++; if (runTest("Every method has name, description and both calls", ok)) passed++;
    }

    // Test 77: unimplemented methods return nothing rather than something wrong
    // Main gates on Method::implemented, so a placeholder that started returning moves
    // would be silently trusted
    {
        bool ok = true;
        CubeState s = CubeState::solved();
        for (Move m : randomScramble(20, 7)) s = s.apply(m);
        for (int i = 0; i < METHOD_COUNT; i++) {
            if (METHODS[i].implemented) continue;
            if (!METHODS[i].solve(s).empty()) ok = false;
        }
        total++; if (runTest("Placeholder methods return no moves", ok)) passed++;
    }

    // Test 78: every implemented method actually solves, the check a new method passes to
    // flip its implemented flag to true
    {
        bool ok = true;
        for (int i = 0; i < METHOD_COUNT; i++) {
            if (!METHODS[i].implemented) continue;
            for (uint64_t seed = 0; seed < 10; seed++) {
                CubeState s = CubeState::solved();
                for (Move m : randomScramble(20, seed)) s = s.apply(m);
                CubeState check = s;
                for (Move m : METHODS[i].solve(s)) check = check.apply(m);
                if (!check.isSolved()) ok = false;
            }
        }
        total++; if (runTest("Every implemented method solves 10 scrambles", ok)) passed++;
    }

    std::cout << "\n" << BOLD;
    if (passed == total)
        std::cout << "\033[32m  All " << total << "/" << total << " tests passed\033[0m\n";
    else
        std::cout << "\033[31m  " << passed << "/" << total << " tests passed\033[0m\n";
    std::cout << RESET << "\n";
    return passed == total;
}

// ─── Entry Point ──────────────────────────────────────────────────────────────
int main() {
    Kociemba::buildTables();
    return runAllTests() ? 0 : 1;
}
