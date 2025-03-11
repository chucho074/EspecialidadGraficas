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
#include "Vectors.h"
#include "Matrix4.h"

const float PI = static_cast<float>(4.0f * std::atan(1.0f));

const float DEG2RAD = static_cast<float>(PI / 180.0f);

const float RAD2DEG = static_cast<float>(180.0f / PI);

inline float 
clamp(float inVal, float inMin, float inMax) {
  return inVal < inMin ? inMin : inVal > inMax ? inMax : inVal;
}


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
    Matrix4 tmpTransform = Matrix4::IDENTITY;
    Matrix4 tmpRotX = Matrix4::IDENTITY, tmpRotY = Matrix4::IDENTITY, tmpRotZ = Matrix4::IDENTITY;
    Matrix4 tmpFinalRot = Matrix4::IDENTITY;

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

struct Camera {

  void
  setLookAt(const Vector3& inEyePos, 
            const Vector3& inTargetPos, 
            const Vector3& inUpDir) {
    position = inEyePos;
    target = inTargetPos;
    up = inUpDir;
    viewMatrix.lookAt(position, target, up);
  }

  void
  setPerspective(float inHalfFOV, Vector2 inScreenSize, float inMinZ, float inMaxZ) {
    fov = inHalfFOV;
    screenSize = inScreenSize;
    minZ = inMinZ;
    maxZ = inMaxZ;
    projectionMatrix.Perspective(fov, screenSize, minZ, maxZ);
  }

  Matrix4
  getViewMatrix() const {
    return viewMatrix;
  }

  Matrix4
  getProjectionMatrix() const {
    return projectionMatrix;
  }

 private:
  Vector3 position;
  Vector3 target;
  Vector3 up;

  float fov;
  Vector2 screenSize;
  float minZ;
  float maxZ;

  Matrix4 viewMatrix;
  Matrix4 projectionMatrix;

};
