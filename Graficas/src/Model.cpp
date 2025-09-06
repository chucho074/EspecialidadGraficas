/**
 * @file    Model.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    15/03/25
 */

/**
 * @include
 */

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Model.h"
#include "GraphicsAPI.h"
#include "ShaderManager.h"

#define MIN_SPHERE_SECTOR 3
#define MIN_SPHERE_STACK 2

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

void
processNode(Model& inModel,
            aiNode* node,
            const aiScene* inScene,
            bool saveMat = true);

MeshData
processMesh(Model& inModel,
            aiMesh* mesh,
            const aiScene* scene,
            bool saveMat);

SPtr<Image>
loadMaterialTexture(Model& inModel,
                    aiMaterial* mat,
                    aiTextureType type);

Path
getTexturePath(Model& inModel,
               aiMaterial* mat,
               aiTextureType type);

String 
removeDoubleDots(String str) {
  const String target = "..";
  size_t pos;
  while((pos = str.find(target)) != String::npos) {
    str.erase(pos, target.length());
  }
  return str;
}

/////////////////////////////////////////////////////////////////////////////////////////////

bool
Model::loadFromFile(const Path& inPath) {
  
  loadFromAssimp(inPath);
  return true;


  auto& GAPI = g_graphicsAPI();
  
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
  objFile.close();
  m_meshes.resize(1);
  auto& mesh = m_meshes[0];
  mesh.baseVertex = 0;
  mesh.numVertices = m_vertices.size();

  mesh.baseIndex = 0;
  mesh.numIndices = m_indices.size();

  mesh.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

  //Compute tangents
  //computeNormals();
  //computeTangentSpace();

  //Create the buffers
  if(!createBuffers()) {
    return false;
  }
  
  ConsoleOut<< "The model \"" << inPath.string() << "\" is loaded." << ConsoleLine;

  //__debugbreak();
  return true;
}

void 
Model::loadFromAssimp(const Path& inPath) {
  if(!fsys::exists(inPath)) {
    return;
  }

  Assimp::Importer importer;

  importer.ReadFile(inPath.string(),
                    aiProcessPreset_TargetRealtime_MaxQuality |
                    aiProcess_TransformUVCoords |
                    aiProcess_ConvertToLeftHanded |
                    aiProcess_Triangulate);

  const aiScene* tmpScene = importer.GetOrphanedScene();

  if(!tmpScene
     || tmpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
     || !tmpScene->mRootNode) {
    
    __debugbreak();
    return;
  }

  m_path = inPath;

  processNode(*this, tmpScene->mRootNode, tmpScene, true);

  //Compute tangents
  //computeNormals();
  //computeTangentSpace();

  //Create the buffers
  if(!createBuffers()) {
    return;
  }

  ConsoleOut << "The model \"" << inPath.string() << "\" is loaded." << ConsoleLine;

  //__debugbreak();
  return;
}

bool 
Model::loadFromBin(const Path& inPath) {
  fstream objFile(inPath, ios::in | ios::beg | ios::binary);
  if(!objFile.is_open()) {
    return false;
  }

  // Read the mesh data
  int32 meshCount;
  objFile.read(reinterpret_cast<char*>(&meshCount), sizeof(int32));
  m_meshes.resize(meshCount);
  objFile.read(reinterpret_cast<char*>(m_meshes.data()), sizeof(MeshData) * meshCount);

  // Read the vertices
  int32 vertexCount;
  objFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(int32));
  m_vertices.resize(vertexCount);
  objFile.read(reinterpret_cast<char*>(m_vertices.data()), sizeof(SimpleVertex) * vertexCount);

  // Read the indices
  int32 indexCount;
  objFile.read(reinterpret_cast<char*>(&indexCount), sizeof(int32));
  m_indices.resize(indexCount);
  objFile.read(reinterpret_cast<char*>(m_indices.data()), sizeof(uint32) * indexCount);
  
  // Close the file
  objFile.close();

  //computeNormals();

  //Create the buffers
  if(!createBuffers()) {
    return false;
  }

  ConsoleOut << "The model \"" << inPath.string() << "\" is loaded." << ConsoleLine;
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

