# CubeAlgos

A 3×3×3 Rubik's Cube solver in C++17. Kociemba's two-phase algorithm works today;
CFOP and Roux are stubbed out behind the same interface.

The cube is represented as a **cubie model** rather than 54 stickers: eight corner
pieces and twelve edge pieces, each with a position and an orientation. Moves are
applied by table lookup, which keeps state transitions cheap enough to sit
underneath the search.

### Kociemba

Solving runs in two phases. Phase 1 reduces the cube to the subgroup
`G1 = <U, D, R2, L2, F2, B2>` — every piece oriented and the four slice edges back in
the slice — in at most 12 moves. Phase 2 then solves it inside G1, where only those
ten moves are legal. Each phase is an IDA\* search over packed coordinates, guided by
a pruning table built by breadth-first search out from the goal.

---

## Project scope

**In scope, and working today:**

- A compact cubie-model cube state (`cp`, `co`, `ep`, `eo`)
- All 18 half-turn-metric moves as lookup tables
- Applying moves and sequences; detecting the solved state
- Parsing and printing standard move notation
- Canonical random-move scramble generation, seeded or not
- Coordinate encodings for both phases, and move tables indexed by coordinate
- Pruning tables for both phases, built by BFS from the goal state
- A two-phase IDA\* solver (Kociemba) that solves any scramble
- A method table so the front end can offer several solving methods
- A self-checking test suite

**Not in scope yet** — nothing below is implemented:

- CFOP and Roux. Both are placeholders: the directories, headers and method-table
  entries exist, but `solve` returns nothing and the menu marks them unavailable
- Optimal solving. The two-phase split finds a good solution, not the shortest one;
  solutions land around 20–24 moves rather than the ≤20 an optimal solver guarantees
- Searching past the first solution found, which is the usual way to shorten it
- State import from a facelet string
- Slice moves (`M`, `E`, `S`), wide moves (`r`, `u`, …), cube rotations (`x`, `y`, `z`)
- Pattern databases, algorithm libraries, or optimal-solution search
- Any GUI, or any rendering of the cube — `Viz/cubespin` is a separate program

**Non-goals:** cube sizes other than 3×3×3, and speed-optimized bitboard or
SIMD representations. The model favors being readable and obviously correct over
being fast.

## Status

The solver is complete and verified end to end. Current work so far:

| Area | State |
|---|---|
| `CubeState` struct and solved/apply/isSolved | done |
| All 18 move tables | done, verified against face geometry |
| Notation parsing (`parseMove`, `parseSequence`, `moveName`) | done, throws on bad input |
| Scramble generation (`randomScramble`) | done |
| Phase 1 coordinates, move tables, pruning tables | done |
| Phase 2 coordinates, move tables, pruning tables | done |
| `Phase1::solve` / `Phase2::solve` (IDA\*) | done |
| `Kociemba::solve` (both phases joined) | done |
| Method table and `cubealgo` method menu | done |
| CFOP | placeholder only |
| Roux | placeholder only |
| Test suite | 75 tests, all passing |
| Optimal solver | not started |

### Correctness work

The move tables were checked against external ground truth, not just against
themselves. Three bugs were found this way. The first is still guarded by a test; the
other two lived in the facelet net, which has since been removed along with the
terminal visualizer it fed, and are kept here as a record of how they were found:

- **`F`/`F'` and `D`/`D'` had their edge-permutation arrays swapped with each
  other.** Those two faces turned their corners clockwise but their edges
  counter-clockwise. Every self-consistency test still passed, because each move
  was the exact inverse of the other — `F` followed by `F'` still solved the cube,
  quarter turns still had order 4, and opposite faces still commuted. The error only
  showed up against a known external result.

- **The facelet net twisted corner stickers the wrong way.** A piece's sticker `j`
  lands in slot `(j + ori) % 3`, so slot `s` shows sticker `(s − ori)`, not
  `(s + ori)`. Every quarter turn of R/L/F/B drew its own face with wrong corner
  colors. Edges were unaffected, since they rotate mod 2 where both forms agree.

- **The facelet net had the two back-face edge stickers swapped.** `BL` wrote to `B[3]`
  and `BR` to `B[5]`. In the cross layout `B` is unfolded to the *right* of `R`, so on
  the `B` face the R-side column is on the left and the L-side column is on the right —
  the two were the wrong way round. `B` and `B'` still rendered solid, so the
  face-solidity test never saw it; the symptom was `R` painting a white sticker into the
  middle of the back face instead of into its left column.

