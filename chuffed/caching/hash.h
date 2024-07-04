#ifndef CHUFFED_HASH_H
#define CHUFFED_HASH_H

#include <cstddef>
#include <functional>

// Taken from the Boost library.
template<typename T>
void hashCombine( size_t &seed, const T& v) {
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

#endif //CHUFFED_HASH_H