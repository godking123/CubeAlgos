#ifndef COORDTABLES_H
#define COORDTABLES_H

extern int flipMove[2048][18];
extern int twistMove[2187][18];
extern int sliceMove[495][18];

extern int cpMove[40320][18];
extern int epMove[40320][18];
extern int slicePermMove[24][18];

extern int pruneFlipSlice[2048][495];
extern int pruneTwistSlice[2187][495];

extern int pruneCPSlicePerm[40320][24];
extern int pruneEPSlicePerm[40320][24];

extern const int PHASE2_MOVES[10];  // G1 generators <U, D, R2, L2, F2, B2>

void buildPruningTables();
void buildCoordTables();   // call once at startup in main
void buildPhase2Tables();  // same, before any phase 2 search
void buildPhase2PruningTables();

#endif