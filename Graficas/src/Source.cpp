/**
 * @file    Source.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    22/02/25
 */

/**
 * @include
 */
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <iostream>

#include "PrerequisiteGraficas.h"
#include "GraphicsAPI.h"
#include "Model.h"
#include "Texture.h"

struct MatrixCollection {
  Matrix4 world;
  Matrix4 view;
  Matrix4 projection;
};

//Vector2 g_windowSize = {640 , 480};
Vector2 g_windowSize = {1280 , 720};

SDL_Window* g_pWindow = nullptr;
UPtr<GraphicsAPI> g_pGAPI;
UPtr<VertexShader> g_pVertexShader;
UPtr<PixelShader> g_pPixelShader;
UPtr<PixelShader> g_pPixelShader_Reflect;
ID3D11InputLayout* g_pInputLayout = nullptr;
UPtr<GraphicsBuffers> g_pVertexBuffer;
UPtr<GraphicsBuffers> g_pIndexBuffer;
UPtr<GraphicsBuffers> g_pCB_WVP;

ID3D11RasterizerState1* g_pRS_Default = nullptr;
ID3D11RasterizerState1* g_pRS_Wireframe = nullptr;
ID3D11RasterizerState1* g_pRS_Wireframe_NoCull = nullptr;
ID3D11RasterizerState1* g_pRS_CullFront = nullptr;

ID3D11SamplerState* g_pSS_Point = nullptr;
ID3D11SamplerState* g_pSS_Linear = nullptr;
ID3D11SamplerState* g_pSS_Anisotropic = nullptr;

MatrixCollection g_WVP;

Camera g_Camera;

Model g_myModel;
Model g_TerrainModel;
Texture g_myTexture;
Texture g_TerrainTexture;

Texture g_rtReflection;
Texture g_dsReflection;

 /* This function runs once at startup. */
