/**
 * @file    GraphicsAPI.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    22/02/25
 */

/**
 * @include
 */
#include "GraphicsAPI.h"


#include <fstream>

#include <d3dcompiler.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

GraphicsAPI::GraphicsAPI(void* inWindow) : m_pHandle(inWindow) {
  HWND hwnd = static_cast<HWND>(inWindow);
  
  //Get Window size
  RECT rc;
  GetClientRect(hwnd, &rc);

  Vector<IDXGIAdapter*> vAdapters;
  IDXGIAdapter1* pAdapter = nullptr;
  IDXGIFactory1* pFactory = nullptr;
  CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory);

  for(UINT i = 0; pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
    DXGI_ADAPTER_DESC1 apaterDesc;
    pAdapter->GetDesc1(&apaterDesc);
    vAdapters.push_back(pAdapter);
  }
  
  SAFE_RELEASE(pAdapter);
  SAFE_RELEASE(pFactory);

  Vector<D3D_FEATURE_LEVEL> featureLvl = { D3D_FEATURE_LEVEL_11_1,
                                           D3D_FEATURE_LEVEL_11_0,
                                           D3D_FEATURE_LEVEL_10_1,
                                           D3D_FEATURE_LEVEL_10_0,
                                           D3D_FEATURE_LEVEL_9_3 };

  D3D_FEATURE_LEVEL selectedFeatureLvl;

  UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined (_DEBUG)
  deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  ID3D11Device* pDevice;
  ID3D11DeviceContext* pDeviceContext;

  //Create the device
  HRESULT hr =  D3D11CreateDevice(vAdapters[0],
                                  D3D_DRIVER_TYPE_UNKNOWN,
                                  nullptr,
                                  deviceFlags,
                                  featureLvl.data(),
                                  featureLvl.size(),
                                  D3D11_SDK_VERSION,
                                  &pDevice,
                                  &selectedFeatureLvl,
                                  &pDeviceContext);

  if (FAILED(hr)) {
    MessageBox(hwnd, L"Failed to create device", L"Error", MB_OK);
    return;
  }

  pDevice->QueryInterface(__uuidof(ID3D11Device1), (void**)&m_pDevice);
  pDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&m_pDeviceContext);

  SAFE_RELEASE(pDeviceContext);
  SAFE_RELEASE(pDevice);

  DXGI_SWAP_CHAIN_DESC1 scDesc;
  memset(&scDesc, 0, sizeof(scDesc));
  scDesc.BufferCount = 2;
  scDesc.Width = rc.right;
  scDesc.Height = rc.bottom;
  scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  scDesc.Stereo = false;
  scDesc.SampleDesc.Count = 1; //MSAA - Multi Sampling Anti Aliassing
  scDesc.SampleDesc.Quality = 0;
  scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  scDesc.Scaling = DXGI_SCALING_NONE;
  scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  

  IDXGIDevice1* pDXGIDevice = nullptr;
  m_pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDXGIDevice);

  IDXGIAdapter* pDXGIAdapter = nullptr;
  pDXGIDevice->GetAdapter(&pDXGIAdapter);

  IDXGIFactory2* pFactory2 = nullptr;
  pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&pFactory2);

  hr = pFactory2->CreateSwapChainForHwnd(m_pDevice, 
                                         hwnd, 
                                         &scDesc, 
                                         nullptr, 
                                         nullptr, 
                                         &m_pSwapChain);
  
  pDXGIDevice->SetMaximumFrameLatency(3);

  if (FAILED(hr)) {
    MessageBox(hwnd, L"Failed to create swapChain", L"Error", MB_OK);
    return;
  }

  QueryInterces(scDesc.Width, scDesc.Height);

  //Setup the viewport
  D3D11_VIEWPORT vp;
  vp.Width = static_cast<float>(scDesc.Width);
  vp.Height = static_cast<float>(scDesc.Height);
  vp.MinDepth = 0.f;
  vp.MaxDepth = 1.f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  m_pDeviceContext->RSSetViewports(1, &vp);

  SAFE_RELEASE(pDXGIDevice);
  SAFE_RELEASE(pDXGIAdapter);
  SAFE_RELEASE(pFactory2)

}

GraphicsAPI::~GraphicsAPI() {
  SAFE_RELEASE(m_pSwapChain);
  SAFE_RELEASE(m_pDeviceContext);
  SAFE_RELEASE(m_pDevice);
}

