#include <cstdint>
#ifndef MOVE_TABLE_H_
#define MOVE_TABLE_H_

struct MoveTable {
  uint8_t cp[8];
  uint8_t co[8];
  uint8_t ep[12];
  uint8_t eo[12];
}

#endif // MOVE_TABLE_H_