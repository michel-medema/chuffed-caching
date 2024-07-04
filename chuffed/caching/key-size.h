#ifndef CHUFFED_KEY_SIZE_H
#define CHUFFED_KEY_SIZE_H

#include <cstddef>
#include <vector>

inline size_t numBytes( const std::vector<bool> &v ) {
  size_t cap = v.capacity();

  if ( cap % 8 == 0 ) return cap / 8;

  return (cap / 8) + 1;
}

#endif //CHUFFED_KEY_SIZE_H
