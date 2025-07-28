/**
 * @file    ShaderManager.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    04/06/2025
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "Shader.h"
#include "Buffers.h"
#include "UUID.h"
#include "Module.h"
#include "GraphicsAPI.h"
#include "Transform.h"

////////////////////////////////////////////////////////////////////////////////////////////

struct ShaderRef {
  UID shaderID = UID::ZERO;  // Unique identifier for the shader
};

////////////////////////////////////////////////////////////////////////////////////////////

struct MatrixCollection {
  Matrix4 world;
  Matrix4 view;
  Matrix4 projection;

  Matrix4 lightView;
  Matrix4 lightProjection;

  Vector3 lightPosition;
  float lightIntensity;

  Vector3 lightColor;
  float lightRadius;  

  Vector3 viewDir;
  float time;
};

////////////////////////////////////////////////////////////////////////////////////////////

struct SAMPLER_USAGE {
  enum E {
    kPoint = 0,
    kLinear,
    kAnisotropic,
    kAll,
    kSamplerTypeCount
  };
};

////////////////////////////////////////////////////////////////////////////////////////////

struct RASTER_USAGE {
  enum E {
    kDefault = 0,
    kWireframe,
    kWireframe_NoCull,
    kCullFront,
    kRasterTypeCount
  };
};

////////////////////////////////////////////////////////////////////////////////////////////

class ShaderProgram {
 public:
  ShaderProgram() {
    m_vertexShader = make_shared<VertexShader>();
    m_pixelShader = make_shared<PixelShader>();
    m_inputLayout = nullptr;

    m_pCB_WVP = make_shared<GraphicsBuffers>();
  }

  ~ShaderProgram() {
    SAFE_RELEASE(m_inputLayout);
  }

  /**
   * @brief      Saves the information for the creation of the shader.
   * @param      inShaderPath  The path to the shader file.
   * @param      inVertexEntry (optional, Default: vs_main) The entry point to the vertex shader.
   * @param      inPixelEntry  (optional, Default: ps_main) The entry point to the pixel shader.
   * @param      inPixelPath   (optional, Default: inShaderPath) The path to the pixel shader file.
   */
  void
  setShaderData(Path inShaderPath, 
                String inVertexEntry = "", 
                String inPixelEntry = "",
                Path inPixelPath = "") {

    m_shaderPath = inShaderPath;
    m_pixelShaderPath = inPixelPath;
    m_vertexEntry = inVertexEntry;
    m_pixelEntry = inPixelEntry;
  }

  /**
   * @brief      Compiles the shader information.
   */
  void
  compileShaders() {
    auto& GAPI = g_graphicsAPI();

    auto pVertexShader = GAPI.createVertexShaderFromFile(m_shaderPath,
                                      (!m_vertexEntry.empty()) ? m_vertexEntry : "vs_main");
    if(pVertexShader) {
      m_vertexShader = std::move(pVertexShader);
      reflectInputLayout();
    }

    auto pPixelShader = GAPI.createPixelShaderFromFile((!m_pixelShaderPath.empty()) ? m_pixelShaderPath : m_shaderPath,
                                        (!m_pixelEntry.empty()) ? m_pixelEntry : "ps_main");
    
    if(pPixelShader) {
      m_pixelShader = std::move(pPixelShader);
    }

    Vector<char> emtyData;
    m_pCB_WVP = GAPI.createConstantBuffer(emtyData); //Verify if works correctly.
  }

  /**
   * @brief      Sets the shader information.
   * @param      inCBufferData The information of the cBuffer to set.
   */
  void
  setShader(MatrixCollection& inCBufferData) {
    auto& gapi = g_graphicsAPI();
    
    Vector<char> matrix_data;
    matrix_data.resize(sizeof(inCBufferData));

    gapi.setVertexShader(m_vertexShader);
    gapi.setPixelShader(m_pixelShader);
    gapi.setInputLayout(m_inputLayout);

    //Verify if it can be send with the shader manager
    //memcpy(matrix_data.data(), &inCBufferData, sizeof(inCBufferData));
    //gapi.writeToBuffer(m_pCB_WVP, matrix_data);

  }

  /**
   * @brief      Sets a sampler in the shader.
   * @param      inSamplerType The sampler to set.
   */
  void
  setSamplerUsage(SAMPLER_USAGE::E inSamplerType) {
    m_samplerUsage = inSamplerType;
  }

  /**
   * @brief      Set a raster in the shader.
   * @param      inRasterType  The raster to set.
   */
  void
  setRasterUsage(RASTER_USAGE::E inRasterType) {
    m_rasterUsage = inRasterType;
  }

  /**
   * @brief      Gets the sampler setted in the shader.
   * @return     Returns the sampler setted.
   */
  SAMPLER_USAGE::E 
  getSamplerUsage() const {
    return m_samplerUsage;
  }

  /**
   * @brief      Gets the raster setted in the shader.
   * @return     Returns the raster setted.
   */
  RASTER_USAGE::E
  getRasterUsage() const {
    return m_rasterUsage;
  }

 protected:
  
  /**
   * @brief      Creates the input layout automatically.
   */
  void
  reflectInputLayout() {
    ID3D11ShaderReflection* pVertexShaderReflection = nullptr;
    if (m_vertexShader) {
      auto vertexShaderBlob = m_vertexShader->getBlob();
      HRESULT hr = D3DReflect(vertexShaderBlob->GetBufferPointer(), 
                              vertexShaderBlob->GetBufferSize(),
                              IID_ID3D11ShaderReflection, 
                              (void**)&pVertexShaderReflection);
      if(FAILED(hr)) {
        ConsoleOut << "Failed to reflect vertex shader input layout." << ConsoleLine;
        return;
      }

      D3D11_SHADER_DESC shaderDesc;
      pVertexShaderReflection->GetDesc(&shaderDesc);

      Vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;
      for(uint32 i = 0; i < shaderDesc.InputParameters; ++i) {
        D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
        pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

        D3D11_INPUT_ELEMENT_DESC inputElementDesc;
        inputElementDesc.SemanticName = paramDesc.SemanticName;
        inputElementDesc.SemanticIndex = paramDesc.SemanticIndex;
        inputElementDesc.InputSlot = 0;
        inputElementDesc.AlignedByteOffset = 0xffffffff;
        inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputElementDesc.InstanceDataStepRate = 0;

        if(paramDesc.Mask == 1) {
          if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
            inputElementDesc.Format = DXGI_FORMAT_R32_UINT;
          }
          else if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  {
            inputElementDesc.Format = DXGI_FORMAT_R32_SINT;
          }
          else if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
            inputElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
          }
        }
        else if(paramDesc.Mask <= 3) {
          if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
            inputElementDesc.Format = DXGI_FORMAT_R32G32_UINT;
          }
          else if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
            inputElementDesc.Format = DXGI_FORMAT_R32G32_SINT;
          }
          else if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
            inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
          }
        }
        else if(paramDesc.Mask <= 7) {
          if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
            inputElementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
          }
          else if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
            inputElementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
          }
          else if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
            inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
          }
        }
        else if(paramDesc.Mask <= 15) {
          if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
            inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
          }
          else if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
            inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
          }
          else if(paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
            inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
          }
        }

        inputElementDescs.push_back(inputElementDesc);
      }
      //Save the input layout
      if(!inputElementDescs.empty()) {
        auto& GAPI = g_graphicsAPI();
        if(inputElementDescs.size() == 1) inputElementDescs.clear();
        m_inputLayout = GAPI.createInputLayout(inputElementDescs, m_vertexShader);
      }
      else {
        ConsoleOut << "No input layout found in vertex shader." << ConsoleLine;
      }

      pVertexShaderReflection->Release();
    }
  }

  Path m_shaderPath;
  Path m_pixelShaderPath;
  String m_vertexEntry = "";
  String m_pixelEntry = "";

  SAMPLER_USAGE::E m_samplerUsage = SAMPLER_USAGE::kPoint;

  RASTER_USAGE::E m_rasterUsage = RASTER_USAGE::kDefault;

  SPtr<VertexShader> m_vertexShader;
  SPtr<PixelShader> m_pixelShader;
  ID3D11InputLayout* m_inputLayout;

  SPtr<GraphicsBuffers> m_pCB_WVP;

};

