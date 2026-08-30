#include "CoordTables.h"
#include "CubeState.h"
#include "Coords.h"

// actual memory allocated here once
int flipMove[2048][18];
int twistMove[2187][18];
int sliceMove[495][18];

void buildCoordTables() {
    // flip move table
    for (int flip = 0; flip < 2048; flip++) {
        CubeState s = CubeState::solved();
        Coords::decodeFlip(s, flip);
        for (int m = 0; m < 18; m++) {
            CubeState next = s.apply(static_cast<Move>(m));
            flipMove[flip][m] = Coords::encodeFlip(next);
        }
    }

    // twist move table
    for (int twist = 0; twist < 2187; twist++) {
        CubeState s = CubeState::solved();
        Coords::decodeTwist(s, twist);
        for (int m = 0; m < 18; m++) {
            CubeState next = s.apply(static_cast<Move>(m));
            twistMove[twist][m] = Coords::encodeTwist(next);
        }
    }

    // slice move table
    for (int slice = 0; slice < 495; slice++) {
        CubeState s = CubeState::solved();
        Coords::decodeSlice(s, slice);
        for (int m = 0; m < 18; m++) {
            CubeState next = s.apply(static_cast<Move>(m));
            sliceMove[slice][m] = Coords::encodeSlice(next);
        }
    }
}