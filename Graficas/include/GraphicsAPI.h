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
#include "Module.h"

class GraphicsAPI : public Module<GraphicsAPI> {
 public:
  GraphicsAPI() = default;

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
                ID3D11DepthStencilView** inDSV = nullptr,
                ID3D11DepthStencilView** outDSV_RO = nullptr);

  SPtr<VertexShader>
  createVertexShaderFromFile(const Path& inFilePath, 
                             const String& inEntryFunction);
  SPtr<PixelShader>
  createPixelShaderFromFile(const Path& inFilePath, 
                            const String& inEntryFunction);

  ID3D11InputLayout* 
  createInputLayout(Vector<D3D11_INPUT_ELEMENT_DESC> inInputElementDescs,
                    const SPtr<VertexShader>& inVertexShader);

  SPtr<GraphicsBuffers> 
  createVertexBuffer(const Vector<char>& inData);

  SPtr<GraphicsBuffers> 
  createIndexBuffer(const Vector<char>& inData);

  SPtr<GraphicsBuffers> 
  createConstantBuffer(const Vector<char>& inData);

  ID3D11RasterizerState1*
  createRasterState(CD3D11_RASTERIZER_DESC1 inDesc);

  ID3D11SamplerState*
  createSamplerState(CD3D11_SAMPLER_DESC inDesc);
  
  void
  writeToBuffer(const SPtr<GraphicsBuffers>& inBuffer, 
                const Vector<char>& inData);
  
  void
  setInputLayout(ID3D11InputLayout* inInputLayout);

  void
  setVertexShader(const SPtr<VertexShader>& inShader);

  void
  setPixelShader(const SPtr<PixelShader>& inShader);

  void
  setRenderTargets(SPtr<Texture> inRTV, SPtr<Texture> inDSV);
  
  void
  setRenderTargets(int32 inSize, Vector<SPtr<Texture>>& inRT, SPtr<Texture> inDSV);

  void
  setShaderResource(uint32 inStartSlot, SPtr<Texture> inSRV);

  void
  setRasterState(ID3D11RasterizerState1* inState);

  void
  setSamplers(int32 inSlot, ID3D11SamplerState* inSampler);

  void
  setConstantBuffer(int32 inSlot, const SPtr<GraphicsBuffers>& inBuffer);

  void
  setTopology(int32 inTopologyType);

  void
  setViewport(const D3D11_VIEWPORT& inViewport);

  void
  clearRTV(SPtr<Texture> inRTV, FloatColor inClearColor);

  void
  clearDSV(SPtr<Texture> inDSV);

  void
  clearSRV(int32 inSlot);

  void
  resizeBackBuffer(Vector2 inSize);

  void
  present(bool inVsync);

  ID3D11Device1*
  getDevice() { return m_pDevice; }

  ID3D11DeviceContext1*
  getDeviceContext() { return m_pDeviceContext; }

 private:

  void
  QueryInterces(uint32 inWidth, uint32 inHeight);

 public:
  void* m_pHandle;

  ID3D11Device1* m_pDevice = nullptr;
  ID3D11DeviceContext1* m_pDeviceContext = nullptr;
  IDXGISwapChain1* m_pSwapChain = nullptr;

  SPtr<Texture> m_pBackBufferRTV;
  SPtr<Texture> m_pBackBufferDSV;

  ID3D11InputLayout* m_pInputLayout = nullptr;

};

GraphicsAPI&
g_graphicsAPI();