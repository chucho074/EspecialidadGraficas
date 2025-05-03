/**
 * @file    Transform.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    27/3/2025
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"

class Transform {
 public:
  Transform();
  ~Transform() = default;

  void
  setScale(float inScale);
  
  void
  setScale(const Vector3& inScale);

  void
  setRotation(const Vector3& inRotatio);

  void
  setPosition(const Vector3& inPos);

  Matrix4
  getMatrix();

 protected:

  Vector3 m_scale;
  Vector3 m_position;
  Vector3 m_rotation;
};