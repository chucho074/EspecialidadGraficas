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

#include "Mesh.h"

class GraphicsAPI;

struct SimpleVertex {
  Vector3 position;
  Vector3 color;
  float u, v;
};

class Model
{
 public:
	Model() = default;
	~Model() = default;

  bool
  loadFromFile(const Path& inPath, const UPtr<GraphicsAPI>& inGAPI);

  void
  setBuffers(const UPtr<GraphicsAPI>& inGAPI);

  void
  draw(const UPtr<GraphicsAPI>& inGAPI);

  Vector<MeshData> m_meshes;

 protected:
  UPtr<GraphicsBuffers> m_pVertexBuffer;
  UPtr<GraphicsBuffers> m_pIndexBuffer;
};