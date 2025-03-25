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

struct FaceVertex {
  int32 vertex_index = -1;
  int32 uv_index = -1;

  bool 
  operator==(const FaceVertex& vertex) const {
    return vertex_index == vertex.vertex_index && uv_index == vertex.uv_index;
  }
};

namespace std {
  template<>
  struct hash<FaceVertex> {
    size_t operator()(const FaceVertex& vertex) const {
      return hash<int32>()(vertex.vertex_index)
             ^ hash<int32>()(vertex.uv_index);
    }
  };
}

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


  Vector<Vector3> temp_pos;
  Vector<Vector2> temp_tc;
  UMap<FaceVertex, uint16> uniqueVertices;


  int32 vt_index = 0;
  for(const auto& line : lines) {
    Vector<String> tokens = split(line, ' ');
    if (tokens.empty()) {
      continue;
    }
    
    if (tokens[0] == "v") {
      Vector3 pos;
      pos.x = std::stof(tokens[1]);
      pos.y = std::stof(tokens[2]);
      pos.z = std::stof(tokens[3]);
      
      temp_pos.push_back(pos);
    }

    else if (tokens[0] == "vt") {
      Vector2 uv;
      uv.x = std::stof(tokens[1]);
      uv.y = std::stof(tokens[2]);
      temp_tc.push_back(uv);
    }

    else if (tokens[0] == "f") {
      Vector<uint16> faceIndex;

      assert(tokens.size() == 4);
      for (size_t i = 1; i < tokens.size(); ++i) {
        Vector<String> fi = split(tokens[i], '/');
        
        FaceVertex fv;

        fv.vertex_index = std::stoi(fi[0]) - 1;
        fv.uv_index = std::stoi(fi[1]) - 1;

        

        if (uniqueVertices.find(fv) == uniqueVertices.end()) {
          uniqueVertices[fv] = static_cast<uint16>(vertices.size());

          SimpleVertex mvertex;
          mvertex.position = temp_pos[fv.vertex_index];
          mvertex.color = Vector3(1.f, 1.f, 1.f);
          mvertex.u = temp_tc[fv.uv_index].x;
          mvertex.v = 1.0 - temp_tc[fv.uv_index].y;

          vertices.push_back(mvertex);
        }

        faceIndex.push_back(uniqueVertices[fv]);
      }

      indices.push_back(faceIndex[0]);
      indices.push_back(faceIndex[1]);
      indices.push_back(faceIndex[2]);
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

void 
Model::draw(const UPtr<GraphicsAPI>& inGAPI) {
  inGAPI->m_pDeviceContext->DrawIndexed(m_meshes[0].numIndices,
                                        m_meshes[0].baseIndex, 
                                        m_meshes[0].baseVertex);
}