void 
Model::computeNormals() {
  Vector<SimpleVertex>& vertices = m_vertices;
  Vector<uint32>& indices = m_indices;

  for (auto& mesh : m_meshes) {
    for(int32 i = 0; i < mesh.numIndices; i+=3) {

      auto& v0 = vertices[indices[i]].position;
      auto& v1 = vertices[indices[i+1]].position;
      auto& v2 = vertices[indices[i+2]].position;

      auto A = v1 - v0;
      auto B = v2 - v0;

      Vector3 normal = A.cross(B).normalize();
      
      vertices[indices[i+0]].normal += normal;
      vertices[indices[i+1]].normal += normal;
      vertices[indices[i+2]].normal += normal;

      //vertices[indices[i]].normal = vertices[indices[i]].position 
      //                              + vertices[indices[i+1]].position 
      //                              + vertices[indices[i+2]].position;
      //vertices[indices[i]].normal.normalize();
    }

    for (auto& vertex : vertices) {
      vertex.normal.normalize();
    }

    /*for(int32 i = 0; i < vertices.size(); ++i) {
      vertices[i].normal = normals[i];
    }*/
  }
}

bool 
Model::loadFromMem(const Vector<SimpleVertex>& inVertexData, 
                   const Vector<uint32>& inIndexData) {

  auto& GAPI = g_graphicsAPI();

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
  m_pVertexBuffer = GAPI.createVertexBuffer(tmpVertexData);

  if(!m_pVertexBuffer) {
    return false;
  }

  Vector<char> tmpIndexData;
  tmpIndexData.resize(inIndexData.size() * sizeof(uint32));
  memcpy(tmpIndexData.data(), 
         inIndexData.data(), 
         inIndexData.size() * sizeof(uint32));
  m_pIndexBuffer = GAPI.createIndexBuffer(tmpIndexData);

  if(!m_pIndexBuffer) {
    return false;
  }

  return true;
}

bool
Model::createBuffers() {
  auto& GAPI = g_graphicsAPI();

  // Create the information of the buffers.
  Vector<char> tmpVertexData;
  tmpVertexData.resize(m_vertices.size() * sizeof(SimpleVertex));
  memcpy(tmpVertexData.data(), m_vertices.data(), m_vertices.size() * sizeof(SimpleVertex));
  m_pVertexBuffer = GAPI.createVertexBuffer(tmpVertexData);

  if(!m_pVertexBuffer) {
    __debugbreak();
    return false;
  }

  Vector<char> tmpIndexData;
  tmpIndexData.resize(m_indices.size() * sizeof(uint32));
  memcpy(tmpIndexData.data(), m_indices.data(), m_indices.size() * sizeof(uint32));
  m_pIndexBuffer = GAPI.createIndexBuffer(tmpIndexData);

  if(!m_pIndexBuffer) {
    __debugbreak();
    return false;
  }
}