All 18 tables were then regenerated from face geometry and compared entry by entry.
The two facelet bugs were caught by pinning the whole 54-facelet net after every
quarter turn against an independent 3D sticker model (each sticker carries a position
and an outward normal; a turn rotates both) — that check retired with the net itself,
but the corrected move tables it validated are what the suite still runs on.

## Building

Requires a C++17 compiler and `make`.

There are two binaries. They share the same state layer and differ only in which
file supplies `main()`.

```sh
make        # builds both ./cubealgo and ./tests
make test   # builds ./tests and runs it
```

`./cubealgo` builds every method's tables, asks which method to solve with, then
scrambles and solves on each ENTER:

```
$ ./cubealgo
Building tables...
Ready.

Rubik's Cube Solver
───────────────────
    1) Kociemba — two-phase IDA* search, 20-24 moves
    2) CFOP — cross, F2L, OLL, PLL (not implemented)
    3) Roux — blocks, CMLL, LSE (not implemented)

Method number: 1

Solving with Kociemba.
Press ENTER to generate a new scramble and solve it.
Type 'm' to change method, 'q' to quit.

[ Press ENTER ]
Scramble: L2 D B F2 R2 U2 L2 B' D2 U L F U D2 B D L' U' L2 R
Solving with Kociemba...
Method:   Kociemba
Solution: B D' R' U' L2 F D' F2 D' R U2 L2 F2 D2 R2 F2 U' B2 L2 D' B2 U2 L2 B2
Moves:    24
Time:     11.7942 ms
Solved:   YES
```

Picking a method that is not implemented leaves the current one in place and says so.

`./tests` builds every table and runs the suite — one line per test plus a summary.
It exits non-zero when a test fails, so `make test` fails the build with it.

## Move notation

The eighteen half-turn-metric moves are supported:

| | Clockwise | Half turn | Counter-clockwise |
|---|---|---|---|
| Up | `U` | `U2` | `U'` |
| Right | `R` | `R2` | `R'` |
| Front | `F` | `F2` | `F'` |
| Down | `D` | `D2` | `D'` |
| Left | `L` | `L2` | `L'` |
| Back | `B` | `B2` | `B'` |

Notation is case-sensitive: `r` is rejected rather than silently read as `R`.

## State representation

`CubeState` holds four arrays:

| Field | Size | Meaning |
|---|---|---|
| `cp` | 8 | corner permutation — which corner piece sits in each corner slot |
| `co` | 8 | corner orientation, 0–2 (clockwise twists) |
| `ep` | 12 | edge permutation |
| `eo` | 12 | edge orientation, 0–1 (flipped or not) |

Slot indices are fixed:

```
Corners: URF=0 UFL=1 ULB=2 UBR=3 DFR=4 DLF=5 DBL=6 DRB=7
Edges:   UR=0  UF=1  UL=2  UB=3  DR=4  DF=5  DL=6  DB=7
         FR=8  FL=9  BL=10 BR=11
```

Orientation follows the usual convention: `U`/`D` turns twist no corners, and only
`F`/`B` turns flip edges.

### Applying a move

`MOVE_TABLES[m]` stores, for each destination slot, the slot its piece comes from,
plus the orientation delta:

```cpp
next.cp[i] = cp[t.cp[i]];
next.co[i] = (co[t.cp[i]] + t.co[i]) % 3;
```

## API

```cpp
#include "CubeState/CubeState.h"

CubeState s = CubeState::solved();
s = s.apply(Move::R).apply(Move::U);

for (Move m : parseSequence("R U R' U'"))
    s = s.apply(m);

if (s.isSolved()) { /* ... */ }
```

| Function | Behavior |
|---|---|
| `CubeState::solved()` | the identity state |
| `CubeState::apply(Move)` | returns a new state; does not mutate |
| `CubeState::isSolved()` | true when every piece is home and unoriented |
| `moveName(Move)` | move → notation string |
| `parseMove(const std::string&)` | notation → move; throws `std::invalid_argument` |
| `parseSequence(const std::string&)` | whitespace-separated notation → `std::vector<Move>` |
| `sequenceName(const std::vector<Move>&)` | sequence → space-separated notation |
| `randomScramble(int length = 25)` | a canonical random scramble; seeded from `std::random_device` |
| `randomScramble(int length, uint64_t seed)` | the same, reproducible for a given seed |

