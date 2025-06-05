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

const Vector3 Vector3::ZERO = Vector3(0.f, 0.f, 0.f);
const Vector3 Vector3::UNIT = Vector3(1.f, 1.f, 1.f);

const Vector4 Vector4::ZERO = Vector4(0.f, 0.f, 0.f, 0.f);
const Vector4 Vector4::UNIT = Vector4(1.f, 1.f, 1.f, 1.f);

const Vector3 Vector3::UP    = Vector3(0.f, 1.f, 0.f);
const Vector3 Vector3::RIGHT = Vector3(1.f, 0.f, 0.f);
const Vector3 Vector3::FRONT = Vector3(0.f, 0.f, 1.f);
