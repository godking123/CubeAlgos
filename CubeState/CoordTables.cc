#include "CoordTables.h"
#include "CubeState.h"
#include "Coords.h"

// actual memory allocated here once
int flipMove[2048][18];
int twistMove[2187][18];
int sliceMove[495][18];

int pruneFlipSlice[2048][495];
int pruneTwistSlice[2187][495];

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

void buildPruningTables() {
    // flip prune table
    for (int f = 0; f < 2048; f++) {
            for (int s = 0; s < 495; s++) {
            pruneFlipSlice[f][s] = -1;
        }
    }
    
    pruneFlipSlice[0][0] = 0;
    int count = 1;
    int depth = 0;

    while (count < 2048 * 495) {
        // scan for current depth
        for (int f = 0; f < 2048; f++) {
            for (int s = 0; s < 495; s++) {
                if (pruneFlipSlice[f][s] == depth) {
                    for (int m = 0; m < 18; m++) {
                        int newF = flipMove[f][m];
                        int newS = sliceMove[s][m];

                        if (pruneFlipSlice[newF][newS] == -1) {
                            pruneFlipSlice[newF][newS] = depth + 1;
                            count++;
                        }

                    }                
                }
            }
        }
        depth++;
    }

    // twist prune table
    for (int f = 0; f < 2187; f++) {
        for (int s = 0; s < 495; s++) {
            pruneTwistSlice[f][s] = -1;
        }
    }
    
    pruneTwistSlice[0][0] = 0;
    count = 1;
    depth = 0;

    while (count < 2187 * 495) {
        // scan for current depth
        for (int f = 0; f < 2187; f++) {
            for (int s = 0; s < 495; s++) {
                if (pruneTwistSlice[f][s] == depth) {
                    for (int m = 0; m < 18; m++) {
                        int newF = twistMove[f][m];
                        int newS = sliceMove[s][m];

                        if (pruneTwistSlice[newF][newS] == -1) {
                            pruneTwistSlice[newF][newS] = depth + 1;
                            count++;
                        }

                    }                
                }
            }
        }
        depth++;
    }
}