////////////////////////////////////////////////////////////////////////////////////////////

class ShaderManager : public Module<ShaderManager> {
 public:
  ShaderManager() {
    auto& gapi = g_graphicsAPI();

    //Set the reaster and sampler
    CD3D11_RASTERIZER_DESC1 descRD(D3D11_DEFAULT);
    m_pRS_Default = gapi.createRasterState(descRD);

    descRD = CD3D11_RASTERIZER_DESC1(D3D11_DEFAULT);
    descRD.CullMode = D3D11_CULL_FRONT;
    m_pRS_CullFront = gapi.createRasterState(descRD);

    descRD = CD3D11_RASTERIZER_DESC1(D3D11_DEFAULT);
    descRD.FillMode = D3D11_FILL_WIREFRAME;
    m_pRS_Wireframe = gapi.createRasterState(descRD);

    descRD.CullMode = D3D11_CULL_NONE;
    m_pRS_Wireframe_NoCull = gapi.createRasterState(descRD);

    CD3D11_SAMPLER_DESC descSS(D3D11_DEFAULT);
    descSS.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    descSS.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    m_pSS_Point = gapi.createSamplerState(descSS);

    descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    m_pSS_Linear = gapi.createSamplerState(descSS);

    descSS.Filter = D3D11_FILTER_ANISOTROPIC;
    descSS.MaxAnisotropy = 16; //Esto es lo que cambiamos en las opciones de los juegos
    m_pSS_Anisotropic = gapi.createSamplerState(descSS);

    m_pCB_WVP = make_shared<GraphicsBuffers>();

  }

