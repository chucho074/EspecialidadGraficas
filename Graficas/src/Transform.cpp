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
  //m_scale = Vector3::UNIT;
  m_scale = {1, 1, 1};
  m_position = Vector3::ZERO;
  m_rotation = Vector3::ZERO;
}

void 
Transform::setScale(float inScale) {
  m_scale = {inScale, inScale, inScale};
}

void 
Transform::setScale(const Vector3& inScale) {
  m_scale = inScale;
}

void 
Transform::setRotation(const Vector3& inRotation) {
  m_rotation = inRotation;
}

void 
Transform::setPosition(const Vector3& inPos) {
  m_position = inPos;
}

Matrix4 
Transform::getMatrix() {
  Matrix4 outData;
  outData.identity();
  //Scale
  outData.scale(m_scale);

  //Rot
  if(m_rotation.x || 0) {
    outData = outData * Matrix4::getRotatedX(m_rotation.x);
  }
  if(m_rotation.y || 0) {
    outData = outData* Matrix4::getRotatedY(m_rotation.y);
  }
  if(m_rotation.z || 0) {
    outData = outData* Matrix4::getRotatedZ(m_rotation.z);
  }
  
  //Pos
  outData = outData * Matrix4::getTranslated(m_position);

  return outData;
}