ID3D11Texture2D* 
GraphicsAPI::createTexture(int32 inWidth, 
                           int32 inHeight, 
                           DXGI_FORMAT inFormat, 
                           D3D11_USAGE inUsage, 
                           uint32 inBindFlags, 
                           uint32 inCpuAccessFlags, 
                           uint32 inMipLevels, 
                           ID3D11ShaderResourceView** inSRV,
                           ID3D11RenderTargetView** inRTV,
                           ID3D11DepthStencilView** inDSV) {
  ID3D11Texture2D* pOutTexture = nullptr;

  D3D11_TEXTURE2D_DESC desc;
  memset(&desc, 0, sizeof(desc));
  desc.ArraySize = 1;
  desc.BindFlags = inBindFlags;
  desc.CPUAccessFlags = inCpuAccessFlags;
  desc.Format = inFormat;
  desc.Height = inHeight;
  desc.Width = inWidth;
  desc.MiscFlags = 0;

  desc.SampleDesc.Count = 1; //MSAA
  desc.SampleDesc.Quality = 0; //MSAA

  desc.Usage = inUsage;

  if(FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &pOutTexture))) {
    return nullptr;
  }
  //
  if(inSRV != nullptr) {
    if (inBindFlags & D3D11_BIND_SHADER_RESOURCE) {
      D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC();
      srvDesc.Format = inFormat;
      srvDesc.Texture2D.MipLevels =  inMipLevels == 1 ? 1 : -1;
      srvDesc.Texture2D.MostDetailedMip = 0;
      srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

      m_pDevice->CreateShaderResourceView(pOutTexture, &srvDesc, inSRV);
    }

  }
  //
  if(inRTV != nullptr) {
    if (inBindFlags & D3D11_BIND_RENDER_TARGET) {
      D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = CD3D11_RENDER_TARGET_VIEW_DESC();
      rtvDesc.Format = inFormat;
      rtvDesc.Texture2D.MipSlice = 0;
      rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
      m_pDevice->CreateRenderTargetView(pOutTexture, &rtvDesc, inRTV);
    }
  }
  if (inDSV != nullptr) {
    if(inBindFlags & D3D11_BIND_DEPTH_STENCIL) {
      D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC();
      dsvDesc.Format = inFormat;
      dsvDesc.Texture2D.MipSlice = 0;
      dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      m_pDevice->CreateDepthStencilView(pOutTexture, &dsvDesc, inDSV);
    }
  }

  return pOutTexture;
}

void 
GraphicsAPI::QueryInterces(uint32 inWidth, uint32 inHeight) {
  if(!m_pSwapChain) {
    return;
  }

  ID3D11Texture2D* pBackBuffer = nullptr;

  m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& pBackBuffer);

  if(FAILED(m_pDevice->CreateRenderTargetView(pBackBuffer, 
                                              nullptr, 
                                              &m_pBackBufferRTV))) {
    __debugbreak();
  }

  SAFE_RELEASE(pBackBuffer);

  //Create Depth Stencil
  D3D11_DEPTH_STENCIL_DESC dsDesc;

  ID3D11Texture2D* pDepthStencil = nullptr;
  pDepthStencil = createTexture(inWidth, 
                                inHeight, 
                                DXGI_FORMAT_D24_UNORM_S8_UINT,
                                D3D11_USAGE_DEFAULT,
                                D3D11_BIND_DEPTH_STENCIL);
  if(!pDepthStencil) {
    MessageBox(nullptr, L"Failed to create depth stencil buffer", L"Error", MB_OK);
    return;
  }

  m_pDevice->CreateDepthStencilView(pDepthStencil, nullptr, &m_pBackBufferDSV);

  SAFE_RELEASE(pDepthStencil);
}

UPtr<VertexShader>
GraphicsAPI::createVertexShaderFromFile(const Path& inFilePath, 
                                        const String& inEntryFunction) {
  UPtr<VertexShader> tmpShader = make_unique<VertexShader>();
  if(!tmpShader->compile(inFilePath, inEntryFunction, "vs_5_0")) {
    return nullptr;
  }

  HRESULT hr = m_pDevice->CreateVertexShader(tmpShader->getBlob()->GetBufferPointer(),
                                             tmpShader->getBlob()->GetBufferSize(),
                                             nullptr,
                                             &tmpShader->m_pVertexShader);
  
  if(FAILED(hr)) {
    MessageBox(nullptr, L"Failed to create Vertex Shader", L"Error", MB_OK);
    return nullptr;
  }

  return tmpShader;
}

UPtr<PixelShader>
GraphicsAPI::createPixelShaderFromFile(const Path& inFilePath, 
                                       const String& inEntryFunction) {
  UPtr<PixelShader> tmpShader = make_unique<PixelShader>();
  if (!tmpShader->compile(inFilePath, inEntryFunction, "ps_5_0")) {
    return nullptr;
  }

  HRESULT hr = m_pDevice->CreatePixelShader(tmpShader->getBlob()->GetBufferPointer(),
                                             tmpShader->getBlob()->GetBufferSize(),
                                             nullptr,
                                             &tmpShader->m_pPixelShader);

  if (FAILED(hr)) {
    MessageBox(nullptr, L"Failed to create Vertex Shader", L"Error", MB_OK);
    return nullptr;
  }

  return tmpShader;
}

