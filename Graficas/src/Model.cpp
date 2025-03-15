/**
 * @file    Model.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    15/03/25
 */

/**
 * @include
 */
#include "Model.h"
#include "GraphicsAPI.h"

bool 
Model::loadFromFile(const Path& inPath, const UPtr<GraphicsAPI>& inGAPI) {
  
  fstream objFile(inPath, ios::in | ios::ate);
  if (!objFile.is_open()) {
    return false;
  }

  auto fileSize = objFile.tellg();
  objFile.seekp(ios::beg);

  String fileData;
  fileData.resize(fileSize);
  objFile.read(&fileData[0], fileSize);

  Vector<String> lines = split(fileData, '\n');
  Vector<SimpleVertex> vertices;
  Vector<uint16> indices;

  for(const auto& line : lines) {
    Vector<String> tokens = split(line, ' ');
    if (tokens.empty()) {
      continue;
    }
    
    if (tokens[0] == "v") {
      SimpleVertex vertex;
      vertex.position.x = std::stof(tokens[1]);
      vertex.position.y = std::stof(tokens[2]);
      vertex.position.z = std::stof(tokens[3]);
      vertex.color = Vector3( rand() / float(RAND_MAX), 
                              rand() / float(RAND_MAX), 
                              rand() / float(RAND_MAX));
      vertices.push_back(vertex);
    }

    else if (tokens[0] == "f") {
      assert(tokens.size() == 4);
      for (size_t i = 1; i < tokens.size(); ++i) {
        uint16 index = std::stoi(tokens[i]) - 1;
        indices.push_back(index);
      }
    }
  }

  m_meshes.resize(1);
  auto& mesh = m_meshes[0];
  mesh.baseVertex = 0;
  mesh.numVertices = vertices.size();

  mesh.baseIndex = 0;
  mesh.numIndices = indices.size();

  mesh.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


  Vector<char> tmpVertexData;
  tmpVertexData.resize(vertices.size() * sizeof(SimpleVertex));
  memcpy(tmpVertexData.data(), vertices.data(), vertices.size() * sizeof(SimpleVertex));
  m_pVertexBuffer = inGAPI->createVertexBuffer(tmpVertexData);

  if (!m_pVertexBuffer) {
    return false;
  }

  Vector<char> tmpIndexData;
  tmpIndexData.resize(indices.size() * sizeof(uint16));
  memcpy(tmpIndexData.data(), indices.data(), indices.size() * sizeof(uint16));
  m_pIndexBuffer = inGAPI->createIndexBuffer(tmpIndexData);

  if (!m_pIndexBuffer) {
    return false;
  }
  
  //__debugbreak();
  return true;
}

void 
Model::setBuffers(const UPtr<GraphicsAPI>& inGAPI) {
  UINT stride = sizeof(SimpleVertex);
  UINT offset = 0;
  inGAPI->m_pDeviceContext->IASetVertexBuffers(0, 
                                                1,
                                                &m_pVertexBuffer->m_pBuffer,
                                                &stride,
                                                &offset);


  inGAPI->m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer->m_pBuffer,
                                              DXGI_FORMAT_R16_UINT,
                                              0);
}

