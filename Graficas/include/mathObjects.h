/**
 * @file    mathObjects.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    05/02/25
 * @brief   .
 */

/**
 * @include
 */
#pragma once
#include "stdHeaders.h"

const float PI = static_cast<float>(4.0f * std::atan(1.0f));

const float DEG2RAD = static_cast<float>(PI / 180.0f);

const float RAD2DEG = static_cast<float>(180.0f / PI);

inline float 
clamp(float inVal, float inMin, float inMax) {
  return inVal < inMin ? inMin : inVal > inMax ? inMax : inVal;
}


struct Matrix3 {
  //Vector3 row0, row1, row2;
  float m[3][3];
};

struct Matrix4 {
  float m[4][4];

  Matrix4
  operator*(const Matrix4& inMat) const {
    Matrix4 outData;
    for(int32 i = 0; i < 4; i++) {
      
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
  transpose() {}
};

static const Matrix4 
IDENTITY = {1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1};

struct Vector3 {
  float x;
  float y;
  float z;

  Vector3() = default;
  Vector3(float inX, float inY = 0.f, float inZ = 0.f)
    : x(inX),
    y(inY),
    z(inZ) { }

  Vector3 
  operator+(const Vector3& inVec) const {
    return {x + inVec.x, y + inVec.y, z + inVec.z};
  }
  
  Vector3 
  operator-(const Vector3& inVec) const {
    return {x - inVec.x, y - inVec.y, z - inVec.z};
  }

  Vector3 
  operator*(const Vector3& inVec) const {
    return {x * inVec.x, y * inVec.y, z * inVec.z};
  }
  
  Vector3
  operator*(const Matrix3& inMatrix) const {
    return { x * inMatrix.m[0][0] + y * inMatrix.m[0][1] + z * inMatrix.m[0][2],
             x * inMatrix.m[1][0] + y * inMatrix.m[1][1] + z * inMatrix.m[1][2],
             x * inMatrix.m[2][0] + y * inMatrix.m[2][1] + z * inMatrix.m[2][2] };
  }

  void
  operator*=(const Matrix4& inMatrix) {
    x = x * inMatrix.m[0][0] + y * inMatrix.m[0][1] + z * inMatrix.m[0][2] + 1 * inMatrix.m[0][3];
    y = x * inMatrix.m[1][0] + y * inMatrix.m[1][1] + z * inMatrix.m[1][2] + 1 * inMatrix.m[1][3];  // Si es posicion es 1, si es direccion es 0 en la W
    z = x * inMatrix.m[2][0] + y * inMatrix.m[2][1] + z * inMatrix.m[2][2] + 1 * inMatrix.m[2][3];
  }

  void
  operator+=(const Vector3& inVec) {
    x += inVec.x;
    y += inVec.y;
    z += inVec.z;
  }

  void
  operator*=(const Vector3& inVec) {
    x *= inVec.x;
    y *= inVec.y;
    z *= inVec.z;
  }

  Vector3 
  operator/(const Vector3& inVec) const {
    return {x / inVec.x, y / inVec.y, z / inVec.z};
  }

  Vector3
  operator+(float inVal) const {
    return {x + inVal, y + inVal, z + inVal};
  }

  Vector3
  operator-(float inVal) const {
    return {x - inVal, y - inVal, z - inVal};
  }
  
  Vector3
  operator*(float inVal) const {
    return {x * inVal, y * inVal, z * inVal};
  }

  Vector3
  operator/(float inVal) const {
    return {x / inVal, y / inVal, z / inVal};
  }

  Vector3
  operator-() const {
    return {-x, -y, -z};
  }


  Vector3 
  cross(const Vector3& inVect) const {
    return {y * inVect.z - z * inVect.y, 
            z * inVect.x - x * inVect.z, 
            x * inVect.y - y * inVect.x};
  }

  Vector3
  operator^(const Vector3& inVect) const { //For easy usage of cross prod
    return cross(inVect);
  }

  
  bool
  operator!=(const Vector3& inVect) const {
    return (x != inVect.x) || (y != inVect.y) || (z != inVect.z);
  }
  
  bool
  operator!=(const float& inVal) const {
    return (x != inVal) || (y != inVal) || (z != inVal);
  }

  bool
  operator==(const Vector3& inVect) const {
    return (x == inVect.x) || (y == inVect.y);
  }

  bool
  operator>(const float& inVal) const {
    return (x >inVal) || (y > inVal) || (z > inVal);
  }

  bool
  operator<(const float& inVal) const {
    return (x < inVal) || (y < inVal) || (z > inVal);
  }

  float 
  size() const {
    return sqrtf(x * x + y * y + z * z);
  }

  Vector3
  normalize() const {
    float invLenght = 1.f / size();
    return { x * invLenght,
             y * invLenght,
             z * invLenght};
  }

  float 
  dot(const Vector3& inVect) const {
    return x * inVect.x + y * inVect.y + z * inVect.z;
  }

  float
  operator|(const Vector3& inVect) const { //For easy usage of dot prof
    return dot(inVect);
  }

};

struct Vector2 {
  float x;
  float y;

  Vector2 
  operator+(const Vector2& inVec) const {
    return {x + inVec.x, y + inVec.y};
  }
  
  Vector2 
  operator-(const Vector2& inVec) const {
    return {x - inVec.x, y - inVec.y};
  }

  Vector2 
  operator*(const Vector2& inVec) const {
    return {x * inVec.x, y * inVec.y};
  }
  
  Vector2 
  operator/(const Vector2& inVec) const {
    return {x / inVec.x, y / inVec.y};
  }

  Vector2
  operator+(float inVal) const {
    return {x + inVal, y + inVal};
  }

  Vector2
  operator-(float inVal) const {
    return {x - inVal, y - inVal};
  }
  
  Vector2
  operator*(float inVal) const {
    return {x * inVal, y * inVal};
  }

  Vector2
  operator/(float inVal) const {
    return {x / inVal, y / inVal};
  }

  Vector2
  operator-() const {
    return {-x, -y};
  }

  Vector2 
  cross(const Vector2& inVect) const {
    return {(x * inVect.y) - (y * inVect.x)};
  }

  Vector2
  operator^(const Vector2& inVect) const { //For easy usage of cross prod
    return cross(inVect);
  }

  
  bool
  operator!=(const Vector2& inVect) const {
    return (x != inVect.x) || (y != inVect.y);
  }
  
  bool
  operator==(const Vector2& inVect) const {
    return (x == inVect.x) || (y == inVect.y);
  }
  
  bool
  operator>(const float& inVal) const {
    return (x >inVal) || (y > inVal);
  }

  bool
  operator<(const float& inVal) const {
    return (x < inVal) || (y < inVal);
  }

  float 
  size() const {
    return sqrtf(x * x + y * y);
  }

  Vector2
  normalize() const {
    float invLenght = 1.f / size();
    return { x * invLenght,
             y * invLenght};
  }

  float 
  dot(const Vector2& inVect) const {
    return x * inVect.x + y * inVect.y;
  }

  float
  operator|(const Vector2& inVect) const { //For easy usage of dot prof
    return dot(inVect);
  }

};

struct Vector2i {
  int32 x;
  int32 y;

  Vector2i 
  operator+(const Vector2i& inVec) const {
    return {x + inVec.x, y + inVec.y};
  }
  
  Vector2i 
  operator-(const Vector2i& inVec) const {
    return {x - inVec.x, y - inVec.y};
  }

  Vector2i 
  operator*(const Vector2i& inVec) const {
    return {x * inVec.x, y * inVec.y};
  }
  
  Vector2i 
  operator/(const Vector2i& inVec) const {
    return {x / inVec.x, y / inVec.y};
  }

  Vector2i
  operator+(int32 inVal) const {
    return {x + inVal, y + inVal};
  }

  Vector2i
  operator-(int32 inVal) const {
    return {x - inVal, y - inVal};
  }
  
  Vector2i
  operator*(int32 inVal) const {
    return {x * inVal, y * inVal};
  }

  Vector2i
  operator/(int32 inVal) const {
    return {x / inVal, y / inVal};
  }

  Vector2i
  operator-() const {
    return {-x, -y};
  }

  Vector2i 
  cross(const Vector2i& inVect) const {
    return {(x * inVect.y) - (y * inVect.x)};
  }

  Vector2i
  operator^(const Vector2i& inVect) const { //For easy usage of cross prod
    return cross(inVect);
  }

  bool
  operator!=(const Vector2i& inVect) const {
    return (x != inVect.x) || (y != inVect.y);
  }
  
  bool
  operator==(const Vector2i& inVect) const {
    return (x == inVect.x) || (y == inVect.y);
  }

  bool
  operator>(const float& inVal) const {
    return (x >inVal) || (y > inVal);
  }

  bool
  operator<(const float& inVal) const {
    return (x < inVal) || (y < inVal);
  }

  float 
  size() const {
    return sqrtf(x * x + y * y);
  }

  Vector2i
  normalize() const {
    float invLenght = 1.f / size();
    return { (int32)(x * invLenght),
             (int32)(y * invLenght)};
  }

  float 
  dot(const Vector2i& inVect) const {
    return x * inVect.x + y * inVect.y;
  }

  float
  operator|(const Vector2i& inVect) const { //For easy usage of dot prof
    return dot(inVect);
  }

};

struct Color {
  uint8 r;
  uint8 g;
  uint8 b;
  uint8 a;

  bool 
  operator!=(const Color& inColor) {
    if (r != inColor.r &&
        g != inColor.g &&
        b != inColor.b &&
        a != inColor.a) {
      return true;
    }
    return false;
  }

  bool 
  operator==(const Color& inColor) {
    if (r == inColor.r &&
        g == inColor.g &&
        b == inColor.b) {
      return true;
    }
    return false;
  }
};

static Color Black {0, 0, 0, 255};
static Color White {255, 255, 255, 255};
static Color Red {255, 0, 0, 255};
static Color Green {0, 255, 0, 255};
static Color Blue {0, 0, 255, 255};
static Color Purple {255, 0, 255, 255};
static Color Grey {30, 30, 30, 255};

struct FloatColor {
  FloatColor() = default;
  FloatColor(float inR, float inG, float inB, float inA) : r(inR), g(inG), b(inB), a(inA){}
  FloatColor(const Color& inColor) 
  : r(inColor.r / 255.f),
    g(inColor.g / 255.f),
    b(inColor.b / 255.f),
    a(inColor.a / 255.f) {}

  Color 
  toColor() {
    return Color{ static_cast<uint8>(r * 255.f),
                  static_cast<uint8>(g * 255.f), 
                  static_cast<uint8>(b * 255.f), 
                  static_cast<uint8>(a * 255.f)};
  }

  FloatColor&
  saturate() {
    r = clamp(r, 0.f, 1.f);
    g = clamp(g, 0.f, 1.f);
    b = clamp(b, 0.f, 1.f);
    a = clamp(a, 0.f, 1.f);
    return *this;
  }

  float 
  toGrey() {
    return 0.299f * r + 0.587f * g + 0.114f * b;//The max range of color the human eye can see. | Factor de luminancia persceptual
  }


  FloatColor operator+(const FloatColor& inColor) {
    return FloatColor(r+inColor.r, g+inColor.g, b+inColor.b, a+inColor.a);
  }
  FloatColor operator+(float inScalar) {
    return FloatColor(r + inScalar, g + inScalar, b + inScalar, a + inScalar);
  }
  FloatColor operator-(const FloatColor& inColor) {
    return FloatColor(r-inColor.r, g-inColor.g, b-inColor.b, a-inColor.a);
  }
  FloatColor operator*(const FloatColor& inColor) {
    return FloatColor(r * inColor.r, g*inColor.g, b*inColor.b, a*inColor.a);
  }
  FloatColor operator*(float inScalar) {
    return FloatColor(r * inScalar, g * inScalar, b * inScalar, a * inScalar);
  }
  FloatColor operator/(float inScalar) {
    return FloatColor(r/inScalar, g/inScalar, b/inScalar, a/inScalar);
  }

  float r;
  float g;
  float b;
  float a;
};


struct Vertex {
  Vector3 position;
  Vector2 uv;
  Color color;
};

struct Triangle {
  Vertex v0;
  Vertex v1;
  Vertex v2;

  Vector3
  getBaricenter() {
    return Vector3((v0.position + v1.position + v2.position)/3);
  }
};

struct Mesh {
  Vector<Triangle> m_triangles;
  
  inline Vector<Triangle>
  getRotated(Vector3 inAngle) { //Angulos siempre en radianes
    Vector<Triangle> outData;
    outData.reserve(m_triangles.size());

    float cosX = cos(inAngle.x), sinX = sin(inAngle.x);
    float cosY = cos(inAngle.y), sinY = sin(inAngle.y);
    float cosZ = cos(inAngle.z), sinZ = sin(inAngle.z);
    //escala, Rotacion posicion
    Matrix3 rotX = {
      1,    0,     0,
      0, cosX, -sinX,
      0, sinX,  cosX
    };

    Matrix3 rotY = {
      cosY,    0,  sinY,
         0,    1,     0,
     -sinY,    0,  cosY
    };

    Matrix3 rotZ = {
      cosZ, -sinZ,     0,
      sinZ,  cosZ,     0,
         0,     0,     1
    };

    for(const auto& iterTriangle : m_triangles) {
      Triangle tmpTriangle = iterTriangle;
      //Rotate in X
      tmpTriangle.v0.position = tmpTriangle.v0.position * rotX;
      tmpTriangle.v1.position = tmpTriangle.v1.position * rotX;
      tmpTriangle.v2.position = tmpTriangle.v2.position * rotX;
      //Rotate in Y (with the previous calculated data)
      tmpTriangle.v0.position = tmpTriangle.v0.position * rotY;
      tmpTriangle.v1.position = tmpTriangle.v1.position * rotY;
      tmpTriangle.v2.position = tmpTriangle.v2.position * rotY;
      //Rotate in Z (with the previous calculated data)
      tmpTriangle.v0.position = tmpTriangle.v0.position * rotZ;
      tmpTriangle.v1.position = tmpTriangle.v1.position * rotZ;
      tmpTriangle.v2.position = tmpTriangle.v2.position * rotZ;
      //Save the data to return 
      outData.push_back(tmpTriangle);
    }
    return outData;
  }

  inline void
  setPosition(Vector3 inNewPos) {
    for(auto& iterTriangle : m_triangles) {
      iterTriangle.v0.position += inNewPos;
      iterTriangle.v1.position += inNewPos;
      iterTriangle.v2.position += inNewPos;
    }
  }

  inline Vector<Triangle>
  getTransformed(Vector3 inScale, Vector3 inRotDeg, Vector3 inPos) {
    //Get the rotation in radians
    Vector3 tmpRotRad = inRotDeg * DEG2RAD;
    Vector<Triangle> outData;
    Matrix4 tmpTransform = IDENTITY;
    Matrix4 tmpRotX = IDENTITY, tmpRotY = IDENTITY, tmpRotZ = IDENTITY;
    Matrix4 tmpFinalRot = IDENTITY;

    float cosX = cos(tmpRotRad.x), sinX = sin(tmpRotRad.x);
    float cosY = cos(tmpRotRad.y), sinY = sin(tmpRotRad.y);
    float cosZ = cos(tmpRotRad.z), sinZ = sin(tmpRotRad.z);

    //Agregar el Baricentro
    //Al calcular la rotacion restar el baricentro
    //escala, Rotacion posicion

    //Adding the scale to the matrix
    tmpTransform.m[0][0] = inScale.x;
    tmpTransform.m[1][1] = inScale.y;
    tmpTransform.m[2][2] = inScale.z;

    if(inRotDeg != 0) {
      
      //Adding the rotation
      tmpRotX.m[1][1] = cosX;
      tmpRotX.m[2][2] = cosX;
      tmpRotX.m[1][2] = -sinX;
      tmpRotX.m[2][1] = sinX;
                            
      tmpRotY.m[0][0] = cosY;
      tmpRotY.m[2][2] = cosY;
      tmpRotY.m[0][2] = sinY;
      tmpRotY.m[2][0] = -sinY;

      tmpRotZ.m[0][0] = cosZ; 
      tmpRotZ.m[1][1] = cosZ; 
      tmpRotZ.m[0][1] = -sinZ;
      tmpRotZ.m[1][0] = sinZ;

      tmpFinalRot = tmpRotX * tmpRotY * tmpRotZ;
    }
    //Adding positions
    tmpTransform.m[0][3] = inPos.x;
    tmpTransform.m[1][3] = inPos.y;
    tmpTransform.m[2][3] = inPos.z;

    tmpTransform = tmpTransform * tmpFinalRot;

    for (const auto& iterTriangle : m_triangles) {
      Triangle tmpTriangle = iterTriangle;
      tmpTriangle.v0.position *= tmpTransform;
      tmpTriangle.v1.position *= tmpTransform;
      tmpTriangle.v2.position *= tmpTransform;
      outData.push_back(tmpTriangle);
    }
    return outData;
  }
};

