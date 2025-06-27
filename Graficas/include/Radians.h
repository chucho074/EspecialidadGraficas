/**
 * @file    Radians.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    27/06/25
 * @brief   .
 */

/**
 * @include
 */
#pragma once

class Degrees;

class Radians 
{
 public:
  Radians() = default;
  
  Radians(float inVal);
  
  Radians(const Degrees& inDeg);
  
  
  ~Radians() = default;
  
  float 
  getRadians() const;
  
  float 
  getDegrees() const;
  
  Radians
  operator+(const Radians& inRad) const;
  
  Radians&
  operator+=(const Radians& inRad);
  
  Radians
  operator-() const ;
  
  Radians
  operator-(const Radians& inRad) const;
  
  Radians&
  operator-=(const Radians& inRad);
  
  Radians
  operator*(float inVal) const ;
  
  Radians
  operator*(const Radians& inVal) const;
  
  Radians&
  operator*=(float inVal);
  
  Radians
  operator/(float inVal) const;
  
  Radians&
  operator/=(float inVal);
  
  bool
  operator<(const Radians& inRad) const;
  
  bool
  operator<=(const Radians& inRad) const;
  
  bool
  operator==(const Radians& inRad) const;
  
  bool
  operator!=(const Radians& inRad) const;
  
  bool
  operator>=(const Radians& inRad) const;
  
  bool
  operator>(const Radians& inRad) const;
  
 private:

   float m_radianAngle;
};