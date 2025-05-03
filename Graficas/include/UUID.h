/**
 * @file    UUID.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    30/4/2025
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"

class UUID {
public:
  //Default constructor.
  UUID();

  //Constructor for own purposes.
  UUID(uint64 inData);

  //Copy constructor.
  UUID(const UUID& inUUID) = default;

  //Destructor
  ~UUID() = default;

  operator uint64() const {
    return m_uuid;
  }

  static const UUID
    ZERO;

private:

  uint64 m_uuid;

};

namespace std {
  template<>
  struct hash<UUID> {
    size_T operator()(const UUID& inUUID) const {
      return hash<uint64>()((uint64)inUUID);
    }
  };
}