  ~ShaderManager() {
    SAFE_RELEASE(m_pRS_Default);
    SAFE_RELEASE(m_pRS_Wireframe);
    SAFE_RELEASE(m_pRS_Wireframe_NoCull);
    SAFE_RELEASE(m_pRS_CullFront);
    SAFE_RELEASE(m_pSS_Point);
    SAFE_RELEASE(m_pSS_Linear);
    SAFE_RELEASE(m_pSS_Anisotropic);
  }
  
  /**
   * @brief      Creates a shader and gets a reference.
   * @param      inShaderPath  The path to the shader file.
   * @param      inVertexEntry (optional, Default: vs_main) The entry point to the vertex shader.
   * @param      inPixelEntry  (optional, Default: ps_main) The entry point to the pixel shader.
   * @param      inPixelPath   (optional, Default: inShaderPath) The path to the pixel shader file.
   * @return     Returns a reference to the created shader.
   */
  ShaderRef
  createShaderProgram(Path inShaderPath,
                      String inVertexEntry = "",
                      String inPixelEntry = "",
                      Path inPixelPath = "") {
    ShaderRef outRef;
    outRef.shaderID = UID();

    SPtr<ShaderProgram> newShader = make_shared<ShaderProgram>();
    newShader->setShaderData(inShaderPath, inVertexEntry, inPixelEntry, inPixelPath);

    newShader->compileShaders();

    m_shaders.insert(Pair<UID, SPtr<ShaderProgram>>(outRef.shaderID, newShader));
    return outRef;
  }

  /**
   * @brief      Sets a sampler to an specific Shader.
   * @param      inShaderRef   The reference of the shader.
   * @param      inSamplerType The sampler to set.
   */
  void
  setSamplerToShader(ShaderRef inShaderRef, SAMPLER_USAGE::E inSamplerType) {
    auto it = m_shaders.find(inShaderRef.shaderID);
    if(it != m_shaders.end()) {
      it->second->setSamplerUsage(inSamplerType);
    }
  }

  /**
   * @brief      Sets a raster to an specific Shader.
   * @param      inShaderRef   The reference of the shader .
   * @param      inRasterType  The reaster to set.
   */
  void
  setRasterToShader(ShaderRef inShaderRef, RASTER_USAGE::E inRasterType) {
    auto it = m_shaders.find(inShaderRef.shaderID);
    if(it != m_shaders.end()) {
      it->second->setRasterUsage(inRasterType);
    }
  }

