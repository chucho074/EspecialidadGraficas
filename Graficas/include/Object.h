/**
 * @file    Object.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    02/5/2025
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "UUID.h"

class Object {
 public:
	Object() = default;
	virtual ~Object() = default;

  virtual void
  update(float inDT) {
    UNREFERENCED_PARAMETER(inDT);
  }

  void
  setName(StringView& inName) {
    m_name = inName;
  }

  void
  setID(const UUID& inID) {
    m_id = inID;
  }

  const String&
  getName() const {
    return m_name;
  }

  const UUID&
  getID() const {
    return m_id;
  }

  void
  setActive(bool inStatus) {
    m_isActive = inStatus;
  }

 protected:
  String m_name;
  UUID m_id = UUID::ZERO;
  bool m_isActive = true;

};