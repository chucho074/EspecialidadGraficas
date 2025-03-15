/**
 * @file    Mesh.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    15/03/25
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"

struct MeshData
{
  int32 topology;

  int32 baseVertex;
  int32 numVertices;

  int32 baseIndex;
  int32 numIndices;

};