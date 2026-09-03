#ifndef COORDTABLES_H
#define COORDTABLES_H

// Phase 1 Move Tables
extern int flipMove[2048][18];
extern int twistMove[2187][18];
extern int sliceMove[495][18];

// Phase 2 Move Tables
extern int cpMove[40320][18];
extern int epMove[40320][18];
extern int slicePermMove[24][18];

// Phase 1 Prune Tables
extern int pruneFlipSlice[2048][495];
extern int pruneTwistSlice[2187][495];

// Phase 2 Prune Tables
extern int pruneCPSlicePerm[40320][24];
extern int pruneEPSlicePerm[40320][24];

extern const int PHASE2_MOVES[10];  // G1 Generators <U, D, R2, L2, F2, B2>

// All Four Run Once at Startup, Before Any Search
void buildCoordTables();
void buildPruningTables();
void buildPhase2Tables();
void buildPhase2PruningTables();

#endif // COORDTABLES_H
