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
#include "Vectors.h"




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
  Matrix4(float m00, float m01, float m02, float m03,
          float m10, float m11, float m12, float m13,
          float m20, float m21, float m22, float m23,
          float m30, float m31, float m32, float m33) {
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;
    
    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;

    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;

    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
  }


  void 
  identity();

  float m[4][4];

  
  static const Matrix4 IDENTITY;


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

  Matrix4&
  getTransposed() const;

  void
  rotateX(float inAngle);

  void
  rotateY(float inAngle);

  void
  rotateZ(float inAngle);

  void
  scale(float inScale);

  void
  scale(Vector3 inScale);

  static Matrix4
  getRotatedX(float inAngle);

  static Matrix4
  getRotatedY(float inAngle);

  static Matrix4
  getRotatedZ(float inAngle);

  static Matrix4
  getTranslated(const Vector3& inTranslation);

};


//static Vector3
//operator*(const Matrix3& inMatrix);
//
//static void
//operator*=(const Matrix4& inMatrix);