  /**
   * @brief      Sets the information of the given shader.
   * @param      inShaderRef   The reference of the shader to set.
   */
  void
  setDataToShader(ShaderRef inShaderRef) {

    auto& gapi = g_graphicsAPI();

    auto it = m_shaders.find(inShaderRef.shaderID);
    if(it != m_shaders.end()) {
      auto& tmpShader = it->second;

      //Set the shaders and constant buffer
      tmpShader->setShader(m_matrixCollection);

      //SetSamplers
      if (SAMPLER_USAGE::kAnisotropic == tmpShader->getSamplerUsage()) {
        gapi.setSamplers(0, m_pSS_Anisotropic);
      }
      else if(SAMPLER_USAGE::kLinear == tmpShader->getSamplerUsage()) {
        gapi.setSamplers(0, m_pSS_Linear);
      }
      else if(SAMPLER_USAGE::kPoint == tmpShader->getSamplerUsage()) {
        gapi.setSamplers(0, m_pSS_Point);
      }
      else if(SAMPLER_USAGE::kAll == tmpShader->getSamplerUsage()) {
        gapi.setSamplers(0, m_pSS_Point);
        gapi.setSamplers(1, m_pSS_Linear);
        gapi.setSamplers(2, m_pSS_Anisotropic);
      }
      
      //SetRaster
      if (RASTER_USAGE::kDefault == tmpShader->getRasterUsage()) {
        gapi.setRasterState(m_pRS_Default);
      }
      else if (RASTER_USAGE::kWireframe == tmpShader->getRasterUsage()) {
        gapi.setRasterState(m_pRS_Wireframe);
      }
      else if (RASTER_USAGE::kWireframe_NoCull == tmpShader->getRasterUsage()) {
        gapi.setRasterState(m_pRS_Wireframe_NoCull);
      }
      else if (RASTER_USAGE::kCullFront == tmpShader->getRasterUsage()) {
        gapi.setRasterState(m_pRS_CullFront);
      }
    }
  }

  /**
   * @brief      Sets the values of the matrix Collection cbuffer.
   * @param      inConstant    The values to set.
   */
  void
  setConstantValues(MatrixCollection& inConstant) {
    m_matrixCollection = inConstant;
  }

  /**
   * @brief      Compile all shadders saved.
   */
  void
  compileAllShaders() {
    for(auto& shaderPair : m_shaders) {
      shaderPair.second->compileShaders();
    }
  }

  /**
   * @brief      Sets an specific raster state.
   * @param      inRaster      The raster state to set.
   */
  void
  setRaster(RASTER_USAGE::E inRaster) {
    auto& gapi = g_graphicsAPI();
    if (RASTER_USAGE::kDefault == inRaster) {
      gapi.setRasterState(m_pRS_Default);
    }
    else if (RASTER_USAGE::kWireframe == inRaster) {
      gapi.setRasterState(m_pRS_Wireframe);
    }
    else if (RASTER_USAGE::kWireframe_NoCull == inRaster) {
      gapi.setRasterState(m_pRS_Wireframe_NoCull);
    }
    else if (RASTER_USAGE::kCullFront == inRaster) {
      gapi.setRasterState(m_pRS_CullFront);
    }
  }

  /**
   * @brief      Set a transform to the cbuffer and sets the cbuffer.
   * @param      inTransform   The transformation to set.
   */
  void
  setTransform(Transform& inTransform) {
    auto& gapi = g_graphicsAPI();
    
    Vector<char> matrix_data;
    matrix_data.resize(sizeof(m_matrixCollection));

    m_matrixCollection.world = m_worldTransform.getMatrix() * inTransform.getMatrix();
    m_matrixCollection.world.transpose();
    
    memcpy(matrix_data.data(), &m_matrixCollection, sizeof(m_matrixCollection));
    if(!m_pCB_WVP->m_pBuffer) {
      m_pCB_WVP = gapi.createConstantBuffer(matrix_data);
    }
    else {
      gapi.writeToBuffer(m_pCB_WVP, matrix_data);
    }
    gapi.setConstantBuffer(0, m_pCB_WVP);
  }

 protected:
  
  MatrixCollection m_matrixCollection;

  Transform m_worldTransform;

  SPtr<GraphicsBuffers> m_pCB_WVP;

  ID3D11RasterizerState1* m_pRS_Default = nullptr;
  ID3D11RasterizerState1* m_pRS_Wireframe = nullptr;
  ID3D11RasterizerState1* m_pRS_Wireframe_NoCull = nullptr;
  ID3D11RasterizerState1* m_pRS_CullFront = nullptr;

  ID3D11SamplerState* m_pSS_Point = nullptr;
  ID3D11SamplerState* m_pSS_Linear = nullptr;
  ID3D11SamplerState* m_pSS_Anisotropic = nullptr;

  Map<UID, SPtr<ShaderProgram>> m_shaders;
};

ShaderManager& g_shaderManager();

////////////////////////////////////////////////////////////////////////////////////////////

