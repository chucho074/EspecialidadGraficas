/**
 * @file    Radians.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    27/06/25
 */

/**
 * @include
 */
#include "Radians.h"
#include "Degrees.h"
#include "mathObjects.h"

Radians::Radians(float inVal) {
  m_radianAngle = inVal;
}

Radians::Radians(const Degrees& inDeg) {
  m_radianAngle = inDeg.getRadians();
}

float
Radians::getRadians() const {
  return m_radianAngle;
}

float
Radians::getDegrees() const {
  return getRadians() * RAD2DEG;
}

Radians 
Radians::operator+(const Radians& inRad) const {
  return Radians(getRadians() + inRad.getRadians());
}

Radians& 
Radians::operator+=(const Radians& inRad) {
  m_radianAngle += inRad.getRadians();
  return *this;
}

Radians 
Radians::operator-() const {
  return Radians(-getRadians());
}

Radians 
Radians::operator-(const Radians& inRad) const {
  return Radians(getRadians() - inRad.getRadians());
}

Radians& 
Radians::operator-=(const Radians& inRad) {
  m_radianAngle -= inRad.getRadians();
  return *this;
}

Radians 
Radians::operator*(float inFloat) const {
  return Radians(getRadians() * inFloat);
}

Radians 
Radians::operator*(const Radians& inRad) const {
  return Radians(getRadians() * inRad.getRadians());
}

Radians& 
Radians::operator*=(float inFloat) {
  m_radianAngle *= inFloat;
  return *this;
}

Radians 
Radians::operator/(float inFloat) const {
  return Radians(getRadians() / inFloat);
}

Radians& 
Radians::operator/=(float inFloat) {
  m_radianAngle /= inFloat;
  return *this;
}

bool 
Radians::operator<(const Radians& inRad) const {
  return getRadians() < inRad.getRadians();
}

bool 
Radians::operator<=(const Radians& inRad) const {
  return getRadians() <= inRad.getRadians();
}

bool 
Radians::operator==(const Radians& inRad) const {
  return getRadians() == inRad.getRadians();
}

bool 
Radians::operator!=(const Radians& inRad) const {
  return getRadians() != inRad.getRadians();
}

bool 
Radians::operator>=(const Radians& inRad) const {
  return getRadians() >= inRad.getRadians();
}

bool 
Radians::operator>(const Radians& inRad) const {
  return getRadians() > inRad.getRadians();
}