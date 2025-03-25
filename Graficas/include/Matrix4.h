/**
 * @file    Matrix4.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    08/03/25
 */

/**
 * @include
 */
#pragma once
#include "stdHeaders.h"

class Vector2;
class Vector3;

class Matrix3 {
 public:
  Matrix3()  = default;
  //Vector3 row0, row1, row2;
  float m[3][3];
  friend class Vector2;
  friend class Vector3;
  friend class Vector4;
};

class Matrix4 {
 public:
  Matrix4() = default;

  void 
  identity();

  float m[4][4];

  Matrix4
  operator*(const Matrix4& inMat) const;

  void
  lookAt(const Vector3& inEyePos, 
         const Vector3& inTargetPos, 
         const Vector3& inUpDir);


  void
  Perspective(float inHalfFOV, 
              Vector2 inScreenSize, 
              float inMinZ, 
              float inMaxZ);

  void
  Translate(const Vector3& inTranslation);

  Vector3
  TransformPosition(const Vector3& inVect);

  Vector3
  TransformDirection(const Vector3& inVect);

  void 
  transpose();

  void
  rotateY(float inAngle);

  void
  scale(float inScale);

  void
  scale(Vector3 inScale);


  static const Matrix4
  IDENTITY;
};

