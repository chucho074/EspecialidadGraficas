/**
 * @file    Degrees.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    27/06/25
 */

/**
 * @include
 */
#pragma once
class Radians;

class Degrees 
{
 public:
  Degrees() = default;
  
  Degrees(float inVal);
  
  Degrees(const Radians& inRad);
  
  ~Degrees() = default;
  
  float 
  getRadians() const;
  
  float 
  getDegrees() const;
  
  Degrees
  operator+(const Degrees& inDeg) const;
  
  Degrees&
  operator+=(const Degrees& inDeg);
  
  Degrees
  operator-() const;
  
  Degrees
  operator-(const Degrees& inDeg) const;
  
  Degrees&
  operator-=(const Degrees& inDeg);
  
  Degrees
  operator*(const Degrees& inDeg) const;
  
  bool
  operator<(const Degrees& inDeg) const;
  
  bool
  operator<=(const Degrees& inDeg) const;
  
  bool
  operator==(const Degrees& inDeg) const;
  
  bool
  operator!=(const Degrees& inDeg) const;
  
  bool
  operator>=(const Degrees& inDeg) const;
  
  bool 
  operator>(const Degrees& inDeg) const;
   
 private:
  float m_degreesAngle;
};
