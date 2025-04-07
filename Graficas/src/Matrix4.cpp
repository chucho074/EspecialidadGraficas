/**
 * @file    Matrix4.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    08/03/25
 */

/**
 * @include
 */
#include "Matrix4.h"
#include "Vectors.h"

const Matrix4 
Matrix4::IDENTITY(1, 0, 0, 0,
                  0, 1, 0, 0,
                  0, 0, 1, 0,
                  0, 0, 0, 1 );

void 
Matrix4::identity() {
  for (int32 i = 0; i < 4; ++i) {
    for (int32 j = 0; j < 4; ++j) {
      if (i == j) {
        m[i][j] = 1.f;
        continue;
      }
      m[i][j] = 0.f;
    }
  }
}

Matrix4 
Matrix4::operator*(const Matrix4& inMat) const {
  Matrix4 outData;
  for (int32 i = 0; i < 4; i++) {

    outData.m[i][0] = m[i][0] * inMat.m[0][0] + m[i][1] * inMat.m[1][0] +
      m[i][2] * inMat.m[2][0] + m[i][3] * inMat.m[3][0];

    outData.m[i][1] = m[i][0] * inMat.m[0][1] + m[i][1] * inMat.m[1][1] +
      m[i][2] * inMat.m[2][1] + m[i][3] * inMat.m[3][1];

    outData.m[i][2] = m[i][0] * inMat.m[0][2] + m[i][1] * inMat.m[1][2] +
      m[i][2] * inMat.m[2][2] + m[i][3] * inMat.m[3][2];

    outData.m[i][3] = m[i][0] * inMat.m[0][3] + m[i][1] * inMat.m[1][3] +
      m[i][2] * inMat.m[2][3] + m[i][3] * inMat.m[3][3];
  }
  return outData;
}

void 
Matrix4::lookAt(const Vector3& inEyePos, 
                const Vector3& inTargetPos, 
                const Vector3& inUpDir) {

  const Vector3 WAxis = (inEyePos - inTargetPos).normalize();
  const Vector3 ZAxis = (inTargetPos - inEyePos).normalize();
  const Vector3 XAxis = inUpDir.cross(ZAxis).normalize();
  const Vector3 YAxis = ZAxis.cross(XAxis);

  m[0][0] = XAxis.x;
  m[1][0] = XAxis.y;
  m[2][0] = XAxis.z;

  m[0][1] = YAxis.x;
  m[1][1] = YAxis.y;
  m[2][1] = YAxis.z;

  m[0][2] = ZAxis.x;
  m[1][2] = ZAxis.y;
  m[2][2] = ZAxis.z;

  m[0][3] = 0.f;
  m[1][3] = 0.f;
  m[2][3] = 0.f;

  Vector3 eyeNeg = -inEyePos;

  m[3][0] = inEyePos | XAxis;
  m[3][1] = inEyePos | YAxis;
  m[3][2] = inEyePos | ZAxis;
  m[3][3] = 1.f;

}

void 
Matrix4::Perspective(float inHalfFOV, 
                     Vector2 inScreenSize, 
                     float inMinZ, 
                     float inMaxZ) {
  float plane0[4] = { 1.f / tanf(inHalfFOV), 0.f, 0.f, 0.f };
  float plane1[4] = { 0.f, inScreenSize.x / tanf(inHalfFOV) / inScreenSize.y, 0.f, 0.f };
  float plane2[4] = { 0.f, 0.f, inMaxZ / (inMaxZ - inMinZ), 1.f };
  float plane3[4] = { 0.f, 0.f, -inMinZ * inMaxZ / (inMaxZ - inMinZ), 0.f };

  m[0][0] = plane0[0];
  m[0][1] = plane0[1];
  m[0][2] = plane0[2];
  m[0][3] = plane0[3];

  m[1][0] = plane1[0];
  m[1][1] = plane1[1];
  m[1][2] = plane1[2];
  m[1][3] = plane1[3];

  m[2][0] = plane2[0];
  m[2][1] = plane2[1];
  m[2][2] = plane2[2];
  m[2][3] = plane2[3];

  m[3][0] = plane3[0];
  m[3][1] = plane3[1];
  m[3][2] = plane3[2];
  m[3][3] = plane3[3];
}