void 
Model::createSphere(int32 inNumTriangles) {
  float radius = 1;
  uint32 sectors = MIN_SPHERE_SECTOR;
  for(int32 i = MIN_SPHERE_SECTOR; (((inNumTriangles) % i) == 0); i += 3) {
    sectors = i;
  }

  uint32 stacks = ((inNumTriangles) / sectors) / 2;

  uint32 sphereSectors = sectors < MIN_SPHERE_SECTOR ? MIN_SPHERE_SECTOR : sectors;
  uint32 sphereStacks = stacks < MIN_SPHERE_STACK ? MIN_SPHERE_STACK : stacks;

  float x, y, z, xy;
  float nx, ny, nz, lengthInv = 1.0f / radius;
  float s, t;

  float sectorStep = 2 * PI / sphereSectors;
  float sectorAngle, stackAngle;

  Vector<SimpleVertex> sphereVertices;
  Vector<uint32> sphereIndices;
  //Vector<ResourceRef> sphereTextures;
  SimpleVertex vertex;
  vertex.tangent = Vector3(1.0f, 1.0f, 1.0f);
  //vertex.BiNor = Vector3(1.0f, 1.0f, 1.0f);

  sphereVertices.resize((sphereSectors * (sphereStacks - 1)) + 2);
  int32 tmpIter = 0;
  tmpIter++;

  //First Vertex data.
  SimpleVertex tmpVertex;
  tmpVertex.position = {0, 1, 0};
  tmpVertex.normal = {0, 1, 0};
  tmpVertex.u = 0.f;
  tmpVertex.v = 1.f;
  sphereVertices[0] = tmpVertex;

  //Vertices
  for(uint32 i = 0; i < sphereStacks - 1; ++i) {
    auto phi = PI * double(i + 1) / double(sphereStacks);
    for(uint32 j = 0; j < sphereSectors; ++j) {
      sectorAngle = 2.0 * PI * double(j) / double(sphereSectors);

      //Vertex
      x = sin(phi) * cos(sectorAngle);
      y = cos(phi);
      z = sin(phi) * sin(sectorAngle);
      vertex.position = Vector3(x, y, z);
      //Normal
      nx = x * lengthInv;
      ny = y * lengthInv;
      nz = z * lengthInv;
      vertex.normal = Vector3(nx, ny, nz);
      //Texcoords
      s = (float)j / sphereSectors;
      t = (float)i / sphereStacks;
      Vector2 tmpTex = Vector2(s, t);
      tmpTex = tmpTex.normalize();
      vertex.u = s;
      vertex.v = t;
      if(tmpIter >= ((sphereSectors * (sphereStacks - 1)) + 2)) {
        __debugbreak();
        tmpIter = tmpIter - 1;
        break;
      }
      else {
        sphereVertices[tmpIter] = vertex;
        tmpIter++;
      }
    }
  }
  //Last vertex data.
  tmpVertex.position = {0, -1, 0};
  tmpVertex.normal = {0, -1, 0};
  tmpVertex.u = 0.5f;
  tmpVertex.v = 0.5f;
  sphereVertices[tmpIter] = tmpVertex;

  //Indices

  //Triangles with the first Vertex.
  for(int32 i = 0; i < sphereSectors; ++i) {
    auto i0 = i + 1;
    auto i1 = (i + 1) % sphereSectors + 1;
    sphereIndices.push_back(0);
    sphereIndices.push_back(i0);
    sphereIndices.push_back(i1);
    i0 = i + sphereSectors * (sphereStacks - 2) + 1;
    i1 = (i + 1) % sphereSectors + sphereSectors * (sphereStacks - 2) + 1;
    sphereIndices.push_back(tmpIter);
    sphereIndices.push_back(i0);
    sphereIndices.push_back(i1);
  }

  //Middle triangles
  for(int32 i = 0; i < sphereStacks - 2; ++i) {
    auto i0 = i * sphereSectors + 1;
    auto i1 = (i + 1) * sphereSectors + 1;
    for(int32 j = 0; j < sphereSectors; ++j) {
      auto j0 = i0 + j;
      auto j1 = i0 + (j + 1) % sphereSectors;
      auto j2 = i1 + (j + 1) % sphereSectors;
      auto j3 = i1 + j;
      sphereIndices.push_back(j0);
      sphereIndices.push_back(j1);
      sphereIndices.push_back(j2);

      sphereIndices.push_back(j0);
      sphereIndices.push_back(j2);
      sphereIndices.push_back(j3);

    }
  }


  loadFromMem(sphereVertices, sphereIndices);
  m_meshes[0].meshMaterial.setShaderRef(g_shaderManager().getDefaultShader());
}

void 
Model::setBuffers() {

  auto& GAPI = g_graphicsAPI();
  
  if (!m_pVertexBuffer || !m_pIndexBuffer) {
    return; // Maybe create the buffers if a model info exists.
  }

  uint32 stride = sizeof(SimpleVertex);
  uint32 offset = 0;
  GAPI.m_pDeviceContext->IASetVertexBuffers(0, 
                                            1,
                                            &m_pVertexBuffer->m_pBuffer,
                                            &stride,
                                            &offset);


  GAPI.m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer->m_pBuffer,
                                          DXGI_FORMAT_R32_UINT,
                                          0);

}

