#include "Method.h"
#include "Kociemba/Kociemba.h"
#include "CFOP/CFOP.h"
#include "Roux/Roux.h"

const Method METHODS[3] = {
    {"Kociemba", "two-phase IDA* search, 20-24 moves", true,
     Kociemba::buildTables, Kociemba::solve},
    {"CFOP",     "cross, F2L, OLL, PLL",               false,
     CFOP::buildTables,     CFOP::solve},
    {"Roux",     "blocks, CMLL, LSE",                  false,
     Roux::buildTables,     Roux::solve},
};

const int METHOD_COUNT = 3;