SDL_AppResult
SDL_AppInit(void** appstate, int argc, char* argv[]) {

  //Initialize sdl
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  //Create sdl window
  g_pWindow = SDL_CreateWindow("Graficas Basicas", 
                               g_windowSize.x, 
                               g_windowSize.y, 
                               SDL_WINDOW_RESIZABLE);

  auto pHandle = SDL_GetPointerProperty(SDL_GetWindowProperties(g_pWindow), 
                                        SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                        nullptr);
  if(pHandle) {
    g_pGAPI = make_unique<GraphicsAPI>(pHandle);

    if (!g_pGAPI) {
      return SDL_APP_FAILURE;
    }

    g_pVertexShader = g_pGAPI->createVertexShaderFromFile("Shaders/basicVertexShader.hlsl", 
                                                          "vertex_main");
    if(!g_pVertexShader) {
      return SDL_APP_FAILURE;
    }

    g_pPixelShader = g_pGAPI->createPixelShaderFromFile("Shaders/basicVertexShader.hlsl", 
                                                        "pixel_main");
    if (!g_pPixelShader) {
      return SDL_APP_FAILURE;
    }
    g_pPixelShader_Reflect = g_pGAPI->createPixelShaderFromFile("Shaders/basicVertexShader.hlsl", 
                                                                "pixel_reflect_main");
    if (!g_pPixelShader_Reflect) {
      return SDL_APP_FAILURE;
    }
  }

  Vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };

  g_pInputLayout = g_pGAPI->createInputLayout(inputElementDescs, g_pVertexShader);
  if (!g_pInputLayout) {
    return SDL_APP_FAILURE;
  }

  SimpleVertex vertex[] = {
    { Vector3(-1.0f,  1.0f, -1.0f), Vector3(0, 1, 0)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3( 1.0f,  1.0f, -1.0f), Vector3(0, 1, 0)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3( 1.0f,  1.0f,  1.0f), Vector3(0, 1, 0)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3(-1.0f,  1.0f,  1.0f), Vector3(0, 1, 0)  }, //Vector2(0.0f, 1.0f), -0.f },

    { Vector3(-1.0f, -1.0f, -1.0f), Vector3(0, 1, 0)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3( 1.0f, -1.0f, -1.0f), Vector3(0, 1, 0)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3( 1.0f, -1.0f,  1.0f), Vector3(0, 1, 0)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3(-1.0f, -1.0f,  1.0f), Vector3(0, 1, 0)  }, //Vector2(0.0f, 1.0f), -0.f },
    
    { Vector3(-1.0f, -1.0f,  1.0f), Vector3(1, 0, 0)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3(-1.0f, -1.0f, -1.0f), Vector3(1, 0, 0)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3(-1.0f,  1.0f, -1.0f), Vector3(1, 0, 0)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3(-1.0f,  1.0f,  1.0f), Vector3(1, 0, 0)  }, //Vector2(0.0f, 1.0f), -0.f },
    
    { Vector3( 1.0f, -1.0f,  1.0f), Vector3(1, 0, 0)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3( 1.0f, -1.0f, -1.0f), Vector3(1, 0, 0)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3( 1.0f,  1.0f, -1.0f), Vector3(1, 0, 0)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3( 1.0f,  1.0f,  1.0f), Vector3(1, 0, 0)  }, //Vector2(0.0f, 1.0f), -0.f },
    
    { Vector3(-1.0f, -1.0f, -1.0f), Vector3(0, 0, 1)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3( 1.0f, -1.0f, -1.0f), Vector3(0, 0, 1)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3( 1.0f,  1.0f, -1.0f), Vector3(0, 0, 1)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3(-1.0f,  1.0f, -1.0f), Vector3(0, 0, 1)  }, //Vector2(0.0f, 1.0f), -0.f },
    
    { Vector3(-1.0f, -1.0f,  1.0f), Vector3(0, 0, 1)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3( 1.0f, -1.0f,  1.0f), Vector3(0, 0, 1)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3( 1.0f,  1.0f,  1.0f), Vector3(0, 0, 1)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3(-1.0f,  1.0f,  1.0f), Vector3(0, 0, 1)  }  //Vector2(0.0f, 1.0f), -0.f },

  };


  unsigned short indices[] = {
      3,1,0,
      2,1,3,

      6,4,5,
      7,4,6,

      11,9,8,
      10,9,11,

      14,12,13,
      15,12,14,

      19,17,16,
      18,17,19,

      22,20,21,
      23,20,22
  };

  Vector<char> tmpVertexData;
  tmpVertexData.resize(sizeof(vertex));
  memcpy(tmpVertexData.data(), vertex, sizeof(vertex));
  g_pVertexBuffer = g_pGAPI->createVertexBuffer(tmpVertexData);

  if(!g_pVertexBuffer) {
    return SDL_APP_FAILURE;
  }

  Vector<char> tmpIndexData;
  tmpIndexData.resize(sizeof(indices));
  memcpy(tmpIndexData.data(), indices, sizeof(indices));
  g_pIndexBuffer = g_pGAPI->createIndexBuffer(tmpIndexData);

  if (!g_pIndexBuffer) {
    return SDL_APP_FAILURE;
  }


  if(!g_pWindow) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  g_Camera.setLookAt(Vector3(5, -5, -5), Vector3(0,0,0), Vector3(0,1,0));
  g_Camera.setPerspective(3.1415926353f/4.f, g_windowSize, 0.1f, 100.f);
  
  
  g_WVP.world.identity();
  g_WVP.view = g_Camera.getViewMatrix();
  g_WVP.projection = g_Camera.getProjectionMatrix();

  g_WVP.world.transpose();
  g_WVP.view.transpose();
  g_WVP.projection.transpose();

  CD3D11_RASTERIZER_DESC1 descRD(D3D11_DEFAULT);
  g_pGAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Default);

  descRD.FillMode = D3D11_FILL_WIREFRAME;
  descRD.CullMode = D3D11_CULL_NONE;
  g_pGAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Wireframe_NoCull);

  CD3D11_SAMPLER_DESC descSS(D3D11_DEFAULT);
  descSS.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  descSS.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  g_pGAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Point);


  descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  g_pGAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Linear);


  descSS.Filter = D3D11_FILTER_ANISOTROPIC;
  descSS.MaxAnisotropy = 16; //Esto es lo que cambiamos en las opciones de los juegos
  g_pGAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Anisotropic);

  Vector<char> matrix_data;
  matrix_data.resize(sizeof(g_WVP));
  memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
  g_pCB_WVP = g_pGAPI->createConstantBuffer(matrix_data);

  /*CD3D11_RASTERIZER_DESC1 descRD(D3D11_DEFAULT);
  g_pGAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Default);

  descRD.FillMode = D3D11_FILL_WIREFRAME;
  g_pGAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Wireframe);
  
  descRD.FillMode = D3D11_FILL_WIREFRAME;
  descRD.CullMode = D3D11_CULL_NONE;
  g_pGAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Wireframe_NoCull);

  descRD = CD3D11_RASTERIZER_DESC1(D3D11_DEFAULT);
  descRD.CullMode = D3D11_CULL_FRONT;
  g_pGAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_CullFront);

  CD3D11_SAMPLER_DESC descSS(D3D11_DEFAULT);
  descSS.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  descSS.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;

  descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  g_pGAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Point);

  descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  g_pGAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Linear);

  descSS.Filter = D3D11_FILTER_ANISOTROPIC;
  descSS.MaxAnisotropy = 16;
  g_pGAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Anisotropic);*/


  g_myModel.loadFromFile("Models/rex.obj", g_pGAPI);
  g_TerrainModel.loadFromFile("Models/disc.obj", g_pGAPI);

  Image srcImage;
  srcImage.decode("Models/Rex_C.bmp");
  g_myTexture.createFromImage(srcImage, g_pGAPI);

  Image terrainImage;
  terrainImage.decode("Models/Terrain.bmp");
  g_TerrainTexture.createFromImage(terrainImage, g_pGAPI);

  g_rtReflection.m_pTexture = g_pGAPI->createTexture(g_windowSize.x, 
                                                     g_windowSize.y,
                                                     DXGI_FORMAT_B8G8R8A8_UNORM,
                                                     D3D11_USAGE_DEFAULT,
                                                     D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 
                                                     0,
                                                     1,
                                                     &g_rtReflection.m_pSRV,
                                                     &g_rtReflection.m_pRTV);

  g_dsReflection.m_pTexture = g_pGAPI->createTexture(g_windowSize.x, 
                                                     g_windowSize.y,
                                                     DXGI_FORMAT_D24_UNORM_S8_UINT,
                                                     D3D11_USAGE_DEFAULT,
                                                     D3D11_BIND_DEPTH_STENCIL, 
                                                     0,
                                                     1,
                                                     nullptr,
                                                     nullptr,
                                                     &g_dsReflection.m_pDSV);
  return SDL_APP_CONTINUE;
}