`parseMove` and `parseSequence` throw `std::invalid_argument` on an unrecognized
token, so callers handling user input should wrap them in a `try`/`catch`.

### Solving

Each method exposes `buildTables()` and `solve()`. Build once at startup, then solve:

```cpp
#include "CubeState/CubeAlgos.h"
#include "Solvers/Kociemba/Kociemba.h"

Kociemba::buildTables();

CubeState s = /* a scrambled cube */;
for (Move m : Kociemba::solve(s)) s = s.apply(m);   // now solved
```

The Kociemba phases are also callable individually. Phase 2 assumes its input is
already in G1, so it must be given the state *after* phase 1 is applied.

| Function | Behavior |
|---|---|
| `Kociemba::buildTables()` | fills every coordinate and pruning table; call once |
| `Kociemba::solve(const CubeState&)` | both phases joined; solves any state |
| `Phase1::solve(const CubeState&)` | moves reducing any state to G1; ≤ 12 moves |
| `Phase2::solve(const CubeState&)` | moves solving a G1 state; G1 generators only |

All return an empty sequence when their goal is already met. `Kociemba::buildTables`
allocates and fills roughly 22 MB of static tables in well under a second.

### Adding a method

`Solvers/Method.h` defines the interface the front end talks to:

```cpp
struct Method {
    const char* name;
    const char* description;
    bool implemented;
    void (*buildTables)();
    std::vector<Move> (*solve)(const CubeState& s);
};
```

To add one: create `Solvers/<Name>/`, write `<Name>.h` and `<Name>.cc` exposing
`buildTables` and `solve` in a `<Name>` namespace, add a row to `METHODS` in
`Solvers/Method.cc`, and add the source to `SOLVERS` in the Makefile. `Main.cc`
needs no changes — it iterates the table.

`implemented` is what the menu gates on. Leave it `false` until the method solves;
a test asserts that placeholders return no moves, and another asserts that every
method claiming `implemented` actually solves.

## Tests

The suite is the `tests` binary — 75 tests covering the group structure (move
orders, commuting and non-commuting face pairs, permutation validity,
orientation-sum and parity invariants), both phases' coordinate encodings, move
tables and pruning tables, both solvers, the method table, scramble generation, and
move parsing. It runs in well under a second.

Several checks are load-bearing, because a move table wired up backwards still
satisfies every self-consistency test:

- **Superflip.** The canonical 20-move sequence must leave all eight corners solved
  and all twelve edges in place but flipped. This pins `cp`, `co`, `ep` and `eo`
  against an external result rather than against each other, and is the only test
  in the suite that catches the `F`/`D` bug described above.
- **Known algorithm orders.** `(R U)` has order 105, `(R U')` 63, `(R U2)` 30, and the
  T-perm order 2. A move table whose edge cycle runs the wrong way still has order 4,
  so these pin the cycle direction against published values.
- **End-to-end solve.** Twenty scrambles must come back solved after both phases.
  Every table, coordinate and search in the project feeds into this one assertion.
- **Heuristic admissibility.** A state reached in `n` moves must have a heuristic of
  at most `n`. IDA\* returns an optimal solution only if the heuristic never
  overestimates; an inadmissible one silently returns longer solutions instead of
  failing, so nothing else would catch it.

## Layout

```
Main.cc                  cubealgo — method menu, scramble-and-solve prompt
Tests.cc                 the test suite
Makefile

CubeState/               the state layer; knows nothing about solving
  CubeState.h/.cc          Move enum, CubeState, state ops, notation parsing
  MoveTable.h/.cc          the 18 move tables
  Scramble.h/.cc           scramble generation and sequence printing
  CubeAlgos.h              umbrella header for the state layer

Solvers/
  Method.h                 the interface Main talks to
  Method.cc                the METHODS table — one row per method
  Kociemba/                two-phase algorithm; works
    Coords.h/.cc             coordinate encode/decode for both phases
    CoordTables.h/.cc        move and pruning tables, indexed by coordinate
    Phase1.h/.cc             reduce to G1
    Phase2.h/.cc             solve within G1
    Kociemba.h/.cc           builds the tables, joins the two phases
  CFOP/
    CFOP.h/.cc               placeholder
  Roux/
    Roux.h/.cc               placeholder
```
