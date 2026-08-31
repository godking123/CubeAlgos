#include "CoordTables.h"
#include "CubeState.h"
#include "Coords.h"

// actual memory allocated here once
int flipMove[2048][18];
int twistMove[2187][18];
int sliceMove[495][18];

int cpMove[40320][18];
int epMove[40320][18];
int slicePermMove[24][18];

int pruneFlipSlice[2048][495];
int pruneTwistSlice[2187][495];

int pruneCPSlicePerm[40320][24];
int pruneEPSlicePerm[40320][24];

// phase 2 only turns the G1 generators <U, D, R2, L2, F2, B2>
const int PHASE2_MOVES[10] = {
    static_cast<int>(Move::U),  static_cast<int>(Move::U2), static_cast<int>(Move::Up),
    static_cast<int>(Move::D),  static_cast<int>(Move::D2), static_cast<int>(Move::Dp),
    static_cast<int>(Move::R2), static_cast<int>(Move::L2),
    static_cast<int>(Move::F2), static_cast<int>(Move::B2)
};

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

void buildPhase2Tables() {
    // CP move table
    for (int cp = 0; cp < 40320; cp++) {
        CubeState s = CubeState::solved();
        Coords::decodeCP(s, cp);
        for (int m = 0; m < 18; m++) {
            CubeState next = s.apply(static_cast<Move>(m));
            cpMove[cp][m] = Coords::encodeCP(next);
        }
    }

    // EP move table
    for (int ep = 0; ep < 40320; ep++) {
        CubeState s = CubeState::solved();
        Coords::decodeEP(s, ep);
        for (int m = 0; m < 18; m++) {
            CubeState next = s.apply(static_cast<Move>(m));
            epMove[ep][m] = Coords::encodeEP(next);
        }
    }

    // SlicePerm move table
    for (int sp = 0; sp < 24; sp++) {
        CubeState s = CubeState::solved();
        Coords::decodeSlicePerm(s, sp);
        for (int m = 0; m < 18; m++) {
            CubeState next = s.apply(static_cast<Move>(m));
            slicePermMove[sp][m] = Coords::encodeSlicePerm(next);
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

void buildPhase2PruningTables() {
    // cp prune table
    for (int c = 0; c < 40320; c++) {
        for (int s = 0; s < 24; s++) {
            pruneCPSlicePerm[c][s] = -1;
        }
    }

    pruneCPSlicePerm[0][0] = 0;
    int count = 1;
    int depth = 0;

    while (count < 40320 * 24) {
        // scan for current depth
        for (int c = 0; c < 40320; c++) {
            for (int s = 0; s < 24; s++) {
                if (pruneCPSlicePerm[c][s] == depth) {
                    for (int i = 0; i < 10; i++) {
                        int m = PHASE2_MOVES[i];
                        int newC = cpMove[c][m];
                        int newS = slicePermMove[s][m];

                        if (pruneCPSlicePerm[newC][newS] == -1) {
                            pruneCPSlicePerm[newC][newS] = depth + 1;
                            count++;
                        }

                    }
                }
            }
        }
        depth++;
    }

    // ep prune table
    for (int e = 0; e < 40320; e++) {
        for (int s = 0; s < 24; s++) {
            pruneEPSlicePerm[e][s] = -1;
        }
    }

    pruneEPSlicePerm[0][0] = 0;
    count = 1;
    depth = 0;

    while (count < 40320 * 24) {
        // scan for current depth
        for (int e = 0; e < 40320; e++) {
            for (int s = 0; s < 24; s++) {
                if (pruneEPSlicePerm[e][s] == depth) {
                    for (int i = 0; i < 10; i++) {
                        int m = PHASE2_MOVES[i];
                        int newE = epMove[e][m];
                        int newS = slicePermMove[s][m];

                        if (pruneEPSlicePerm[newE][newS] == -1) {
                            pruneEPSlicePerm[newE][newS] = depth + 1;
                            count++;
                        }

                    }
                }
            }
        }
        depth++;
    }
}
