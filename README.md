# CubeAlgos

A 3×3×3 Rubik's Cube state model in C++17, with a terminal visualizer and an
interactive move prompt.

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
- A colored terminal visualizer in the standard cross layout
- An interactive prompt for driving the cube by hand
- A self-checking test suite that runs on startup

**Not in scope yet** — nothing below is implemented:

- Any solver (no IDA\*, no Kociemba/Thistlethwaite, no pruning tables)
- Coordinate encodings or move tables indexed by coordinate
- Scramble generation, or scramble/state import from a facelet string
- Slice moves (`M`, `E`, `S`), wide moves (`r`, `u`, …), cube rotations (`x`, `y`, `z`)
- Pattern databases, algorithm libraries, or optimal-solution search
- Any GUI — the visualizer is ANSI terminal output only

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
| Facelet rendering + terminal visualizer | done |
| Interactive prompt | done |
| Test suite | 25 tests, all passing |
| Solver | not started |

### Correctness work

The move tables and the renderer were each checked against external ground truth,
not just against themselves. Two bugs were found and fixed this way:

- **`F`/`F'` and `D`/`D'` had their edge-permutation arrays swapped with each
  other.** Those two faces turned their corners clockwise but their edges
  counter-clockwise. Every self-consistency test still passed, because each move
  was the exact inverse of the other — `F` followed by `F'` still solved the cube,
  quarter turns still had order 4, and opposite faces still commuted. The error only
  showed up against a known external result.

- **The renderer twisted corner stickers the wrong way.** A piece's sticker `j`
  lands in slot `(j + ori) % 3`, so slot `s` shows sticker `(s − ori)`, not
  `(s + ori)`. Every quarter turn of R/L/F/B drew its own face with wrong corner
  colors. Edges were unaffected, since they rotate mod 2 where both forms agree.

- **The renderer had the two back-face edge stickers swapped.** `BL` wrote to `B[3]`
  and `BR` to `B[5]`. In the cross layout `B` is unfolded to the *right* of `R`, so on
  the `B` face the R-side column is on the left and the L-side column is on the right —
  the two were the wrong way round. `B` and `B'` still rendered solid, so the
  face-solidity test never saw it; the symptom was `R` painting a white sticker into the
  middle of the back face instead of into its left column.

All 18 tables were then regenerated from face geometry and compared entry by
entry, and each bug got a dedicated regression test (see [Tests](#tests)). The whole
54-facelet net after every quarter turn is now pinned against an independent 3D sticker
model (each sticker carries a position and an outward normal; a turn rotates both),
which is what catches errors on the four side faces a turn writes into.

## Building

Requires a C++17 compiler and `make`.

```sh
make        # builds ./cubealgo
./cubealgo
```

Or directly:

```sh
g++ -std=c++17 -O2 -I. -o cubealgo Main.cc CubeState/CubeState.cc CubeState/MoveTable.cc
```

Running the binary executes the test suite, prints a few example states, and then
drops into interactive mode:

```
> R U R' U'
> reset
> quit
```

Enter any space-separated sequence in standard notation. `reset` (or `r`) returns
to solved, `quit` (or `q`) exits. Unrecognized tokens are reported without
changing the cube.

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

## Color scheme

The cubie model is orientation-agnostic — a piece is identified by its slot, never by a
color — so which color sits on which face is purely a rendering choice, made in
`FACE_BG` in `Main.cc`.

The visualizer shows the standard scheme held **yellow up, white down, blue front**:

| Face | U | D | F | B | R | L |
|---|---|---|---|---|---|---|
| Color | yellow | white | blue | green | red | orange |

That is the usual white-up / green-front cube turned 180° about the R–L axis, so `U`/`D`
and `F`/`B` swap colors while `R` and `L` keep theirs. To hold it a different way, permute
`FACE_BG` — nothing else depends on it.
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

`parseMove` and `parseSequence` throw `std::invalid_argument` on an unrecognized
token, so callers handling user input should wrap them in a `try`/`catch`.

## Tests

The suite runs automatically on startup — 25 tests covering the group structure
(move orders, commuting and non-commuting face pairs, permutation validity,
orientation-sum and parity invariants), the facelet rendering, and move parsing.

Two checks are load-bearing, because a move table wired up backwards still
satisfies every self-consistency test:

- **Superflip.** The canonical 20-move sequence must leave all eight corners solved
  and all twelve edges in place but flipped. This pins `cp`, `co`, `ep` and `eo`
  against an external result rather than against each other, and is the only test
  in the suite that catches the `F`/`D` bug described above.
- **Face solidity.** Turning a face permutes that face's own nine stickers among
  themselves, so it must still render as one solid color. This is what catches the
  corner-orientation bug in the renderer.
- **Net ground truth.** The full 54-facelet net after each of `U R F D L B` must match
  an external geometric model. Face solidity only checks the turning face, so it is
  blind to a facelet swapped between two of the four side faces — this test is not.

## Layout

```
Main.cc                  tests, facelet rendering, interactive prompt
CubeState/CubeState.h    Move enum, CubeState, parsing declarations
CubeState/CubeState.cc   state operations and notation parsing
CubeState/MoveTable.h    MoveTable struct
CubeState/MoveTable.cc   the 18 move tables
```

## Terminal requirements

The visualizer uses ANSI background colors, including a 256-color code for orange.
It renders correctly in most modern terminals; on Windows use Windows Terminal
rather than the legacy console host.