void 
Model::draw(bool inWithMaterial) {
  auto& GAPI = g_graphicsAPI();
  
  if (!m_pVertexBuffer || !m_pIndexBuffer) {
    //Logger xd
    return;
  }

  setBuffers();

  for (auto& mesh : m_meshes) {
    if(4 == mesh.topology) { //If is a triangle
      GAPI.setTopology(mesh.topology);
      
      if(inWithMaterial) {
        mesh.meshMaterial.draw();
      }

      GAPI.m_pDeviceContext->DrawIndexed(mesh.numIndices, 
                                         mesh.baseIndex,
                                         mesh.baseVertex);
    }
  }
}

void 
Model::exportToFile(Path inExportPath) {
  ofstream outputFile(inExportPath, std::ios::binary);
  if(outputFile.is_open()) {
    // Mesh data
    // Write the number of meshes
    int32 meshCount = m_meshes.size();
    outputFile.write(reinterpret_cast<char*>(&meshCount), sizeof(int32));
    outputFile.write(reinterpret_cast<const char*>(m_meshes.data()), sizeof(MeshData) * meshCount);

    // Vertex
    // Write the size of the vertices vector
    int32 vertexCount = m_vertices.size();
    outputFile.write(reinterpret_cast<char*>(&vertexCount), sizeof(int32));
    outputFile.write(reinterpret_cast<char*>(m_vertices.data()), sizeof(SimpleVertex) * vertexCount);

    // Index
    // Write the size of the indices vector
    int32 indexCount = m_indices.size();
    outputFile.write(reinterpret_cast<char*>(&indexCount), sizeof(int32));
    outputFile.write(reinterpret_cast<const char*>(m_indices.data()), sizeof(uint32) * indexCount);
  }
}

void 
processNode(Model& inModel, 
            aiNode* node, 
            const aiScene* inScene, 
            bool saveMat) {
  // process all the node's meshes (if any)
  for (uint32 i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = inScene->mMeshes[node->mMeshes[i]];
    inModel.m_meshes.push_back(processMesh(inModel, mesh, inScene, saveMat));
  }
  // then do the same for each of its children
  for (uint32 i = 0; i < node->mNumChildren; i++) {
    processNode(inModel, node->mChildren[i], inScene, saveMat);
  }
}


