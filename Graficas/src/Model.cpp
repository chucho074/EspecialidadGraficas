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
  int32 normal_index = -1;

  bool 
  operator==(const FaceVertex& vertex) const {
    return vertex_index == vertex.vertex_index 
           && uv_index == vertex.uv_index 
           && normal_index == vertex.normal_index;
  }
};

namespace std {
  template<>
  struct hash<FaceVertex> {
    size_t 
    operator()(const FaceVertex& vertex) const {
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
  //Vector<SimpleVertex> vertices;
  //Vector<uint32> indices;

  Vector<Vector3> temp_pos;
  Vector<Vector2> temp_tc;
  Vector<Vector3> temp_norm;
  UMap<FaceVertex, uint32> uniqueVertices;

  int32 vt_index = 0;
  for(const auto& line : lines) {
    Vector<String> tokens = split(line, ' ');
    if (tokens.empty()) {
      continue;
    }

    if(tokens[0] == "#") {
      continue;
    }
    /*if(tokens[0] == " ") {
      continue;
    }*/
    
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
    else if(tokens[0] == "vn") {
      Vector3 normal;
      normal.x = std::stof(tokens[1]);
      normal.y = std::stof(tokens[2]);
      normal.z = std::stof(tokens[3]);
      temp_norm.push_back(normal);
    }

    else if (tokens[0] == "f") {
      Vector<uint32> faceIndex;

      assert(tokens.size() == 4);
      for (size_t i = 1; i < tokens.size(); ++i) {
        Vector<String> fi = split(tokens[i], '/');
        
        FaceVertex fv;

        fv.vertex_index = std::stoi(fi[0]) - 1;
        fv.uv_index = std::stoi(fi[1]) - 1;
        fv.normal_index = std::stoi(fi[2]) - 1;

        if (uniqueVertices.find(fv) == uniqueVertices.end()) {
          uniqueVertices[fv] = static_cast<uint32>(m_vertices.size());

          SimpleVertex mvertex;
          mvertex.position = temp_pos[fv.vertex_index];
          mvertex.color = Vector3(1.f, 1.f, 1.f);
          mvertex.normal = temp_norm[fv.normal_index];
          mvertex.u = temp_tc[fv.uv_index].x;
          mvertex.v = 1.0 - temp_tc[fv.uv_index].y;

          m_vertices.push_back(mvertex);
        }

        faceIndex.push_back(uniqueVertices[fv]);
      }

      m_indices.push_back(faceIndex[0]);
      m_indices.push_back(faceIndex[1]);
      m_indices.push_back(faceIndex[2]);
    }
  }

  m_meshes.resize(1);
  auto& mesh = m_meshes[0];
  mesh.baseVertex = 0;
  mesh.numVertices = m_vertices.size();

  mesh.baseIndex = 0;
  mesh.numIndices = m_indices.size();

  mesh.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

  //Compute tangents
  computeTangentSpace();

  //Create the information of the buffers.
  Vector<char> tmpVertexData;
  tmpVertexData.resize(m_vertices.size() * sizeof(SimpleVertex));
  memcpy(tmpVertexData.data(), m_vertices.data(), m_vertices.size() * sizeof(SimpleVertex));
  m_pVertexBuffer = inGAPI->createVertexBuffer(tmpVertexData);

  if (!m_pVertexBuffer) {
    return false;
  }

  Vector<char> tmpIndexData;
  tmpIndexData.resize(m_indices.size() * sizeof(uint32));
  memcpy(tmpIndexData.data(), m_indices.data(), m_indices.size() * sizeof(uint32));
  m_pIndexBuffer = inGAPI->createIndexBuffer(tmpIndexData);

  if (!m_pIndexBuffer) {
    return false;
  }
  
  //__debugbreak();
  return true;
}

void 
Model::computeTangentSpace() {
  Vector<SimpleVertex>& vertices = m_vertices;
  Vector<uint32>& indices = m_indices;
  //Compute tangents
  Vector<Vector3> tan1(vertices.size(), Vector3(0.f, 0.f, 0.f));
  Vector<Vector3> tan2(vertices.size(), Vector3(0.f, 0.f, 0.f));

  for(auto& mesh : m_meshes) {
    if(mesh.topology != D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
      continue;
    }

    int32 numTriangles = mesh.numIndices / 3;

    for(int32 t = 0; t < numTriangles; ++t) {
      int32 i = mesh.baseIndex + (t * 3);
      uint32 i1 = indices[i];
      uint32 i2 = indices[i + 1];
      uint32 i3 = indices[i + 2];
      
      const Vector3& v1 = vertices[i1].position;
      const Vector3& v2 = vertices[i2].position;
      const Vector3& v3 = vertices[i3].position;

      const Vector2& w1 = Vector2(vertices[i1].u, vertices[i1].v);
      const Vector2& w2 = Vector2(vertices[i2].u, vertices[i2].v);
      const Vector2& w3 = Vector2(vertices[i3].u, vertices[i3].v);

      float x1 = v2.x - v1.x;
      float x2 = v3.x - v1.x;
      float y1 = v2.y - v1.y;
      float y2 = v3.y - v1.y;
      float z1 = v2.z - v1.z;
      float z2 = v3.z - v1.z;

      float s1 = w2.x - w1.x;
      float s2 = w3.x - w1.x;
      float t1 = w2.y - w1.y;
      float t2 = w3.y - w1.y;

      float denominator = s1 * t2 - s2 * t1;
      if (denominator == 0.f) {
        continue;
      }

      float r = 1.f / denominator;

      Vector3 sdir = Vector3((t2 * x1 - t1 * x2) * r,
                             (t2 * y1 - t1 * y2) * r,
                             (t2 * z1 - t1 * z2) * r);

      Vector3 tdir = Vector3((-s2 * x1 + s1 * x2) * r,
                             (-s2 * y1 + s1 * y2) * r,
                             (-s2 * z1 + s1 * z2) * r);

      tan1[i1] += sdir;
      tan1[i2] += sdir;
      tan1[i3] += sdir;

      tan2[i1] += tdir;
      tan2[i2] += tdir;
      tan2[i3] += tdir;
    }

    for (int32 i = mesh.baseVertex; i < mesh.baseVertex + mesh.numVertices; ++i) {
      const Vector3& n = vertices[i].normal;
      
      if (std::abs(n.y) > 0.999f) {
        //Esta en un polo
        Vector3 helper = (n.dot(Vector3(0.f, 1.f, 0.f))) < 0.999f ? 
                               Vector3(1.f, 0.f, 0.f) : 
                               Vector3(0.f, 0.f, 1.f);

        vertices[i].tangent = (n.cross(helper)).normalize();
        continue;

      }

      //Gram-Schmidt orthogonalize
      const Vector3& t = tan1[i];
      auto tmpTang = t - n * n.dot(t);
      
      if(tmpTang == Vector3(0.f, 0.f, 0.f)) {
        Vector3 helper = (n.z) < 0.999f ? 
                               Vector3(0.f, 0.f, 1.f) : 
                               Vector3(1.f, 0.f, 0.f);

        continue;
      }
      vertices[i].tangent = tmpTang.normalize();

    }
  }

}

bool 
Model::loadFromMem(const Vector<SimpleVertex>& inVertexData, 
                   const Vector<uint32>& inIndexData, 
                   const UPtr<GraphicsAPI>& inGAPI) {

  m_meshes.resize(1);
  auto& mesh = m_meshes[0];
  mesh.baseVertex = 0;
  mesh.numVertices = inVertexData.size();

  mesh.baseIndex = 0;
  mesh.numIndices = inIndexData.size();

  mesh.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

  Vector<char> tmpVertexData;
  tmpVertexData.resize(inVertexData.size() * sizeof(SimpleVertex));
  memcpy(tmpVertexData.data(), 
         inVertexData.data(), 
         inVertexData.size() * sizeof(SimpleVertex));
  m_pVertexBuffer = inGAPI->createVertexBuffer(tmpVertexData);

  if(!m_pVertexBuffer) {
    return false;
  }

  Vector<char> tmpIndexData;
  tmpIndexData.resize(inIndexData.size() * sizeof(uint32));
  memcpy(tmpIndexData.data(), 
         inIndexData.data(), 
         inIndexData.size() * sizeof(uint32));
  m_pIndexBuffer = inGAPI->createIndexBuffer(tmpIndexData);

  if(!m_pIndexBuffer) {
    return false;
  }

  return true;
}

void 
Model::setBuffers(const UPtr<GraphicsAPI>& inGAPI) {
  uint32 stride = sizeof(SimpleVertex);
  uint32 offset = 0;
  inGAPI->m_pDeviceContext->IASetVertexBuffers(0, 
                                               1,
                                               &m_pVertexBuffer->m_pBuffer,
                                               &stride,
                                               &offset);


  inGAPI->m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer->m_pBuffer,
                                             DXGI_FORMAT_R32_UINT,
                                             0);

}

void 
Model::draw(const UPtr<GraphicsAPI>& inGAPI) {
  inGAPI->m_pDeviceContext->DrawIndexed(m_meshes[0].numIndices,
                                        m_meshes[0].baseIndex, 
                                        m_meshes[0].baseVertex);
}