void 
Matrix4::Translate(const Vector3& inTranslation) {
  m[3][0] += inTranslation.x;
  m[3][1] += inTranslation.y;
  m[3][2] += inTranslation.z;
}


Vector3 
Matrix4::TransformPosition(const Vector3& inVect) {

    return Vector3{inVect.x * m[0][0] + inVect.y * m[1][0] + inVect.z * m[2][0] + m[3][0] * 1.f,
                   inVect.x * m[0][1] + inVect.y * m[1][1] + inVect.z * m[2][1] + m[3][1] * 1.f,
                   inVect.x * m[0][2] + inVect.y * m[1][2] + inVect.z * m[2][2] + m[3][2] * 1.f};
}

Vector3 
Matrix4::TransformDirection(const Vector3& inVect) {
  return Vector3{inVect.x * m[0][0] + inVect.y * m[1][0] + inVect.z * m[2][0] + m[3][0] * 0.f,
                 inVect.x * m[0][1] + inVect.y * m[1][1] + inVect.z * m[2][1] + m[3][1] * 0.f,
                 inVect.x * m[0][2] + inVect.y * m[1][2] + inVect.z * m[2][2] + m[3][2] * 0.f};
}

void 
Matrix4::transpose() {
  for (int32 i = 0; i < 4; i++) {
    for (int32 j = i + 1; j < 4; j++) {
      float temp = m[i][j];
      m[i][j] = m[j][i];
      m[j][i] = temp;
    }
  }
}


Matrix4&
Matrix4::getTransposed() const {
  Matrix4 outData;
  outData = *this;
  outData.transpose();
  return outData;
}

void 
Matrix4::rotateX(float inAngle) {
  identity();
  //Y  X
  m[1][1] = cos(inAngle);
  m[1][2] = -sin(inAngle);
  m[2][1] = sin(inAngle);
  m[2][2] = cos(inAngle);
}

void 
Matrix4::rotateY(float inAngle) {
  
  identity();
  //Y  X
  m[0][0] = cos(inAngle);
  m[0][2] = sin(inAngle);
  m[2][0] = -sin(inAngle);
  m[2][2] = cos(inAngle);
}

void 
Matrix4::rotateZ(float inAngle) {
  identity();
  //Y  X
  m[0][0] = cos(inAngle);
  m[1][0] = sin(inAngle);
  m[0][1] = -sin(inAngle);
  m[1][1] = cos(inAngle);
}

void 
Matrix4::scale(float inScale) {
  identity();
  m[0][0] = inScale;
  m[1][1] = inScale;
  m[2][2] = inScale;
}

void 
Matrix4::scale(Vector3 inScale) {
  identity();
  m[0][0] = inScale.x;
  m[1][1] = inScale.y;
  m[2][2] = inScale.z;
}

Matrix4 
Matrix4::getRotatedX(float inAngle) {
  Matrix4 outData;
  //outData.identity();
  outData.rotateX(inAngle);
  return outData;
}

Matrix4 
Matrix4::getRotatedY(float inAngle) {
  Matrix4 outData;
  //outData.identity();
  outData.rotateY(inAngle);
  return outData;
}

Matrix4 
Matrix4::getRotatedZ(float inAngle) {
  Matrix4 outData;
  //outData.identity();
  outData.rotateZ(inAngle);
  return outData;
}

Matrix4 
Matrix4::getTranslated(const Vector3& inTranslation) {
  Matrix4 outData;
  outData.identity();
  outData.Translate(inTranslation);
  return outData;
}


