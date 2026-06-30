#ifndef MOVE_TABLE_H_
#define MOVE_TABLE_H_

#include <cstdint>

struct MoveTable {
  uint8_t cp[8];
  uint8_t co[8];
  uint8_t ep[12];
  uint8_t eo[12];
};

extern const MoveTable MOVE_TABLES[18]; // For each move contains the state transformation

#endif // MOVE_TABLE_H_
