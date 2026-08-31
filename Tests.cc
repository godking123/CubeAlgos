#include "CubeState/CubeAlgos.h"

// ─── ANSI color codes ──────────────────────────────────────────────────────────
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

// Returns true when every test passed, so the binary's exit status can carry the
// result to `make test` and to CI.
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


    // Test 21: a random sequence undone move-by-move in reverse returns to solved.
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

    // Test 22: parse errors are reported, not silently accepted.
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

    // Test 41: pruning table is admissible — for any reachable state, the heuristic
    // never exceeds the true distance to G1. We verify on the superflip (known 20
    // moves from solved, but only ~6 from G1) and the solved state.
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
        // solved is already G1 so heuristic must be 0
        // superflip has all edges flipped so heuristic must be > 0
        bool ok = (h_sol == 0) && (h_sf > 0);
        total++; if (runTest("Heuristic = 0 at G1, > 0 for superflip", ok)) passed++;
    }

    // Test 42: coordinate move tables are consistent — applying a move twice via
    // table gives same result as applying the double move table entry directly
    {
        bool ok = true;
        // check for U: flipMove[x][U] applied twice should equal flipMove[x][U2]
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

    // Test 43: a scramble has the requested length, uses only legal moves, and is
    // reproducible — the same seed must give the same sequence.
    {
        auto a = randomScramble(25, 777);
        auto b = randomScramble(25, 777);
        auto c = randomScramble(25, 778);
        bool ok = (a.size() == 25) && (a == b) && (a != c);
        for (Move m : a)
            if (static_cast<int>(m) < 0 || static_cast<int>(m) >= 18) ok = false;
        total++; if (runTest("Scramble: length, determinism by seed, legal moves", ok)) passed++;
    }

    // Test 44: scrambles are canonical — no two consecutive moves on the same face,
    // and no `R L R` triple, where the outer two commute past the middle one and the
    // sequence is really one move shorter than it counts.
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

    // Test 45: scrambles actually scramble, and round-trip through notation. A
    // 25-move canonical sequence cannot cancel back to solved, and printing it then
    // reparsing it must give the same moves back.
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

    // Test 46: edge cases — length 0 is the empty sequence (and the identity),
    // length 1 is a single move, and a negative length is rejected.
    {
        bool ok = randomScramble(0, 1).empty() && sequenceName({}).empty()
               && randomScramble(1, 1).size() == 1;
        try { randomScramble(-1, 1); ok = false; }
        catch (const std::invalid_argument&) { /* expected */ }
        total++; if (runTest("Scramble edge cases (length 0, 1, negative throws)", ok)) passed++;
    }

    std::cout << "\n" << BOLD;
    if (passed == total)
        std::cout << "\033[32m  All " << total << "/" << total << " tests passed\033[0m\n";
    else
        std::cout << "\033[31m  " << passed << "/" << total << " tests passed\033[0m\n";
    std::cout << RESET << "\n";
    return passed == total;
}

// ─── Entry point ──────────────────────────────────────────────────────────────
int main() {
    buildCoordTables();
    buildPruningTables();
    return runAllTests() ? 0 : 1;
}
