/**
 * @file    Camera.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    24/06/25
 */

/**
 * @include
 */
#pragma once
#include "mathObjects.h"
#include "windows.h"
#include "Radians.h"
#include "Degrees.h"

class Camera {
 public:
  Camera() = default;
  
  ~Camera() = default;

  void
  recalculateValues() { //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!move after rotate
    float yawRad = Radians(m_YPR.x).getRadians();   // Yaw (horizontal)
    float pitchRad = Radians(m_YPR.y).getRadians(); // Pitch (vertical)

    Vector3 front;
    front.x = cos(yawRad) * cos(pitchRad);
    front.y = sin(pitchRad);
    front.z = sin(yawRad) * cos(pitchRad);
    front = front.normalize();
    m_cameraFront = front;

    m_cameraRight = (m_cameraFront ^ m_WorldUp).normalize();
    m_upVec = (m_cameraRight ^ m_cameraFront).normalize();
  }

  void
  setLookAt(const Vector3& inEyePos, 
            const Vector3& inTargetPos, 
            const Vector3& inUpDir) {
    m_position = inEyePos;
    m_target = inTargetPos;
    m_upVec = inUpDir;
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
  }

  void 
  setOrthographic(float inLeft, 
                  float inRight, 
                  float inBottom, 
                  float inTop, 
                  float inNearZ, 
                  float inFarZ) {

    m_viewportRect = { static_cast<LONG>(inLeft), 
                       static_cast<LONG>(inTop), 
                       static_cast<LONG>(inRight), 
                       static_cast<LONG>(inBottom) };
    minZ = inNearZ;
    maxZ = inFarZ;
  }

  Vector3
  getCameraDir() const {
    return (m_target - m_position).normalize();
  }


  Matrix4
  getViewMatrix() {
    viewMatrix.lookAt(m_position, (m_position + m_cameraFront), m_upVec);
    return viewMatrix;
  }

  Matrix4
  getProjectionMatrix() {
    projectionMatrix.PerspectiveHalfFovLH(halfFov, screenSize, minZ, maxZ);
    return projectionMatrix;
  }

  Matrix4
  getOrthoMatrix() {
    projectionMatrix.OrthographicLH(m_viewportRect.left,
                                    m_viewportRect.right,
                                    m_viewportRect.bottom, 
                                    m_viewportRect.top,
                                    minZ,
                                    maxZ);
    return projectionMatrix;
  }

  Vector3
  getPosition() const {
    return m_position;
  }

 public:
  Vector3 m_position;
  Vector3 m_target;
  Vector3 m_upVec;
  Vector3 m_cameraRight;

  Vector3 m_cameraFront = Vector3::FRONT;
  Vector3 m_WorldUp = Vector3::UP;


  Vector2 screenSize;
  float halfFov;
  float minZ = 0.1f;
  float maxZ = 1000.f;

  RECT m_viewportRect = { 0, 0, 1280, 720 }; // Default viewport size

  Matrix4 viewMatrix;
  Matrix4 projectionMatrix;

  Vector3 m_YPR = Vector3::ZERO; //Yaw Pitch Roll Values
};

class EditorCamera : public Camera {
 public:
  EditorCamera() = default;
  ~EditorCamera() = default;

  void 
  move(float inDeltaTime) {

    float velocity = m_speed * inDeltaTime;

    if(m_YawPos) {
      m_YPR.x += velocity;
      if((Radians(m_YPR.x)) > (Degrees(360.f).getRadians())) {
        m_YPR.x = 0.f;
      }
    }

    recalculateValues();

    if(m_front)
      m_position += m_cameraFront * velocity;
    if(m_back)
      m_position -= m_cameraFront * velocity;
    if(m_left)
      m_position += m_cameraRight * velocity;
    if(m_right)
      m_position -= m_cameraRight * velocity;
    if(m_up)
      m_position += m_upVec * velocity;
    if(m_down)
      m_position -= m_upVec * velocity;
    if(m_YawNeg) {
      m_YPR.x -= velocity;
      if((Radians(m_YPR.x)) < (Radians(0.f))) {
        m_YPR.x = Degrees(360.f).getRadians();
      }
    }
    
  }


  float m_speed = 1.f;

  bool m_front, m_back, m_left, m_right = false;
  bool m_up, m_down = false;
  bool m_YawPos = false, m_YawNeg = false;
 
};

class ShadowCamera : public Camera {
 public:
  ShadowCamera() = default;
  ~ShadowCamera() = default;



};