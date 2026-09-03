#ifndef METHOD_H
#define METHOD_H

#include <vector>
#include "../CubeState/CubeState.h"

// A solving method, one per directory under Solvers/
// Same two calls for every method, so Main keeps them in a table and never changes
//
// buildTables runs once at startup, empty for a method that needs no tables
// solve returns the moves that solve the state, or nothing if it is already solved
// implemented == false marks a placeholder, which Main reports instead of calling
struct Method {
    const char* name;
    const char* description;
    bool implemented;
    void (*buildTables)();
    std::vector<Move> (*solve)(const CubeState& s);
};

extern const Method METHODS[3];
extern const int METHOD_COUNT;

#endif // METHOD_H
