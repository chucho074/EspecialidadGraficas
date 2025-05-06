/**
 * @file    UID.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    30/4/2025
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"

class UID {
public:
  //Default constructor.
  UID();

  //Constructor for own purposes.
  UID(uint64 inData);

  //Copy constructor.
  UID(const UID& inUUID) = default;

  //Destructor
  ~UID() = default;

  operator uint64() const {
    return m_uuid;
  }

  static const UID
    ZERO;

private:

  uint64 m_uuid;

};

namespace std {
  template<>
  struct hash<UID> {
    size_T operator()(const UID& inUUID) const {
      return hash<uint64>()((uint64)inUUID);
    }
  };
}