/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult
SDL_AppEvent(void* appstate, SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
  }
  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult
SDL_AppIterate(void* appstate) {

  /* put the newly-cleared rendering on the screen. */
  //SDL_RenderPresent(g_renderer);

  //Rellenar el input assembly
  //IA > Input Assambly
  //OM > Output Merger
  D3D11_VIEWPORT vp;
  vp.Width = g_windowSize.x;
  vp.Height = g_windowSize.y;
  vp.MinDepth = 0.f;
  vp.MaxDepth = 1.f;
  vp.TopLeftX = 0; 
  vp.TopLeftY = 0;


  g_pGAPI->m_pDeviceContext->OMSetRenderTargets(1, 
                                                &g_pGAPI->m_pBackBufferRTV, 
                                                g_pGAPI->m_pBackBufferDSV);
  
  float clearColor[] = { 0.5f, 0.5f, 1.0f, 1.0f };
  float blackClearColor[] = { 0.0f, 0.f, 0.0f, 1.0f };
  g_rtReflection.clearTexture(blackClearColor, g_pGAPI);
  g_dsReflection.clearTexture(blackClearColor, g_pGAPI);

  g_pGAPI->m_pDeviceContext->ClearRenderTargetView(g_pGAPI->m_pBackBufferRTV, 
                                                   clearColor);

  g_pGAPI->m_pDeviceContext->ClearDepthStencilView(g_pGAPI->m_pBackBufferDSV, 
                                                   D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
                                                   1.f, 
                                                   0);

  g_pGAPI->setVertexShader(g_pVertexShader);
  g_pGAPI->setPixelShader(g_pPixelShader);

  g_pGAPI->m_pDeviceContext->IASetInputLayout(g_pInputLayout);
  g_pGAPI->setTopology(g_myModel.m_meshes[0].topology);

  UINT stride = sizeof(SimpleVertex);
  UINT offset = 0;
  g_pGAPI->m_pDeviceContext->IASetVertexBuffers(0, 
                                                1,
                                                &g_pVertexBuffer->m_pBuffer,
                                                &stride,
                                                &offset);


  g_pGAPI->m_pDeviceContext->IASetIndexBuffer(g_pIndexBuffer->m_pBuffer,
                                              DXGI_FORMAT_R16_UINT,
                                              0);
  
  static float rotationAngle = 0.f;
  //rotationAngle += 0.001f;
  Matrix4 rotation; //000
  rotation.rotateY(rotationAngle);

  Matrix4 translation1; //000
  translation1.identity();
  translation1.Translate(Vector3(0, 0, 0));
  Matrix4 translation2; //1.5, 0, -2.5
  translation2.identity();
  translation2.Translate({1.5, 0, -2.5});
  Matrix4 translation3; //000
  translation3.identity();
  translation3.Translate({0, 0, 0});


  g_WVP.world = rotation * translation1;
  g_WVP.world.transpose();

  Vector<char> matrix_data;
  matrix_data.resize(sizeof(g_WVP));

  g_pGAPI->m_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pCB_WVP->m_pBuffer);

  g_pGAPI->m_pDeviceContext->RSSetState(g_pRS_Default);

  //Set the samplers
  g_pGAPI->m_pDeviceContext->PSSetSamplers(0, 1, &g_pSS_Point);
  g_pGAPI->m_pDeviceContext->PSSetSamplers(0, 1, &g_pSS_Linear);
  g_pGAPI->m_pDeviceContext->PSSetSamplers(0, 1, &g_pSS_Anisotropic);



  ////////////////////////////////////////////////////////////////////////////////////////////
  Matrix4 dinoTransform;
  dinoTransform.identity();
  //rotation.rotateY(rotationAngle);
  g_WVP.world = dinoTransform * rotation * translation1;
  memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
  g_pGAPI->writeToBuffer(g_pCB_WVP, matrix_data);

  g_pGAPI->m_pDeviceContext->OMSetRenderTargets(1, 
                                                &g_pGAPI->m_pBackBufferRTV, 
                                                g_pGAPI->m_pBackBufferDSV);


  g_myModel.setBuffers(g_pGAPI);

  g_pGAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &g_myTexture.m_pSRV);


  g_myModel.draw(g_pGAPI);

  ////////////////////////////////////////////////////////////////////////////////////////////
  Matrix4 refScale; //Reflection
  refScale.identity();
  refScale.scale({1, -1, 1});
  rotation.rotateY(rotationAngle);
  g_WVP.world = refScale * rotation * translation3;
  g_WVP.world.transpose();

  memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
  g_pGAPI->writeToBuffer(g_pCB_WVP, matrix_data);

  g_pGAPI->m_pDeviceContext->RSSetState(g_pRS_CullFront);


  ID3D11ShaderResourceView* nullRTV = nullptr;
  g_pGAPI->m_pDeviceContext->PSSetShaderResources(1, 1, &nullRTV);
  
  g_pGAPI->m_pDeviceContext->OMSetRenderTargets(1,
                                                &g_rtReflection.m_pRTV,
                                                g_dsReflection.m_pDSV);
  
  g_myModel.setBuffers(g_pGAPI); 
  g_pGAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &g_myTexture.m_pSRV);

  g_myModel.draw(g_pGAPI);
  ////////////////////////////////////////////////////////////////////////////////////////////
  Matrix4 floorScale;
  floorScale.identity();
  floorScale.scale(0.08f);
  rotation.rotateY(rotationAngle);
  g_WVP.world = floorScale * rotation * translation2;
  g_WVP.world.transpose();

  memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
  g_pGAPI->writeToBuffer(g_pCB_WVP, matrix_data);

  g_pGAPI->m_pDeviceContext->RSSetState(g_pRS_Default);

  g_TerrainModel.setBuffers(g_pGAPI);


  g_pGAPI->m_pDeviceContext->OMSetRenderTargets(1, 
                                                &g_pGAPI->m_pBackBufferRTV, 
                                                g_pGAPI->m_pBackBufferDSV);

  g_pGAPI->setPixelShader(g_pPixelShader_Reflect);

  g_pGAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &g_TerrainTexture.m_pSRV);
  g_pGAPI->m_pDeviceContext->PSSetShaderResources(1, 1, &g_rtReflection.m_pSRV);


  g_TerrainModel.draw(g_pGAPI);


  g_pGAPI->m_pDeviceContext->PSSetShader(g_pPixelShader->m_pPixelShader, nullptr, 0);
  ////////////////////////////////////////////////////////////////////////////////////////////
  
  g_pGAPI->m_pSwapChain->Present(0, 0);

  return SDL_APP_CONTINUE;  /* carry on with the program! */
}



/* This function runs once at shutdown. */
void
SDL_AppQuit(void* appstate, SDL_AppResult result) {
  /* SDL will clean up the window/renderer for us. */
  if(g_pWindow) {
    SDL_DestroyWindow(g_pWindow);
    g_pWindow = nullptr;
  }
  SDL_Quit();
}
