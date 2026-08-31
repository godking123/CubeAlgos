#ifndef COORDTABLES_H
#define COORDTABLES_H

extern int flipMove[2048][18];
extern int twistMove[2187][18];
extern int sliceMove[495][18];

extern int pruneFlipSlice[2048][495];
extern int pruneTwistSlice[2187][495];

void buildPruningTables();
void buildCoordTables();  // call once at startup in main

#endif