/**
 * @file    GraphicsAPI.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    22/02/25
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "Shader.h"
#include "Buffers.h"
#include "Texture.h"
#include <d3d11_2.h>

class GraphicsAPI {
 public:
  GraphicsAPI(void* inWindow);

  ~GraphicsAPI();

  ID3D11Texture2D* 
  createTexture(int32 inWidth, 
                int32 inHeight, 
                DXGI_FORMAT inFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
                D3D11_USAGE inUsage = D3D11_USAGE_DEFAULT,
                uint32 inBindFlags = D3D11_BIND_SHADER_RESOURCE,
                uint32 inCpuAccessFlags = 0,
                uint32 inMipLevels = 1, 
                ID3D11ShaderResourceView** inSRV = nullptr,
                ID3D11RenderTargetView** inRTV = nullptr,
                ID3D11DepthStencilView** inDSV = nullptr);

  UPtr<VertexShader>
  createVertexShaderFromFile(const Path& inFilePath, 
                             const String& inEntryFunction);
  UPtr<PixelShader>
  createPixelShaderFromFile(const Path& inFilePath, 
                            const String& inEntryFunction);

  ID3D11InputLayout* 
  createInputLayout(Vector<D3D11_INPUT_ELEMENT_DESC> inInputElementDescs,
                    const UPtr<VertexShader>& inVertexShader);

  UPtr<GraphicsBuffers> 
  createVertexBuffer(const Vector<char>& inData);

  UPtr<GraphicsBuffers> 
  createIndexBuffer(const Vector<char>& inData);

  UPtr<GraphicsBuffers> 
  createConstantBuffer(const Vector<char>& inData);
  
  void
  writeToBuffer(const UPtr<GraphicsBuffers>& inBuffer, 
                const Vector<char>& inData);

  void
  setVertexShader(const UPtr<VertexShader>& inShader);

  void
  setPixelShader(const UPtr<PixelShader>& inShader);

  void
  setRenderTargets(const Texture& inRTV, const Texture& inDSV);

  void
  setShaderResource(uint32 inStartSlot, const Texture& inSRV);

  void
  setRasterState(ID3D11RasterizerState1* inState);



  void
  setTopology(int32 inTopologyType);

  void
  clearRTV(const Texture& inRTV, FloatColor inClearColor);

  void
  clearDSV(const Texture& inDSV);

  void
  clearSRV(int32 inSlot);

 private:

  void
  QueryInterces(uint32 inWidth, uint32 inHeight);

 public:
  void* m_pHandle;

  ID3D11Device1* m_pDevice = nullptr;
  ID3D11DeviceContext1* m_pDeviceContext = nullptr;
  IDXGISwapChain1* m_pSwapChain = nullptr;

  Texture m_pBackBufferRTV;
  Texture m_pBackBufferDSV;

  ID3D11InputLayout* m_pInputLayout = nullptr;

};