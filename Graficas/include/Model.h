/**
 * @file    Model.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    15/03/25
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "Buffers.h"
#include "Transform.h"
#include "Mesh.h"

class GraphicsAPI;

struct SimpleVertex {
  Vector3 position;
  Vector3 color;
  Vector3 normal;
  Vector3 tangent;
  float u, v;
};

class Model
{
 public:
  Model() = default;
  ~Model() = default;

  bool
  loadFromFile(const Path& inPath);

  void
  computeTangentSpace();


  bool 
  loadFromMem(const Vector<SimpleVertex>& inVertexData,
              const Vector<uint32>& inIndexData);

  void
  setBuffers();

  void
  draw();

  Vector<MeshData> m_meshes;

  Vector<SimpleVertex> m_vertices;
  Vector<uint32> m_indices;

 protected:
  SPtr<GraphicsBuffers> m_pVertexBuffer;
  SPtr<GraphicsBuffers> m_pIndexBuffer;
};