MeshData
processMesh(Model& inModel, 
            aiMesh* mesh, 
            const aiScene* scene, 
            bool saveMat) {

  auto& texManager = g_textureManager();

  MeshData outMesh;
  outMesh.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
  outMesh.baseVertex = inModel.m_vertices.size();
  outMesh.numVertices = mesh->mNumVertices;
  outMesh.baseIndex = inModel.m_indices.size();
  //outMesh.baseIndex = 0;
  outMesh.numIndices = mesh->mNumFaces * 3; //Assuming all faces are triangles
  outMesh.meshMaterial.setShaderRef(g_shaderManager().getDefaultShader());
  outMesh.meshName = mesh->mName.C_Str();

  for(uint32 i = 0; i < mesh->mNumVertices; i++) {
      SimpleVertex vertex;

      if(4 != mesh->mPrimitiveTypes) { //Verify the topology
        outMesh.topology = mesh->mPrimitiveTypes;
        //__debugbreak();
      }

      // process vertex positions, normals and texture coordinates
      //Pos
      vertex.position.x = mesh->mVertices[i].x;
      vertex.position.y = mesh->mVertices[i].y;
      vertex.position.z = mesh->mVertices[i].z;
      //Normals
      if(mesh->HasNormals()) {
        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;
      }
      else {
        vertex.normal.x = 0.0f;
        vertex.normal.y = 0.0f;
        vertex.normal.z = 0.0f;
      }
      //Texture / UVs
      if (mesh->HasTextureCoords(0)) {
        vertex.u = mesh->mTextureCoords[0][i].x;
        vertex.v = mesh->mTextureCoords[0][i].y;
      }
      else {
        vertex.u = 0.f;
        vertex.v = 0.f;
      }

      //Tangentes
      if(mesh->HasTangentsAndBitangents()) {
        vertex.tangent.x = mesh->mTangents[i].x;
        vertex.tangent.y = mesh->mTangents[i].y;
        vertex.tangent.z = mesh->mTangents[i].z;
      }
      else {
        vertex.tangent.x = 0.0f;
        vertex.tangent.y = 0.0f;
        vertex.tangent.z = 0.0f;
      }
      //Bitangentes
      /*if(mesh->mBitangents) {
        vertex.binormals.x = mesh->mBitangents[i].x;
        vertex.binormals.y = mesh->mBitangents[i].y;
        vertex.binormals.z = mesh->mBitangents[i].z;
      }
      else {
        vertex.BiNor.x = 0.0f;
        vertex.BiNor.y = 0.0f;
        vertex.BiNor.z = 0.0f;
      }*/

      inModel.m_vertices.push_back(vertex);
    }
  // Process indices
  for (uint32 i = 0; i < mesh->mNumFaces; ++i) {
    aiFace face = mesh->mFaces[i];
    for (uint32 j = 0; j < face.mNumIndices; ++j) {
      inModel.m_indices.push_back(face.mIndices[j]);
    }
  }

  if (saveMat) {
    // Process material
    if(mesh->mMaterialIndex >= 0) {
      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
      outMesh.meshMaterial.m_materialName = material->GetName().C_Str();

      ////To change for the creation of the textures in the resource Manager.
      TextureRef tmpAlbedo = texManager.loadTexture(getTexturePath(inModel, 
                                                                   material, 
                                                                   aiTextureType_DIFFUSE));
      if(tmpAlbedo) {
        outMesh.meshMaterial.setTexture(tmpAlbedo,
                                        TEXTURE_TYPE::kAlbedo);
      }
      else {
        outMesh.meshMaterial.setTexture(texManager.getDefaultTexture(),
                                        TEXTURE_TYPE::kAlbedo);
      }
           
      ////To change for the creation of the textures in the resource Manager.
      TextureRef tmpNormals = texManager.loadTexture(getTexturePath(inModel,
                                                                    material,
                                                                    aiTextureType_NORMALS));
                                                                    //aiTextureType_HEIGHT));
      if(tmpNormals) {
        outMesh.meshMaterial.setTexture(tmpNormals,
                                        TEXTURE_TYPE::kNormal);
      }
      else {
        outMesh.meshMaterial.setTexture(texManager.getDefaultNormalTexture(),
                                        TEXTURE_TYPE::kNormal);
      }

      ////To change for the creation of the textures in the resource Manager.
      TextureRef tmpSpecular = texManager.loadTexture(getTexturePath(inModel,
                                                                     material, 
                                                                     aiTextureType_SPECULAR));
      if(tmpSpecular) {
        outMesh.meshMaterial.setTexture(tmpSpecular,
                                        TEXTURE_TYPE::kSpecular);
      }
      else {
        outMesh.meshMaterial.setTexture(texManager.getDefaultTexture(),
                                        TEXTURE_TYPE::kSpecular);
      }

      ////To change for the creation of the textures in the resource Manager.
      TextureRef tmpMetallic = texManager.loadTexture(getTexturePath(inModel,
                                                                     material,
                                                                     aiTextureType_SHININESS));
      if(tmpMetallic) {
        outMesh.meshMaterial.setTexture(tmpMetallic,
                                        TEXTURE_TYPE::kMetallic);
      }
      else {
        outMesh.meshMaterial.setTexture(texManager.getDefaultTexture(),
                                        TEXTURE_TYPE::kMetallic);
      }
    }
  }


  return outMesh;
}

//////////////////////////////////////////////////////////////////////////////////////////////

//String 
//getPathCorrectly(String inFile) {
//  size_T realPos = 0;
//  size_T posInvSlash = inFile.rfind('\\');
//  size_T posSlash = inFile.rfind('/');
//
//  if(posInvSlash == String::npos) {
//    if(posSlash != String::npos) {
//      realPos = posSlash;
//    }
//  }
//  else {
//    realPos = posInvSlash;
//    if (posSlash == String::npos) {
//      if (posSlash > realPos) {
//        posSlash = realPos;
//      }
//    }
//  }
//  if (realPos == 0) {
//    //return "/" + inFile;
//  }
//
//  String prefix = "..\\";
//  if(inFile.rfind(prefix, 0) == 0) {
//    inFile.erase(0, prefix.size());
//  }
//
//  String tmpPath = inFile;
//  std::replace(tmpPath.begin(), tmpPath.end(), '\\', '/');
//  return tmpPath;
//  return "/" + inFile.substr(realPos + 1, inFile.length() - realPos);
//}