ID3D11InputLayout* 
GraphicsAPI::createInputLayout(Vector<D3D11_INPUT_ELEMENT_DESC> inInputElementDescs, 
                               const UPtr<VertexShader>& inVertexShader) {
  ID3D11InputLayout* tmpInputLayout = nullptr;

  if (inInputElementDescs.empty()) {
    return nullptr;
  }

  HRESULT hr = m_pDevice->CreateInputLayout(inInputElementDescs.data(),
                                            inInputElementDescs.size(),
                                            inVertexShader->getBlob()->GetBufferPointer(),
                                            inVertexShader->getBlob()->GetBufferSize(),
                                            &tmpInputLayout);
  if (FAILED(hr)) {
    MessageBox(nullptr, L"Failed to create input layout", L"Error", MB_OK);
    return nullptr;
  }

  return tmpInputLayout;
}

UPtr<GraphicsBuffers> 
GraphicsAPI::createVertexBuffer(const Vector<char>& inData) {
  UPtr<GraphicsBuffers> outData = make_unique<GraphicsBuffers>();
  
  D3D11_BUFFER_DESC tmpDesc;
  memset(&tmpDesc, 0, sizeof(tmpDesc));
  tmpDesc.Usage  = D3D11_USAGE_DEFAULT;
  tmpDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  tmpDesc.CPUAccessFlags = 0;
  tmpDesc.MiscFlags = 0;

  tmpDesc.ByteWidth = inData.size();
  
  D3D11_SUBRESOURCE_DATA initData;
  initData.pSysMem = inData.data();
  initData.SysMemPitch = 0;
  initData.SysMemSlicePitch = 0;

  HRESULT hr = m_pDevice->CreateBuffer(&tmpDesc, &initData, &outData->m_pBuffer);
  if (FAILED(hr)) {
    MessageBox(nullptr, L"Failed to create Vertex buffer", L"Error", MB_OK);
    return nullptr;
  }

  return outData;
}

UPtr<GraphicsBuffers> 
GraphicsAPI::createIndexBuffer(const Vector<char>& inData) {
  UPtr<GraphicsBuffers> outData = make_unique<GraphicsBuffers>();

  D3D11_BUFFER_DESC tmpDesc;
  memset(&tmpDesc, 0, sizeof(tmpDesc));
  tmpDesc.Usage = D3D11_USAGE_DEFAULT;
  tmpDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  tmpDesc.CPUAccessFlags = 0;
  tmpDesc.MiscFlags = 0;

  tmpDesc.ByteWidth = inData.size();

  D3D11_SUBRESOURCE_DATA initData;
  initData.pSysMem = inData.data();
  initData.SysMemPitch = 0;
  initData.SysMemSlicePitch = 0;

  HRESULT hr = m_pDevice->CreateBuffer(&tmpDesc, &initData, &outData->m_pBuffer);
  if (FAILED(hr)) {
    MessageBox(nullptr, L"Failed to create Index buffer", L"Error", MB_OK);
    return nullptr;
  }

  return outData;
}

UPtr<GraphicsBuffers> 
GraphicsAPI::createConstantBuffer(const Vector<char>& inData) {
  UPtr<GraphicsBuffers> outData = make_unique<GraphicsBuffers>();

  D3D11_BUFFER_DESC tmpDesc;
  memset(&tmpDesc, 0, sizeof(tmpDesc));
  tmpDesc.Usage = D3D11_USAGE_DEFAULT;
  tmpDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  tmpDesc.CPUAccessFlags = 0;
  tmpDesc.MiscFlags = 0;

  tmpDesc.ByteWidth = inData.size();

  D3D11_SUBRESOURCE_DATA initData;
  initData.pSysMem = inData.data();
  initData.SysMemPitch = 0;
  initData.SysMemSlicePitch = 0;

  HRESULT hr = m_pDevice->CreateBuffer(&tmpDesc, &initData, &outData->m_pBuffer);
  if (FAILED(hr)) {
    MessageBox(nullptr, L"Failed to create Constant buffer", L"Error", MB_OK);
    return nullptr;
  }

  return outData;
}

void 
GraphicsAPI::writeToBuffer(const UPtr<GraphicsBuffers>& inBuffer, 
                           const Vector<char>& inData) {
  /*D3D11_MAPPED_SUBRESOURCE mappedResource;
  m_pDeviceContext->Map(inBuffer->m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  memcpy(mappedResource.pData, inData.data(), inData.size());
  m_pDeviceContext->Unmap(inBuffer->m_pBuffer, 0);*/

  m_pDeviceContext->UpdateSubresource1(inBuffer->m_pBuffer, 0, nullptr, inData.data(), 0, 0, 0);
}

void 
GraphicsAPI::setVertexShader(const UPtr<VertexShader>& inShader) {
  m_pDeviceContext->VSSetShader(inShader->m_pVertexShader, nullptr, 0);
}

void 
GraphicsAPI::setPixelShader(const UPtr<PixelShader>& inShader) {
  m_pDeviceContext->PSSetShader(inShader->m_pPixelShader, nullptr, 0);
}

void 
GraphicsAPI::setTopology(int32 inTopologyType) {
  m_pDeviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(inTopologyType));
}
