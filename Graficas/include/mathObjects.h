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

static Color Black  {0,   0,   0,   255};
static Color White  {255, 255, 255, 255};
static Color Red    {255, 0,   0,   255};
static Color Green  {0,   255, 0,   255};
static Color Blue   {0,   0,   255, 255};
static Color Purple {255, 0,   255, 255};
static Color Grey   {30,  30,  30,  255};

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

  float*
  toArray() {
    float outData[4] = {r, g, b, a};
    return outData;
  }

  FloatColor operator+(const FloatColor& inColor) {
    return FloatColor(r+inColor.r, g+inColor.g, b+inColor.b, a+inColor.a);
  }
  FloatColor operator-(const FloatColor& inColor) {
    return FloatColor(r-inColor.r, g-inColor.g, b-inColor.b, a-inColor.a);
  }
  FloatColor operator*(const FloatColor& inColor) {
    return FloatColor(r * inColor.r, g*inColor.g, b*inColor.b, a*inColor.a);
  }
  FloatColor operator+(float inScalar) {
    return FloatColor(r + inScalar, g + inScalar, b + inScalar, a + inScalar);
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
    Matrix4 rotX = {
      1,    0,     0, 0,
      0, cosX, -sinX, 0,
      0, sinX,  cosX, 0,
      0,    0,     0, 1
    };

    Matrix4 rotY = {
      cosY,    0,  sinY, 0,
         0,    1,     0, 0,
     -sinY,    0,  cosY, 0,
         0,    0,     0, 1

    };

    Matrix4 rotZ = {
      cosZ, -sinZ,    0, 0,
      sinZ,  cosZ,    0, 0,
         0,     0,    1, 0,
         0,    0,     0, 1
    };

    for(const auto& iterTriangle : m_triangles) {
      Triangle tmpTriangle = iterTriangle;
      //Rotate in X
      tmpTriangle.v0.position = rotX.TransformPosition(tmpTriangle.v0.position);
      tmpTriangle.v1.position = rotX.TransformPosition(tmpTriangle.v1.position);
      tmpTriangle.v2.position = rotX.TransformPosition(tmpTriangle.v2.position);
      //Rotate in Y (with the previous calculated data)
      tmpTriangle.v0.position = rotY.TransformPosition(tmpTriangle.v0.position);
      tmpTriangle.v1.position = rotY.TransformPosition(tmpTriangle.v1.position);
      tmpTriangle.v2.position = rotY.TransformPosition(tmpTriangle.v2.position);
      //Rotate in Z (with the previous calculated data)
      tmpTriangle.v0.position = rotZ.TransformPosition(tmpTriangle.v0.position);
      tmpTriangle.v1.position = rotZ.TransformPosition(tmpTriangle.v1.position);
      tmpTriangle.v2.position = rotZ.TransformPosition(tmpTriangle.v2.position);
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
      tmpTransform.TransformPosition(tmpTriangle.v0.position);
      tmpTransform.TransformPosition(tmpTriangle.v1.position);
      tmpTransform.TransformPosition(tmpTriangle.v2.position);
      outData.push_back(tmpTriangle);
    }
    return outData;
  }
};

class Camera {
 public:
  Camera() = default; 
  ~Camera() = default;

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
  setPerspectiveHalf(float inHalfFOV, 
                     Vector2 inScreenSize, 
                     float inMinZ, 
                     float inMaxZ) {
    halfFov = inHalfFOV;
    screenSize = inScreenSize;
    minZ = inMinZ;
    maxZ = inMaxZ;
    projectionMatrix.PerspectiveHalfFovLH(halfFov, screenSize, minZ, maxZ);
  }

  void
  setPerspective(float inFOV,
                 Vector2 inScreenSize,
                 float inNear,
                 float inFar) {
    halfFov = inFOV / 2;
    screenSize = inScreenSize;
    minZ = inNear;
    maxZ = inFar;
    projectionMatrix.PerspectiveFovLH(inFOV, (screenSize.x/screenSize.y), minZ, maxZ);
  }