String
getAbsolutePath(const Path& inPath) {
  
  auto tmpPath = fsys::absolute(inPath);
  return tmpPath.string();
}

bool
existPath(const Path& inPath) {
  return fsys::exists(inPath);
}

String
getFileName(const Path& inPath) {
  
  inPath.filename();
  return inPath.string();
}

SPtr<Image>
loadMaterialTexture(Model& inModel,
                    aiMaterial* mat,
                    aiTextureType type) {

  Path outPath;

  SPtr<Image> outImage;

  bool noTexture = true;
  //Get the number of textures in assimp in the material.
  //Evaluate if is searching for normals and try heights if there is no normals available.
  int32 tmp = mat->GetTextureCount(type);

  if(type == aiTextureType_NORMALS && 0 == mat->GetTextureCount(type)) {
    type = aiTextureType_HEIGHT;
  }

  for(uint32 i = 0; i < mat->GetTextureCount(type); i++) {
    noTexture = false;
    aiString str;
    mat->GetTexture(type, i, &str);

    String tmpTextureName = str.C_Str();
    //Get just the name of the texture.
    Path tmpPath = tmpTextureName;
    Path tmpClean = tmpPath.lexically_normal();

    tmpTextureName = tmpClean.string();
    tmpTextureName = getAbsolutePath(tmpTextureName);
    tmpPath = tmpTextureName;

    bool skip = false;

    //for(uint32 j = 0; j < inModel.lock()->m_materialsLoaded.size(); j++) {
    //  //Get the list of names of textures in the material.
    //  auto tmpNames = RM.getTextureNameFromMaterial(inModel.lock()->m_materialsLoaded[j]);
    //  //Compare if the texture is already in any material by it name.
    //  auto tmpIterNames = find(tmpNames.begin(), tmpNames.end(), tmpTextureName);
    //  //If the Texture exist, get the reference of that texture.
    //  if(tmpIterNames != tmpNames.end()) {
    //    //Get the reference of the texture.
    //    tmpTextureRef = RM.getReferenceByNameInMaterial(inModel.lock()->m_materialsLoaded[j],
    //                                                    tmpIterNames->data());
    //    skip = true;
    //    break;
    //  }
    //}

    if(!skip) {   // if texture hasn't been loaded already, load it.

      //Get the path without the name of the model name.
      Path tmpParent = inModel.m_path.parent_path();
      outPath = tmpParent.parent_path().string() + tmpTextureName;

    }
  }

  if(noTexture) {
    //return "";
  }


//  __debugbreak();
  //return outPath;
  return outImage;
}

Path
getTexturePath(Model& inModel,
               aiMaterial* mat,
               aiTextureType type) {
  Path outPath;  

  bool hasTexture = true;
  //Get the number of textures in assimp in the material.
  int32 texCount = mat->GetTextureCount(type);

  if(type == aiTextureType_NORMALS && 0 == mat->GetTextureCount(type)) {
    type = aiTextureType_HEIGHT;
    texCount = mat->GetTextureCount(type);
  }

  // Verify if the material has any texture.
  for(uint32 i = 0; i < texCount; i++) {
    hasTexture = false;
    aiString str;
    mat->GetTexture(type, i, &str); // Gets the texture path 

    String tmpTextureName = str.C_Str();
    
    // If the texture has 2 dots, verify in the parent path.
    if (tmpTextureName.find("..") != String::npos) {
      //Get just the name of the texture.
      Path tmpAbsPath = getAbsolutePath(inModel.m_path.parent_path());
      tmpAbsPath.replace_filename("");
      String pathWithoutDots = removeDoubleDots(tmpTextureName);
      outPath = tmpAbsPath.string() + pathWithoutDots;
    }
    else { 
      // Obtain from the same folder as the model.
      Path tmpPath = getAbsolutePath(inModel.m_path.parent_path());
      outPath = tmpPath.string() + "/" + tmpTextureName;
    }

  }
  
  if(hasTexture) {
    return "";
  }
  //__debugbreak();
  return outPath;
}