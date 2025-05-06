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
  setLocalScale(float inScale);
  
  void
  setLocalScale(const Vector3& inScale);

  void
  setLocalRotation(const Vector3& inRotation);

  void
  setLocalPosition(const Vector3& inPos);
  

  
  void
  setGlobalScale(float inScale);
  
  void
  setGlobalScale(const Vector3& inScale);

  void
  setGlobalRotation(const Vector3& inRotation);

  void
  setGlobalPosition(const Vector3& inPos);


  Vector3 getLocalPosition() const;
  Vector3 getLocalScale() const;
  Vector3 getLocalRotation() const;

  Vector3 getGlobalPosition() const;
  Vector3 getGlobalScale() const;
  Vector3 getGlobalRotation() const;

  Matrix4
  getMatrix();

 protected:

  Vector3 m_localScale;
  Vector3 m_localPosition;
  Vector3 m_localRotation;

  Vector3 m_globalScale;
  Vector3 m_globalPosition;
  Vector3 m_globalRotation;
};