  void 
  setOrthographic(float inLeft, 
                  float inRight, 
                  float inBottom, 
                  float inTop, 
                  float inNearZ, 
                  float inFarZ) {
    projectionMatrix.OrthographicLH(inLeft, inRight, inBottom, inTop, inNearZ, inFarZ);
  }


  Matrix4
  getViewMatrix() {
    //setLookAt(position, (position + Vector3::RIGHT), up);
    setLookAt(position, target, up);
    return viewMatrix;
  }

  Matrix4
  getProjectionMatrix() {
    projectionMatrix.PerspectiveHalfFovLH(halfFov, screenSize, minZ, maxZ);
    return projectionMatrix;
  }

  Vector3
  getPosition() const {
    return position;
  }

  Vector3
  getViewDir() const {
    return (target - position).normalize();
  }

 protected:
  Vector3 position;
  Vector3 target;
  Vector3 up;

  Vector2 screenSize;
  float halfFov;
  float minZ = 0.1f;
  float maxZ = 1000.f;

  Matrix4 viewMatrix;
  Matrix4 projectionMatrix;

};

//TODO: Arreglar las rotaciones
class EditorCamera : public Camera {
 public:
  EditorCamera() = default;
  ~EditorCamera() = default;


  void
  updateRotations() {
    float yawRad = (m_YPR.x * DEG2RAD);   // Yaw (horizontal)
    float pitchRad = (m_YPR.y * DEG2RAD); // Pitch (vertical)

    Vector3 forward = (target - position);
    Vector3 right = (up.cross(forward).normalize());

    // Calculo correcto del vector forward
    forward.x = cos(yawRad) * cos(pitchRad);
    forward.y = sin(pitchRad);
    forward.z = sin(yawRad) * cos(pitchRad);
    forward.normalize();

    // Recalcular los vectores de cámara
    up = forward.cross(right);
    right = up.cross(forward);
    //m_rightVector = m_forward.cross({0.0f, 1.0f, 0.0f, 0.0f}); // Siempre sobre Y global
    right.normalize();
    up = right.cross(forward);
    up.normalize();

    //Recalculate matrix
    target = (position + forward);
    setLookAt(position, target, up);
  }

  void 
  move(float inDeltaTime) {
    Vector3 forward = (target - position);
    Vector3 right = (up.cross(forward).normalize());

    float velocity = m_speed * inDeltaTime;
    if(m_front)
      position += forward * velocity;
    if(m_back)
      position -= forward * velocity;
    if(m_left)
      position -= right * velocity;
    if(m_right)
      position += right * velocity;
    if(m_up)
      position += up * velocity;
    if(m_down)
      position -= up * velocity;
    if(m_YawNeg) {
      m_YPR.x -= m_speed * inDeltaTime;
      if((m_YPR.x * DEG2RAD) < (0.f * DEG2RAD)) {
        m_YPR.x = 360.f * DEG2RAD;
      }

      updateRotations();
    }
    if(m_YawPos) {
      m_YPR.x += m_speed * inDeltaTime;
      if((m_YPR.x * DEG2RAD) > (360.f * DEG2RAD)) {
        m_YPR.x = (0.f * DEG2RAD);
      }
      updateRotations();
    }


    ConsoleOut << "X: " << getPosition().x
               << "\nY: " << getPosition().y
               << "\nZ: " << getPosition().z << "\n" << ConsoleLine;

    ConsoleOut << "Yaw: " << m_YPR.x
               << "\nPitch: " << m_YPR.y
               << "\nRoll: "  << m_YPR.z << "\n" << ConsoleLine;
  }

  Vector3 m_YPR = Vector3::ZERO; //Yaw Pitch Roll Values

  float m_speed = 0.00001f;

  bool m_front, m_back, m_left, m_right = false;
  bool m_up, m_down = false;
  bool m_YawPos = false, m_YawNeg = false;
 
};

class ShadowCamera : public Camera {
 public:
  ShadowCamera() = default;
  ~ShadowCamera() = default;


};