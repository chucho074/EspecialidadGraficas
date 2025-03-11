/**
 * @file    Vectors.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    08/03/25
 */

/**
 * @include
 */
#include "Vectors.h"
#include "Matrix4.h"

Vector3 
Vector3::operator*(const Matrix3& inMatrix) const {
  return { x * inMatrix.m[0][0] + y * inMatrix.m[0][1] + z * inMatrix.m[0][2],
             x * inMatrix.m[1][0] + y * inMatrix.m[1][1] + z * inMatrix.m[1][2],
             x * inMatrix.m[2][0] + y * inMatrix.m[2][1] + z * inMatrix.m[2][2] };
}

void 
Vector3::operator*=(const Matrix4& inMatrix) {
  x = x * inMatrix.m[0][0] + y * inMatrix.m[0][1] + z * inMatrix.m[0][2] + 1 * inMatrix.m[0][3];
  y = x * inMatrix.m[1][0] + y * inMatrix.m[1][1] + z * inMatrix.m[1][2] + 1 * inMatrix.m[1][3];  // Si es posicion es 1, si es direccion es 0 en la W
  z = x * inMatrix.m[2][0] + y * inMatrix.m[2][1] + z * inMatrix.m[2][2] + 1 * inMatrix.m[2][3];
}
