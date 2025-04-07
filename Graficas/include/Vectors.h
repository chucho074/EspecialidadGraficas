/**
 * @file    Vectors.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    08/03/25
 */

/**
 * @include
 */
#pragma once
#include "stdHeaders.h"

class Vector3 {
 public:
  Vector3() {
    *this = Vector3::ZERO;
  };
  Vector3(float inX, float inY = 0.f, float inZ = 0.f) {
    x = inX;
    y = inY;
    z = inZ;
  }

  Vector3(const Vector3& inVec) {
    x = inVec.x;
    y = inVec.y;
    z = inVec.z;
  }; 

  ~Vector3() = default;

  float x;
  float y;
  float z;

  /**
   * @brief A zero vector (0,0,0)
   */
  static const Vector3 ZERO;

  /**
   * @brief A unit vector (1,1,1)
   */
  static const Vector3 UNIT;

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
  
  const Vector3&
  operator=(const Vector3& inVec) {
    x = inVec.x;
    y = inVec.y;
    z = inVec.z;
    return *this;
  }
  
  bool
  operator==(const Vector3& inVec) const {
    return x == inVec.x && y == inVec.y && z == inVec.z;
  }

  bool
  operator!=(const Vector3& inVec) const {
    return !(*this == inVec);
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
  operator!=(const float& inVal) const {
    return (x != inVal) || (y != inVal) || (z != inVal);
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

class Vector2 {
 public:
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

class Vector2i {
 public:
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