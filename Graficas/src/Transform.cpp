/**
 * @file    Transform.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    27/3/2025
 * @brief   .
 */

/**
 * @include
 */
#include "Transform.h"
#include "Vectors.h"

Transform::Transform() {
  m_localScale = {1, 1, 1};
  m_localPosition = Vector3::ZERO;
  m_localRotation = Vector3::ZERO;

  m_globalScale = {1, 1, 1};
  m_globalPosition = Vector3::ZERO;
  m_globalRotation = Vector3::ZERO;
}

void 
Transform::setLocalScale(float inScale) {
  m_localScale = {inScale, inScale, inScale};
}

void 
Transform::setLocalScale(const Vector3& inScale) {
  m_localScale = inScale;
}

void 
Transform::setLocalRotation(const Vector3& inRotation) {
  m_localRotation = inRotation;
}

void 
Transform::setLocalPosition(const Vector3& inPos) {
  m_localPosition = inPos;
}

void 
Transform::setGlobalScale(float inScale) {
  m_globalScale = {inScale, inScale, inScale};
}

void
Transform::setGlobalScale(const Vector3& inScale) {
  m_globalScale = inScale;
}

void 
Transform::setGlobalRotation(const Vector3& inRotation) {
  m_globalRotation = inRotation;
}

void 
Transform::setGlobalPosition(const Vector3& inPos) {
  m_globalPosition = inPos;
}

Vector3 
Transform::getLocalPosition() const {
  return m_localPosition;
}

Vector3 
Transform::getLocalScale() const {
  return m_localScale;
}

Vector3 
Transform::getLocalRotation() const {
  return m_localRotation;
}

Vector3 
Transform::getGlobalPosition() const {
  return m_globalPosition;
}

Vector3 
Transform::getGlobalScale() const {
  return m_globalScale;
}

Vector3 
Transform::getGlobalRotation() const {
  return m_globalRotation;
}



Matrix4 
Transform::getMatrix() {
  Matrix4 outData;
  outData.identity();
  //Scale
  outData.scale(m_localScale);

  //Rot
  if(m_localRotation.x || 0) {
    outData = outData * Matrix4::getRotatedX(m_localRotation.x);
  }
  if(m_localRotation.y || 0) {
    outData = outData * Matrix4::getRotatedY(m_localRotation.y);
  }
  if(m_localRotation.z || 0) {
    outData = outData * Matrix4::getRotatedZ(m_localRotation.z);
  }
  
  //Pos
  outData = outData * Matrix4::getTranslated(m_localPosition);

  return outData;
}
