#ifndef METHOD_H
#define METHOD_H

#include <vector>
#include "../CubeState/CubeState.h"

// A solving method. Each one lives in its own directory under Solvers/ and exposes
// the same two calls, so Main can keep them in a table and add a method without
// touching the menu code.
//
// buildTables runs once at startup; a method that needs no tables leaves it empty.
// solve returns the moves that solve the given state, or an empty sequence if the
// state is already solved. A method with implemented == false is a placeholder:
// its solve returns nothing and Main says so rather than reporting a bad solve.
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
