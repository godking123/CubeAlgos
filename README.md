# CubeAlgos

A 3×3×3 Rubik's Cube state model in C++17.

The cube is represented as a **cubie model** rather than 54 stickers: eight corner
pieces and twelve edge pieces, each with a position and an orientation. Moves are
applied by table lookup, which keeps state transitions cheap enough to sit
underneath a search later on.

---

## Project scope

This is the **state layer** — the part a solver would be built on top of. It is
deliberately not a solver.

**In scope, and working today:**

- A compact cubie-model cube state (`cp`, `co`, `ep`, `eo`)
- All 18 half-turn-metric moves as lookup tables
- Applying moves and sequences; detecting the solved state
- Parsing and printing standard move notation
- Canonical random-move scramble generation, seeded or not
- A self-checking test suite that runs on startup

**Not in scope yet** — nothing below is implemented:

- Any solver (no IDA\*, no Kociemba/Thistlethwaite, no pruning tables)
- Coordinate encodings or move tables indexed by coordinate
- State import from a facelet string
- Slice moves (`M`, `E`, `S`), wide moves (`r`, `u`, …), cube rotations (`x`, `y`, `z`)
- Pattern databases, algorithm libraries, or optimal-solution search
- Any GUI, or any rendering of the cube — `Viz/cubespin` is a separate program

**Non-goals:** cube sizes other than 3×3×3, and speed-optimized bitboard or
SIMD representations. The model favors being readable and obviously correct over
being fast.

## Status

The state layer is complete and verified. Current work so far:

| Area | State |
|---|---|
| `CubeState` struct and solved/apply/isSolved | done |
| All 18 move tables | done, verified against face geometry |
| Notation parsing (`parseMove`, `parseSequence`, `moveName`) | done, throws on bad input |
| Scramble generation (`randomScramble`) | done |
| `cubealgo` CLI | prints scrambles |
| Test suite | 46 tests, all passing |
| Solver | not started |

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

`./cubealgo` prints scrambles, one per line:

```
$ ./cubealgo
B2 L D' U' F2 R U2 F R2 D2 L U2 R B' D2 R U2 D2 R' D' L2 B R L F2

$ ./cubealgo 3 8      # three scrambles, eight moves each
```

It takes an optional count and length (`./cubealgo --help`), writes nothing but the
scrambles to stdout, and is safe to pipe.

`./tests` builds the coordinate and pruning tables and runs the suite — one line per
test plus a summary. It exits non-zero when a test fails, so `make test` fails the
build with it.

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

## Tests

The suite is the `tests` binary — 46 tests covering the group structure (move
orders, commuting and non-commuting face pairs, permutation validity,
orientation-sum and parity invariants), coordinate encodings, the pruning tables,
scramble generation, and move parsing.

Two checks are load-bearing, because a move table wired up backwards still
satisfies every self-consistency test:

- **Superflip.** The canonical 20-move sequence must leave all eight corners solved
  and all twelve edges in place but flipped. This pins `cp`, `co`, `ep` and `eo`
  against an external result rather than against each other, and is the only test
  in the suite that catches the `F`/`D` bug described above.
- **Known algorithm orders.** `(R U)` has order 105, `(R U')` 63, `(R U2)` 30, and the
  T-perm order 2. A move table whose edge cycle runs the wrong way still has order 4,
  so these pin the cycle direction against published values.

## Layout

```
Main.cc                  cubealgo — the scramble-printing front end
Tests.cc                 the test suite
CubeState/CubeState.h    Move enum, CubeState, parsing declarations
CubeState/CubeState.cc   state operations and notation parsing
CubeState/MoveTable.h    MoveTable struct
CubeState/MoveTable.cc   the 18 move tables
CubeState/Scramble.h     scramble generation and sequence printing
CubeState/Scramble.cc
```
